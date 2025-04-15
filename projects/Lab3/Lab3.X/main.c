/* 
 * File:   main.c
 * Author: ????
 *
 * Created on March 11, 2023, 11:48 AM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/attribs.h>
#include <string.h>
#include <xc.h>
#include <time.h>
#include "BOARD.h"
#include "I2C.h"
#include "MessageIDs.h"
#include "Protocol.h"
#include "uart.h"
#include "NonVolatileMemory.h"

/*
 * 
 */
int main(int argc, char** argv)
{
    char send[2048];
    int address;
    unsigned char page_content[64];
    unsigned char payload[MAXPAYLOADLENGTH];
    uint8_t id, len;
    unsigned char data[64];
    
    
    BOARD_Init();
    NonVolatileMemory_Init();
    Protocol_Init(21);
    
    sprintf(send, "Module 4 %s %s.", __DATE__, __TIME__);
    Protocol_SendDebugMessage(&send);
    sprintf(send, "-----------------------------------");
    Protocol_SendDebugMessage(&send);
    while(1){
//        I2C_WriteReg(0, 102, 7);
        quick_access();
         if (buffer_on() == success){
            Protocol_GetInPacket(&id, &len, &payload);
            address = payload[2];
            address = address << 8;
            address = payload[3] + address;
            switch(id){
            case ID_NVM_READ_BYTE:
                data[0] = NonVolatileMemory_ReadByte(address);
                Protocol_SendPacket(2, ID_NVM_READ_BYTE_RESP, data);
                break;
            case ID_NVM_WRITE_BYTE:
                NonVolatileMemory_WriteByte(address, payload[4]);
                Protocol_SendPacket(1, ID_NVM_WRITE_BYTE_ACK, 0);
                break;
            case ID_NVM_READ_PAGE:
                NonVolatileMemory_ReadPage(address, 64, data);
                Protocol_SendPacket(65, ID_NVM_READ_PAGE_RESP, data);
                break;
            case ID_NVM_WRITE_PAGE:
                NonVolatileMemory_WritePage(address, 64, payload);
                Protocol_SendPacket(1, ID_NVM_WRITE_PAGE_ACK, 0);
                break;
            }
            buffer_using(2);
            head_rotation();
         }
    }
    return (EXIT_SUCCESS);
}

