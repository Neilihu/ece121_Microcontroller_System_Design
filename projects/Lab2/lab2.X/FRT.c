#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "BOARD.h"
#include "FRT.h"


unsigned int ms_counter = 0, us_counter = 0;
void __ISR(_TIMER_5_VECTOR, ipl7auto) Timer5IntHandler(void) {
    if (IFS0bits.T5IF){
        IFS0bits.T5IF = 0;
        ms_counter++;     
    }
}


void FRT_Init(void){
    T5CON = 0X0;
    
    T5CONbits.TCKPS = 0b011;//1:8 prescale
    
    TMR5 = 0x0;//clean T5
    
    PR5 = 5000;//40M/1KHZ/8  5000 1ms 5 1us
    
    IPC5bits.T5IP = 7;//priority
    IPC5bits.T5IS = 1;//sub-priority
    
    IEC0bits.T5IE = 1;//enable interrupt T5
    IFS0bits.T5IF = 0;
    T5CONbits.ON = 1; // start T5
    TRISDbits.TRISD0 = 0;
}



unsigned int FRT_ms(void){//1 tick 1ms
    return ms_counter;
}



unsigned int FRT_us(void){//1 tick 1us
    unsigned int us = TMR5;
    us = us/5;// convert to us
    us_counter = us + ms_counter*1000;
    return us_counter;
}