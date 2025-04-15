#include <sys/attribs.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xc.h>
#include "Protocol.h"
#include "uart.h"
#include "MessageIDs.h"
#include "BOARD.h"
/*
 *structure for store a package
 * use: use the package or not. easy way for clean a package
 * HEAD: only contains 0xCC
 * tail: only contains 0xB9
 * ID: bassed on MessageIDs.h
 * len: the payload length for a package and including ID
 * checkSum:check if payload is right
 * payload: only contains information, no ID in the head
 */
typedef struct rxpT {
    uint8_t use;
    uint8_t HEAD;
    uint8_t tail;
    uint8_t ID;      
    uint8_t len;
    uint8_t checkSum; 
    unsigned char payLoad[MAXPAYLOADLENGTH];
 }rxpADT; 

 /*
  *structure for package buffer
  * head:place for reading a package
  * tail:place for adding a package
  * overlap: only 1 if head = tail and buffer maximum
  * pac: place to store a complete package. type is rxpADT
  */
typedef struct ghj{
    int head;
    int tail;
    int overlap;
    rxpADT pac[PACKETBUFFERSIZE];
}buff; 

static buff buffer;
unsigned char package[MAXPAYLOADLENGTH+5];
char end;

/*******************************************************************************
 *************************** PRIVATE FUNCTIONS *********************************
 ******************************************************************************/
int buffer_init(void);

int check_checksum(){
    unsigned char check = buffer.pac[buffer.head].ID;
    for (int i = 0; i < buffer.pac[buffer.head].len-1; i++){
        check = Protocol_CalcIterativeChecksum(buffer.pac[buffer.head].payLoad[i], check);
    }
    if (check == buffer.pac[buffer.head].checkSum){
        return success;
    } else {
        return false;
    }
}

unsigned char Protocol_CalcIterativeChecksum(unsigned char charIn, unsigned char curChecksum){
    unsigned char k;
    k = curChecksum >> 1;
    k += curChecksum << 7;
    k += charIn;
    return k;
}

int Protocol_Init(unsigned long baudrate){//need more things
    Uart_Init(baudrate);
    buffer_init();
    TRISE = 0x00;//leds initialize
    LATE = 0x00;
    end = off;
    return success;
}

uint8_t Protocol_QueuePacket (){//
    buffer.pac[buffer.head].HEAD = GetChar();
    buffer.pac[buffer.head].len = GetChar();
    buffer.pac[buffer.head].ID = GetChar();
    for (int i = 0; i <buffer.pac[buffer.head].len -1; i++){
        buffer.pac[buffer.head].payLoad[i] = GetChar();
    }
    buffer.pac[buffer.head].tail = GetChar();
    buffer.pac[buffer.head].checkSum = GetChar();
    if (GetChar() != 0x0D){// clean and check 0x0D
        buffer.pac[buffer.head].checkSum++;
    }
    if (GetChar() != 0x0A){// clean and check 0x0A
        buffer.pac[buffer.head].checkSum++;
    }
    tail_rotation();
    return buffer.overlap;
}

int Protocol_GetInPacket(uint8_t *type, uint8_t *len, unsigned char *msg){// no use
    if ((buffer.overlap == 0) && (buffer.head == buffer.tail)){
        return false;
    } else {
        *type = buffer.pac[buffer.head].ID;
        *len = buffer.pac[buffer.head].len;
        for (int j = 0; j < buffer.pac[buffer.head].len-1; j++){
             msg[j] = buffer.pac[buffer.head].payLoad[j];
        }
        return success;
    }
}

int Protocol_SendPacket(unsigned char len, unsigned char ID, unsigned char *Payload){//payload without id
    unsigned char check = 0;
    package[0] = 0xCC;
    package[1] = len;
    package[2] = ID;
    check = ID;//first checksum is ID
    for (int i = 0; i < (len-1); i++){
        package[i+3] = Payload[i];
        check = Protocol_CalcIterativeChecksum(Payload[i], check);
    }
    package[len+2] = 0xB9;
    package[len+3] = check;
    package[len+4] = 0x0D;
    package[len+5] = 0x0A;
    
    for(int j = 0; j < len+6; j++){
        if (PutChar(package[j]) == false){
            return false;
        }
    }
    return success;
}

int Protocol_SendDebugMessage(char *Message){
    unsigned char check = 0;
    package[0] = 0xcc;
    package[1] = strlen(Message);
    package[2] = ID_DEBUG;
    check = ID_DEBUG;
    for (int i  = 0; i < package[1]-1; i++){
        package[i+3] = Message[i];
        check = Protocol_CalcIterativeChecksum(Message[i], check);
    }
    package[package[1]+2] = 0xB9;
    package[package[1]+3] = check;
    package[package[1]+4] = 0x0D;
    package[package[1]+5] = 0x0A;
    
    for(int j = 0; j < package[1]+6; j++){
        if (PutChar(package[j]) == false){
            return false;
        }
    }
    return success;
}

unsigned char Protocol_ReadNextPacketID(){
    return (buffer.pac[buffer.head].ID);
}

int buffer_init(){
    rxpADT test[PACKETBUFFERSIZE];
    buffer.head = 0;
    buffer.tail = 0;
    buffer.overlap = off;
    for (int i = 0; i < PACKETBUFFERSIZE; i++){
        buffer.pac[i]= test[i];
        buffer.pac[i].HEAD = 0;
        buffer.pac[i].ID = 0;
        buffer.pac[i].checkSum = 0;
        buffer.pac[i].len = 0;
        buffer.pac[i].tail = 0;
        buffer.pac[i].use = off;
        for (int j = 0; j < MAXPAYLOADLENGTH; j++){
            buffer.pac[i].payLoad[j] = 0;
        }
    }
    return success;
}

void head_rotation(){
    if (buffer.head == (PACKETBUFFERSIZE -1)){
            buffer.head = 0;
    } else {
        buffer.head++;
    }
    if (buffer.overlap == 1){
        buffer.overlap = off;
    }
}

void tail_rotation(){
    if (buffer.tail == (PACKETBUFFERSIZE -1)){//circler buffer
        buffer.tail = 0;
    } else {
        buffer.tail++;
    }
    if (buffer.tail == buffer.head){//check full
        buffer.overlap = on;
    }
}

void buffer_using(int i){
    if (i == 1){
        buffer.pac[buffer.head].use = on;
    } else {
        buffer.pac[buffer.head].use = off;
    }
}
void change_ID(int num){
    buffer.pac[buffer.head].ID = num;
}

int buffer_on(){
    if (buffer.pac[buffer.head].use == on){
        return success;
    } else {
        return false;
    }
}

int quick_access(){
    if (end == on){// signal from uart.c
        Protocol_QueuePacket();//put package into package buffer
        if (check_checksum() == success){//check if package is right
            buffer_using(1);// package is useful
            end = off;// allow new package to add into package buffer
            return SUCCESS;
        } else {//discard current package
            change_ID(125);
            head_rotation();
            end = off;// allow new package to add into package buffer
            return ERROR;
        }
    } else {
        return ERROR;
    }
}