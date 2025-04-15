#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "BOARD.h"
#include "FRT.h"
#include "PingSensor.h"
#include "FRT.h"

unsigned short time = 0;
void __ISR(_TIMER_4_VECTOR) Timer4IntHandler(void){
    IFS0bits.T4IF = 0;
    ten_us_Trigger();
}

void __ISR(_INPUT_CAPTURE_3_VECTOR) __IC3Interrupt(void){
    IFS0bits.IC3IF = 0;
    if (PORTDbits.RD10 == 1){
        T2CONbits.ON = 1;//open t2
    }
    if (PORTDbits.RD10 == 0){
        T2CONbits.ON = 0;//off t2
        TMR2 = 0x0;//clear t2
    }
    if(IC3CONbits.ICBNE){
        time = IC3BUF & 0x0000FFFF;
    }
    if(IC3CONbits.ICBNE){
        time = IC3BUF & 0x0000FFFF;
    }
}

int PingSensor_Init(void){
    T2CON = 0x0;
    T4CON = 0x0;
    IC3CON = 0x0;
    
    T4CONbits.TCKPS = 0b110;//1:64  
    TMR4 = 0x0;
    PR4 = 37500;//60ms
    IPC4bits.T4IP = 4;
    IPC4bits.T4IS = 3;
    IEC0bits.T4IE = 1;//open interrupt t4
    IFS0bits.T4IF = 0;
    
    T2CONbits.TCKPS = 0b011;//1:8
    TMR2 = 0x0;
    PR2 = 58450;//11.69ms
    
    IC3CONbits.C32 = 0;//16bits
    IC3CONbits.ICTMR = 1;//timer 2
    IC3CONbits.ICI = 0b00;// interrupt in all edge
    IC3CONbits.ICM = 0b001;// detects every edge
    IPC3bits.IC3IP = 7;
    IPC3bits.IC3IS = 3;
    IEC0bits.IC3IE = 1;//open interrupt ic3
    IFS0bits.IC3IF = 0;
    TRISFbits.TRISF1 = 0;//pin 4 in io shield
    LATFbits.LATF1 = 0;
    
    T4CONbits.ON = 1;
    IC3CONbits.ON = 1;
    return SUCCESS;
}

unsigned short PingSensor_GetDistance(void){
    unsigned short distance = 0;
    distance = time/29 ;//14.7 ticks per mm
    return distance;//0-2015
}

void ten_us_Trigger(void){
    for(char t = 0; t < 50; t++){
        LATFbits.LATF1 = 1;
    }
    LATFbits.LATF1 = 0;
}