#include <sys/attribs.h> 
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BOARD.h"
#include "I2C.h"


void wait_to_ideal(void);

unsigned int I2C_Init(unsigned int Rate){
    I2C1CONbits.ON = 0;//off i2c
    I2C1CON = 0x0;//clear every thing
    I2C1CONbits.DISSLW = 0;//no slew rate control for 100khz
    I2C1BRG = 20000000/Rate - 4;
    I2C1CONbits.ON = 1;
    return SUCCESS;
}

unsigned char I2C_ReadRegister(unsigned char I2CAddress, int deviceRegisterAddress){
    unsigned char result = 12;
    int temper;
    
    wait_to_ideal();
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1);//send start signal 
    
    I2C1TRN = 0b10100000;//write id with write
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    temper = deviceRegisterAddress;
    I2C1TRN = temper >> 8;// send high address
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    I2C1TRN = temper;// send low address
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    wait_to_ideal();
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN == 1);//send start signal 

    I2C1TRN = 0b10100001;//write id with read
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while(I2C1STATbits.ACKSTAT == 1);//wait ack
    
    I2C1CONbits.RCEN = 1;// accept data
    while(I2C1CONbits.RCEN == 1);
    while(I2C1STATbits.RBF == 0);
    result = I2C1RCV;// get data

    wait_to_ideal();
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while(I2C1CONbits.ACKEN == 1);//send nack
    
    wait_to_ideal();
    I2C1CONbits.PEN = 1;
    //I2C_Init(I2C_DEFAULT_RATE);
    return result;
}

unsigned char I2C_WriteReg(unsigned char I2CAddress, int deviceRegisterAddress, unsigned char data){
    int temper;
    
    wait_to_ideal();
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1);//send start signal 
    
    I2C1TRN = 0b10100000;//write id with write
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    temper = deviceRegisterAddress;
    I2C1TRN = temper >> 8;// send high address
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    I2C1TRN = temper;// send low address
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    I2C1TRN = data;
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    wait_to_ideal();
    I2C1CONbits.PEN = 1;
    //I2C_Init(I2C_DEFAULT_RATE);
    return data;   
}

int I2C_ReadInt(char I2CAddress, char deviceRegisterAddress, char isBigEndian){//1 big 0 little

        
}

unsigned char I2C_WritePage(unsigned char I2CAddress, int deviceRegisterAddress, char length, unsigned char* data){
    int temper;
    
    wait_to_ideal();
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1);//send start signal 
    
    I2C1TRN = 0b10100000;//write id with write
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    temper = deviceRegisterAddress;
    I2C1TRN = temper >> 8;// send high address
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    I2C1TRN = temper;// send low address
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT);//wait ack
    
    for (int i = 0; i < length; i++){
        I2C1TRN = data[i + 4];
        while(I2C1STATbits.TBF);
        wait_to_ideal();
        while (I2C1STATbits.ACKSTAT);//wait ack
    }
    
    wait_to_ideal();
    I2C1CONbits.PEN = 1;
    //I2C_Init(I2C_DEFAULT_RATE);
    return SUCCESS;   
}

void I2C_ReadPage(unsigned char I2CAddress, int deviceRegisterAddress, char length, unsigned char* data){
    int temper;
    
    wait_to_ideal();
    I2C1CONbits.SEN = 1;
    while(I2C1CONbits.SEN == 1);//send start signal 
    
    I2C1TRN = 0b10100000;//write id with write
    while(I2C1STATbits.TBF);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    temper = deviceRegisterAddress;
    I2C1TRN = temper >> 8;// send high address
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    I2C1TRN = temper;// send low address
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while (I2C1STATbits.ACKSTAT == 1);//wait ack
    
    wait_to_ideal();
    I2C1CONbits.RSEN = 1;
    while(I2C1CONbits.RSEN == 1);//send start signal 

    I2C1TRN = 0b10100001;//write id with read
    while(I2C1STATbits.TBF == 1);
    wait_to_ideal();
    while(I2C1STATbits.ACKSTAT == 1);//wait ack
    
    for (int i = 0; i < (length - 1); i++){
        I2C1CONbits.RCEN = 1;// accept data
        while(I2C1CONbits.RCEN == 1);
        while(I2C1STATbits.RBF == 0);
        data[i] = I2C1RCV;// get data

        wait_to_ideal();
        I2C1CONbits.ACKDT = 0;
        I2C1CONbits.ACKEN = 1;
        while(I2C1CONbits.ACKEN == 1);//send ack
    }
    
    I2C1CONbits.RCEN = 1;// accept data
    while(I2C1CONbits.RCEN == 1);
    while(I2C1STATbits.RBF == 0);
    data[length - 1] = I2C1RCV;// get data

    wait_to_ideal();
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    while(I2C1CONbits.ACKEN == 1);//send nack
    
    wait_to_ideal();
    I2C1CONbits.PEN = 1;
    //I2C_Init(I2C_DEFAULT_RATE);
    return;
}
void wait_to_ideal(void){
    while(I2C1CON & 0x1F);
    while(I2C1STATbits.TRSTAT);
}
