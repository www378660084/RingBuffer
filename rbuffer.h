/*
 *  * rbuffer.h
 *  *
 *  *  ring buffer by YongHai Yang
 *  *  thread safe:
 *  *      support single reader thread and single writer thread
 *  *      don't read in two thread
 *  *      don't write in two thread
 *  *
 *  *  Created on: 2015-5-28
 *  *      Author: Administrator
 *  */

#ifndef RBUFFER_H_
#define RBUFFER_H_

void* rbuffer_init(void* ptr,int len);
unsigned int rbuffer_data_len(void* ptr);
unsigned int rbuffer_free_len(void* ptr);

/***
 *  * read data from ring buffer,use with rbuffer_write
 *  * return:size of data has been read
 *  */
unsigned int rbuffer_read(void* ptr,void* buf,unsigned int size);

/**
 *  * write data to ring buffer,use with rbuffer_read
 *  * return:size of data has been written
 *  */
unsigned int rbuffer_write(void* ptr,void* data,unsigned int size);

/***
 * * send packet,use with rbuffer_receive
 * * return:size for success,0 for fail
 * */
int rbuffer_send(void* ptr,void* data,unsigned int size);

/***
 * * receive packet,use with rbuffer_send
 * * return:size for success,0 for fail
 * */
int rbuffer_receive(void* ptr,void* buffer,unsigned int size);


#endif /* RBUFFER_H_ */

