/* 
 * File:   newmain.c
 * Author: ????
 *
 * Created on February 14, 2023, 11:29 AM
 */


#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include <time.h>
#include "BOARD.h"
#include "FRT.h"
#include "Protocol.h"
#include "RotaryEncoder.h"
#include "MessageIDs.h"
#include "RCServo.h"
#include "uart.h"
#include "PingSensor.h"

/*
 * 
 */
extern char end;
extern char flag;
unsigned short record, record2;
signed int angle;
int state = 2, error = 2;
int main(int argc, char** argv)
{
    unsigned int ms1, ms2, pulse;
    unsigned short distance, encoder;
    unsigned char data[5];
    char send[2048];
    unsigned char payload[MAXPAYLOADLENGTH];
    uint8_t id, len;
    
    BOARD_Init();
    Protocol_Init(21);
    FRT_Init();
    RCServo_Init();
    
    sprintf(send, "Module 4 %s %s.", __DATE__, __TIME__);
    Protocol_SendDebugMessage(&send);
    sprintf(send, "-----------------------------------");
    Protocol_SendDebugMessage(&send);
    ms1 = FRT_ms();
    ms2 = FRT_ms();
    record = 200;
    record2 = 5000;
    while(flag == 0);
    RCServo_SetPulse(600);
    while(1){
        quick_access();
        if (buffer_on() == success){
            Protocol_GetInPacket(&id, &len, &payload);
            switch(payload[0]){
            case 0://ping
                state = 0;
                record = 200;
                SPI2CON = 0;//off encoder
                PingSensor_Init();//on ping
                while(flag == 0);
                RCServo_SetPulse(1500);
                break;
            case 1://encoder
                state = 1;
                record2 = 8192;
                T4CON = 0x0;//off ping
                IC3CON = 0x0;//off ping
                RotaryEncoder_Init(ENCODER_BLOCKING_MODE);//on encoder
                while(flag == 0);
                RCServo_SetPulse(1500);
                break;
            }
            buffer_using(2);
            head_rotation();
        }
        if ((ms1+50) <= FRT_ms()){//20hz
            data[4] = error;
            data[3] = angle;
            data[2] = angle >> 8;
            data[1] = angle >> 16;
            data[0] = angle >> 24;
            Protocol_SendPacket(6, ID_LAB2_ANGLE_REPORT, data);
            ms1 = FRT_ms();
        }
        if ((ms2+25) <= FRT_ms()){//40Hz
            switch (state){
                case 1://rotery encoder
                    encoder = RotaryEncoder_ReadRawAngle();
                    if (encoder == 0){
                        encoder = record2;
                    }
                    if (abs(record2 - encoder) < 64){//debuncing
                        encoder = record2;
                    } else {
                        record2 = encoder;
                    }
                    angle = encoder*5625/128 - 360000;
                    if (encoder < 10239){
                        encoder = 600;
                        error = 4;
                    } else if (encoder > 14335){
                        encoder = 2400;
                        error = 1;
                    } else {
                        encoder = encoder - 8191;
                        encoder = encoder - 2048;
                        encoder = (encoder*225)/512;
                        encoder = encoder + 600; 
                        error = 2;
                    }
                    pulse = encoder;
                    
                    while(flag == 0);
                    RCServo_SetPulse(pulse);
                    break;
                case 0://ping
                    distance = PingSensor_GetDistance();
                    if (abs(record - distance) > 200){//debuncing
                        encoder = record;
                    } else if (abs(record - distance) < 3){
                        encoder = record;
                    } else {
                        record = encoder;
                    }
                    
                    angle = distance*100;
                    if (distance < 250){
                        distance = 600;
                        error = 4;
                    } else if (distance > 1250){
                        if (distance > 3599){
                            angle = 360000;
                        }
                        distance = 2400;
                        error = 1;
                    } else {
                        distance = distance - 250;
                        distance = (distance*9)/5;
                        distance = distance + 600;
                        error = 2;
                    }
                    pulse = distance;
                    
                    while(flag == 0);
                    RCServo_SetPulse(pulse);
                    break;
                case 2:// initiate
                    error = 0b010;
                    pulse = 1500;
                    angle = 30000;
            }
            ms2 = FRT_ms();
        }
    }
    return (EXIT_SUCCESS);
}