/* 
 * File:   LED_Neili.h
 * Author: Neili Hu
 * version 1.0
 * Created on January 19, 2023, 3:21 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <xc.h>

#define LD1 LATEbits.LATE0 //LD1-8 represents LED1-LED8
#define LD2 LATEbits.LATE1
#define LD3 LATEbits.LATE2
#define LD4 LATEbits.LATE3
#define LD5 LATEbits.LATE4
#define LD6 LATEbits.LATE5
#define LD7 LATEbits.LATE6
#define LD8 LATEbits.LATE7
#define on 1
#define off 0
// led name: right is 1 left is 8, they have binary 0b87654321 or 0x00

int LED_READ(void);//return current LED status. light is 1 off is 0. will change to binary

void LED_SET(int abc);//change current LED into abc status

void LED_OFF(void);//close all led

void LED_ON(void);//light all led

void LED_INI(void);//initialize all led

int LED_PATTERN_1(int count, int jump);//light led in binary way 0-255, count per run, can jump middle remaining or not

int LED_PATTERN_2(int start);//light led in decimal way 1-8, once per run, keep light

int LED_PATTERN_3(int start);//light led in decimal way 1-8, once per run, not keep light after head
