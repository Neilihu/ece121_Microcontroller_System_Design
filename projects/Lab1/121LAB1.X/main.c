/* 
 * File:   main.c
 * Author: ????
 *
 * Created on February 12, 2023, 1:39 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/attribs.h> 
#include <string.h>
#include <xc.h>
#include "Protocol.h"
#include "uart.h"
#include "MessageIDs.h"
#include "BOARD.h"
/*
 * 
 */
int end;

int main(int argc, char** argv)
{   
    unsigned long pong_data;
    unsigned char pong_send[4];
    unsigned char led;
    unsigned char payload[MAXPAYLOADLENGTH];
    uint8_t id, len;
    
    BOARD_Init();
    Protocol_Init(21);
    

    while(1){
        if (end == on){// signal from uart.c
            Protocol_QueuePacket();//put package into package buffer
            if (check_checksum() == success){//check if package is right
                buffer_using(1);// package is useful
            } else {//discard current package
                change_ID(125);
                head_rotation();
            }
            end = off;// allow new package to add into package buffer
        }
        if (buffer_on() == success){
            Protocol_GetInPacket(&id, &len, &payload);
            switch(Protocol_ReadNextPacketID()){// deal with packages
            case ID_LEDS_SET:
                LATE = payload[0];
                break;
            case ID_LEDS_GET:
                led = PORTE;
                while (Protocol_SendPacket(2, ID_LEDS_STATE, &led) == false){
                    ;
                }
                break;
            case ID_PING:
                pong_data = 0;
                for(int l = 0; l < 4; l++){
                    pong_data = pong_data + payload[l];
                    if (l == 3){
                        break;
                    }
                    pong_data = pong_data << 8;
                }
                pong_data = pong_data >> 1;
                pong_send[0] = pong_data >> 24;
                pong_send[1] = pong_data >> 16;
                pong_send[2] = pong_data >> 8;
                pong_send[3] = pong_data;
                
                while (Protocol_SendPacket(5, ID_PONG, pong_send) == false){
                    ;
                }
                break;
            }
            buffer_using(2);
            head_rotation();
        }
    }

    return (EXIT_SUCCESS);
}

