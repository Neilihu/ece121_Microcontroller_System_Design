/*RX_get
 *get data from TX_BUFFER. buffer has size "size"
 *return: number get from buffer or -1 if nothing in the buffer.
 */
int RX_get(void);//getchar

/*TX_add
 *add asc2 code into TX-buffer. buffer has size "size"
 *return: success, buffer isn't full.  
 *          false, if buffer is full and no data will be load into buffer.
 */
int TX_add(int msg);//putchar
