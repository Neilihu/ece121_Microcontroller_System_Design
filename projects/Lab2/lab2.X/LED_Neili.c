#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "LED_Neili.h"



int pattern2 = 0, pattern3 = 0;

int LED_READ(){
    int i = 0;
    i = PORTE;
    return i;
}

void LED_SET(int abc){
    LATE = abc;
}

void LED_OFF(void){
    LATE = 0X00;
}

void LED_ON(void){
    LATE = 0xFF;
}

void LED_INI(void){
    TRISE = 0x00;
    pattern2 = 0;
    pattern3 = 0;
    LATE = 0x00;
}

int LED_PATTERN_1(int count, int jump){
    if (jump){
        LATE += count;
    } else {
        for (int i = 0; i < count; i++){
            if (LATE == 0xFF){
                LATE = 0x00;
            } else {
                LATE++;
            }
        }
    }
    return PORTE;
}

int LED_PATTERN_2(int start){
    if (start == on){
        if (pattern2 == 1){
            LATE = 0b00000001;
        } else if ((pattern2 >= 2)&&(pattern2 <= 3)){
            LATE = 0b00000011;
        } else if ((pattern2 >= 4)&&(pattern2 <= 7)){
            LATE = 0b00000111;
        } else if ((pattern2 >= 8)&&(pattern2 <= 15)){
            LATE = 0b00001111;
        } else if ((pattern2 >= 16)&&(pattern2 <= 31)){
            LATE = 0b00011111;
        } else if ((pattern2 >= 32)&&(pattern2 <= 63)){
            LATE = 0b00111111;
        } else if ((pattern2 >= 64)&&(pattern2 <= 127)){
            LATE = 0b01111111;
        } else if ((pattern2 >= 128)&&(pattern2 <= 254)){
            LATE = 0b11111111;
        } else if (pattern2 == 255){
            LATE = 0x00;
        }
    }
    return PORTE;
}

int LED_PATTERN_3(int start){
    if (start == on){
        pattern3 = PORTE;
        if (pattern3 == 1){
            LATE = 0b00000001;
        } else if (pattern3 == 2){
            LATE << 1;
        } else if (pattern3 == 4){
            LATE << 1;
        } else if (pattern3 == 8){
            LATE << 1;
        } else if (pattern3 == 16){
            LATE << 1;
        } else if (pattern3 == 32){
            LATE << 1;
        } else if (pattern3 == 64){
            LATE << 1;
        } else if (pattern3 == 128){
            LATE << 1;
        }
    }
    return PORTE;
}