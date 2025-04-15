#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "BOARD.h"
#include "FRT.h"
#include "RCServo.h"

char flag = 0;
char counter = 1;
unsigned int data;
void __ISR(_OUTPUT_COMPARE_3_VECTOR) __OC3Interrupt(void){
    IFS0bits.OC3IF = 0;
    flag = 1;
}

void __ISR(_TIMER_3_VECTOR) Timer3IntHandler(void) {
    IFS0bits.T3IF = 0;   
    counter++;
    if (counter == 11){//change frequency current period = 11*?ms=50ms=20HZ
        OC3CONbits.OCM = 0b100;
        counter = 1;
    }
}

int RCServo_Init(void){
    T3CON = 0X0;
    
    T3CONbits.TCKPS = 0b011;//1:8 prescale
    
    TMR3 = 0x0;//clean T3
    PR3 = 25000;//40M/1KHZ/8  5000 1ms 5 us 5 PERIODS (5MS 1 PERIOD)
    
    IPC3bits.T3IP = 4;//priority
    IPC3bits.T3IS = 1;//sub-priority
    
    IEC0bits.T3IE = 1;//enable interrupt T3
    IFS0bits.T3IF = 0;
    
    OC3CON = 0x0;
    OC3CONbits.OC32 = 0;//16 bits compare
    OC3CONbits.OCTSEL = 1;//timer 3 as comparator
    OC3CONbits.OCM = 0b100;
    //total 50ms 1 period: 1ms nothing, 2ms for pulse (1.54-2.44)), then until 50 is nothing
    OC3R = 5000; // start at 1ms
    OC3RS = 10050; // run 1500 ticks  time = (ticks-600)>>1*5+7800
    data = OC3RS;
    IPC3bits.OC3IP = 6;//priority
    IPC3bits.OC3IS = 2;//sub priority
    
    IEC0bits.OC3IE = 1;//enable interrupt C1
    IFS0bits.OC3IF = 0;

    T3CONbits.ON = 1; // start T3
    OC3CONbits.ON = 1;// start OC1
    return SUCCESS;
}

int RCServo_SetPulse(unsigned int inPulse){
    flag = 0;
    if ((inPulse >=600) && (inPulse <= 2400)){
        inPulse = (inPulse-600)/2*5 +7800;
        OC3RS = inPulse;
        data = OC3RS;
        return SUCCESS;
    }
    return ERROR;
}

unsigned int RCServo_GetPulse(void){
    unsigned int result;
    result = data;
    result = result - 7800;
    result = result / 5;
    result = result *2;
    result = result + 600;
    return result;
}

unsigned int RCServo_GetRawTicks(void){// NEED MORE
    unsigned int result;
    if (counter == 1){
        if (TMR3 < 5000){
            result = 5000-TMR3;
        } else {
            result = 255000 -TMR3;
        }
    } else {
        result = 255000 - TMR3 - counter*25000;
    }
    return result;
}

