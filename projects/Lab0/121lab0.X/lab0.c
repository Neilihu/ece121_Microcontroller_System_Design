/* 
 * File:   lab0.c
 * Author: Neili Hu
 *
 * Created on January 11, 2023, 12:18 PM
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
#define BTN1 PORTFbits.RF1 //BTN1-4 represents BUTTON1-BUTTON4
#define BTN2 PORTDbits.RD5
#define BTN3 PORTDbits.RD6
#define BTN4 PORTDbits.RD7
#define SW1 PORTDbits.RD8 // SW1 represents a switch which is for easy-demonstration only
#define off 0
#define on 1
#define PUSH_5 310     // 5ms delay should be 327 ,but after adjustted, 310 is better than it  
#define PUSH_250 50

int main(int argc, char** argv)
{
    BOARD_Init();
    //initialize for led and set them to output
    TRISE = 0x00;
    
    //set every led to off so no led is lighting at start
    LATE = 0X00;
    
    int i = 0, count = 0;
    LATEbits.LATE0=1;
    while(1){
        ;
    }
    while(1){
        if (SW1 == off){ // I use a switch to change between two demos
            count = 0;
            LATE=0x00;
            if (BTN1 == on){ //if button1 pressed first 2 leds on
                LD1 = on;
                LD2 = on;
            } else {        //if button1 released first 2 leds off
                LD1 = off;
                LD2 = off;
            }
            if (BTN2 == on){ //if button2 pressed second 2 leds on
                LD3 = on;
                LD4 = on;
            } else {         //if buttton2 released second 2 leds off
                LD3 = off;
                LD4 = off;
            }
            if (BTN3 == on){ //if button3 pressed third 2 leds on
                LD5 = on;
                LD6 = on;
            } else {       //if button 3 released third 2 leds off
                LD5 = off;
                LD6 = off;
            }
            if (BTN4 == on){ // if button4 pressed last 2 leds on
                LD7 = on;
                LD8 = on;
            } else {        // if button4 released last 2 leds off
                LD7 = off;
                LD8 = off;
            }
        } else if (SW1 == on){
            for (i; i<PUSH_5; i++){ // 5ms delay
                asm("nop");
            }
            count++; // a count is 5ms
            i = 0;
            if (count == PUSH_250){// after 250ms delay add an led count until 255
                count = 0;
                if (LATE == 0XFF){// reset in 255
                    LATE = 0X00;
                } else {
                    LATE++;
                }
            }
            if (BTN1 == on || BTN2 == on || BTN3 == on || BTN4 == on){
                LATE = 0X00;
            }
        }
    }
    return (EXIT_SUCCESS);
}