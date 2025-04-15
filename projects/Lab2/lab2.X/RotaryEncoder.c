#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "BOARD.h"
#include "RotaryEncoder.h"
#include "FRT.h"

#define cs LATGbits.LATG9


int RotaryEncoder_Init(char interfaceMode){
    switch(interfaceMode){
    case ENCODER_BLOCKING_MODE:
        SPI2CON = 0;// off spi2    
        SPI2BUF = 0;
        SPI2CONbits.SMP = 0;
        SPI2CONbits.CKE = 1;//device required
        SPI2CONbits.CKP = 0;//device required
        SPI2CONbits.MODE32 = 0;
        SPI2CONbits.MODE16 = 1;//16bits data
        SPI2CONbits.MSTEN = 1;//master
        
        TRISGbits.TRISG6 = 0;//output sck pin
        TRISGbits.TRISG9 = 0;//output cs
        TRISGbits.TRISG7 = 0;//output sdi
        TRISGbits.TRISG8 = 1;//input sdo
        SPI2BRG = 63;// 312.5KHz for sck
        
        SPI2CONbits.ON = 1;
        break;
    case ENCODER_INTERRUPT_MODE:
        ;
    }
    return SUCCESS;
}


unsigned short RotaryEncoder_ReadRawAngle(void){
    unsigned short result = 0;
    unsigned int time = 0;
    cs = 0;//start transfer
    SPI2BUF = 0xFFFF;
    while(SPI2STATbits.SPIRBF == 0);//WAITING
    cs = 1;//end first transfer, both direction ending together
    result = SPI2BUF;
    
    time = FRT_ms();
    while (time == FRT_ms());//delay 1 ms, may not necessary
    
    cs = 0;//second transfer
    SPI2BUF = 0xC000;//nop 
    while(SPI2STATbits.SPIRBF == 0);//both sending and receiving end in the same time
    cs = 1;//end second transfer
    result = SPI2BUF;//data that need
    
    if (result & 0x4000){
        result  = 0;
    } else {
        result = result << 2;
        result = result >> 2; 
    }
    return result;
}