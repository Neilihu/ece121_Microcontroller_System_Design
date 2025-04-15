#include <sys/attribs.h> 
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BOARD.h"
#include "I2C.h"
#include "NonVolatileMemory.h"

int NonVolatileMemory_Init(void){
    I2C_Init(I2C_DEFAULT_RATE);
    return SUCCESS;
}

unsigned char NonVolatileMemory_ReadByte(int address){
    unsigned char result;
    result = I2C_ReadRegister(0, address);
    return result;// only one contains data
}

char NonVolatileMemory_WriteByte(int address, unsigned char data){
    char result;
    result = I2C_WriteReg(0, address, data);
    return SUCCESS;
}

void NonVolatileMemory_ReadPage(int page, char length, unsigned char* data){
    //0-1562 page
    unsigned char address;
    
    address = page*64;
    I2C_ReadPage(0, address, length, data);
    return;
}

int NonVolatileMemory_WritePage(int page, char length, unsigned char* data){
    unsigned char address;

    address = page*64;
    I2C_WritePage(0, address, length, data);
    return SUCCESS;
}