/* 
 * File:   buffer.h
 * Author: ????
 *
 * Created on January 30, 2023, 3:35 PM
 */
void buffer_Init();

int TX_add(int msg);// return success if success, else if buffer full return fail

int TX_get(void);

int RX_add(char* msg);

int RX_get(void);

int TX_full(void);

int RX_full(void);
