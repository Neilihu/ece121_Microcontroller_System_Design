/*
 * File:   uart_note.c
 * Author: Neili, ECE121 W2023
 */
#include <sys/attribs.h> 
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BOARD.h"
#include "uart.h"
#include "Protocol.h"

#define size 200
/*
 *structure for both rx and tx buffers. 
 * head: getting data from array
 * tail: putting data into array
 * overlap: only 1 when head is the same as tail and buffer maximum
 * array: store unsigned char
 */
typedef struct{
    int head;
    int tail;
    int overlap;
    unsigned char array[size];
}buffers;

/*
 * simple stages for state machine
 */
typedef enum{
    wait,
    head,
    length,
    tail,
    checksum,
    end1,
    end2,          
}jlkjl;

static buffers TX_BUFFER;
static buffers RX_BUFFER;
int count = 0;
int stage = wait;
extern int end;//signal for protocol to put package into buffer
/*******************************************************************************
 * PRIVATE FUNCTIONS to add to your Uart.c file                                                         *
 ******************************************************************************/

unsigned char RX_get(void);//getchar
int TX_add(unsigned char msg);//putchar

/*RX_add
 *add asc2 code into RX-buffer. buffer has size "size"
 *return: success, buffer isn't full.  
 *          false, if buffer is full and no data will be load into buffer.
 */
int RX_add(unsigned char msg);

/*TX_get
 *get data from TX_BUFFER. buffer has size "size"
 *return: asc2 code from buffer or false if nothing in the buffer.
 */
int TX_get(void);

/*write
 *write data to monitor. use "" as input instead of ''
 *return:none
 */
//void write(char* msg);
/*NEW_LINE
 *move cursor to next line and left_most in monitor. 
 *return:none
 */
//void NEW_LINE(void);

/**
 * @Function _mon_putc(char c)
 * @param c - char to be sent
 * @return None.
 * @brief  overwrites weakly defined extern to use circular buffer
 *         instead of Microchip 
 * functions
 */
void _mon_putc(char c){
    while (U1STAbits.UTXBF);
    U1TXREG = c;
}

/****************************************************************************
 * Function: IntUart1Handler
 * Parameters: None.
 * Returns: None.
 * The PIC32 architecture calls a single interrupt vector for both the 
 * TX and RX state machines. Each IRQ is persistent and can only be cleared
 * after "removing the condition that caused it".  
 ****************************************************************************/
void __ISR(_UART1_VECTOR) IntUart1Handler(void) {
    unsigned char data;
    if (IFS0bits.U1RXIF){
        IFS0bits.U1RXIF = 0; //clean flag
        data = U1RXREG;
        if (end == off){// only allowed one package per time
            RX_add(data);
            switch(stage){//stage machine
            case wait:
                if (data == 0xCC){
                    stage = head;
                }
                break;
            case head:
                stage = length;
                count = data;
                break;
            case length:
                count--;
                if (count == 0){
                    stage = tail;
                }
                break;
            case tail:
                if (data == 0xB9){
                    stage = checksum;
                } else {
                    end = on;
                    stage = wait;
                }
                break;
            case checksum:
                stage = end1;
                break;
            case end1:
                if (data == 0x0D){
                    stage = end2;
                } else {
                    end = on;
                    stage = wait;
                }
                break;
            case end2:
                if(data == 0x0A){
                    stage = wait;
                    end = on;
                }
            }
        }   
    } else if (IFS0bits.U1TXIF){
        IFS0bits.U1TXIF = 0; // clean flag
        IEC0bits.U1TXIE = 0;// close INTERRUPT
        U1TXREG = TX_get(); // write data to output
        while(U1STAbits.TRMT == 0);//until no data in tx
    } else if (IFS0bits.U1EIF){
        IFS0bits.U1EIF = 0; // clean flag
        while(1);
    }  
}
//int main(int argc, char** argv){
//    BOARD_Init();
//    Uart_Init(21);
//    while(1);
//}

unsigned char RX_get(){
    unsigned char i;
    if ((RX_BUFFER.head == RX_BUFFER.tail) && (RX_BUFFER.overlap == off)){
        return false;
    } else {
        i = RX_BUFFER.array[RX_BUFFER.head];
        RX_BUFFER.array[RX_BUFFER.head] = 0;//clear old array
        
        if (RX_BUFFER.head == (size-1)){
            RX_BUFFER.head = 0;
        } else {
            ++RX_BUFFER.head;
        }
        
        RX_BUFFER.overlap = off;
        return i;
    }
}

int RX_add(unsigned char msg){
    if (RX_BUFFER.overlap == off){
        RX_BUFFER.array[RX_BUFFER.tail] = msg;
        
        if (RX_BUFFER.tail == (size - 1)){
            RX_BUFFER.tail = 0;
        } else {
            ++RX_BUFFER.tail;
        }
        
        if (RX_BUFFER.head == RX_BUFFER.tail){
            RX_BUFFER.overlap = on;
        }
        return success;
    } else if (RX_BUFFER.overlap == on){
        return false;
    }
}


int TX_get(){
    unsigned char i;
    if ((TX_BUFFER.head == TX_BUFFER.tail) && (TX_BUFFER.overlap == off)){
        return false;
    } else {
        i = TX_BUFFER.array[TX_BUFFER.head];
        TX_BUFFER.array[TX_BUFFER.head] == 0;//clear old array
        
        if (TX_BUFFER.head == (size - 1)){
            TX_BUFFER.head = 0;
        } else {
            ++TX_BUFFER.head;
        }
        
        TX_BUFFER.overlap = off;
        return i;
    }
}

int TX_add(unsigned char msg){//putchar
    if (TX_BUFFER.overlap == off){
        TX_BUFFER.array[TX_BUFFER.tail] = msg;//add msg into buffer
        
        if (TX_BUFFER.tail == (size - 1)){//move tail
            TX_BUFFER.tail = 0;
        } else {
            ++TX_BUFFER.tail;
        }
        
        if (TX_BUFFER.head == TX_BUFFER.tail){//see if in the same place
            TX_BUFFER.overlap = on;
        }
        while (U1STAbits.TRMT == 0);//waiting for empty tx
        IEC0bits.U1TXIE = 1; //open interrupt
        return success;
    } else if (TX_BUFFER.overlap == on){
        return false;
    }
}

void Uart_Init(unsigned long baudRate){
    //mode bits setting
    U1STACLR = 0xFFFFFFFF;
    
    U1MODEbits.SIDL = 0;
    U1MODEbits.IREN = 0;
    U1MODEbits.RTSMD = 0;//U1RTX in flow control
    U1MODEbits.UEN = 0b11;
    U1MODEbits.WAKE = 0;
    U1MODEbits.ABAUD = 0;
    U1MODEbits.RXINV = 0;
    U1MODEbits.BRGH = 0;// standard speed
    U1MODEbits.PDSEL = 0b00;//8-N
    U1MODEbits.STSEL = 0; //1 stop
    U1MODEbits.LPBACK = 0; 
    
    // sta bits seeting
    U1STAbits.ADM_EN = 0;
    U1STAbits.UTXISEL = 0b10;//tx interrupt when empty
    U1STAbits.UTXINV = 0;
    U1STAbits.UTXBRK = 0;
    U1STAbits.UTXEN = 1;//enable tx
    U1STAbits.URXEN = 1;// enable rx
    U1STAbits.URXISEL = 0b00;// rx interrupt when not empty
    U1STAbits.ADDEN = 0;
    U1STAbits.OERR = 0;
    
    U1BRG = baudRate; // set baud rate to baudRate
    U1MODEbits.ON = 1; // UART1 enable 
    
    //interrupt initializer
    IPC6bits.U1IP = 4;//SET PRIORITY
    IPC6bits.U1IS = 2;//SET SUB-PRIORITY
    IEC0bits.U1EIE = 1;// enable u1 erro
    IEC0bits.U1RXIE = 1;//enable u1 rx
    IEC0bits.U1TXIE = 0;// not able u1 tx yet
    
    //buffer initializer
    TX_BUFFER.head = 0;
    TX_BUFFER.tail = 0;
    TX_BUFFER.overlap = off;
    
    RX_BUFFER.head = 0;
    RX_BUFFER.tail = 0;
    RX_BUFFER.overlap = off;
    
    for (int i = 0; i < size; i++){
        TX_BUFFER.array[i] = 0;//no data saved
        RX_BUFFER.array[i] = 0;//no data saved
    }
}

//void write(char* msg){//test function
//    int count;
//    count = strlen(msg);
//    for (int i = 0; i <count ; i++){
//        TX_add(*msg);
//        msg++;
//    }
//}
//
//void NEW_LINE(){//test function
//    U1TXREG = 10;
//    U1TXREG = 13;
//}

int PutChar(unsigned char ch){
    return TX_add(ch);
}

unsigned char GetChar(){
    
    return RX_get();
}