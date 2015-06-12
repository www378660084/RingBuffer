/*
 *  * rbuffer_t.c
 *   *
 *    *  ring buffer by YongHai Yang
 *     *
 *      *  Created on: 2015-5-28
 *       *      Author: Administrator
 *        */
#include <string.h>
#include <stdio.h>
#include "rbuffer.h"

#define REMAIN 4

typedef struct{
	unsigned int size;
	unsigned int rpos;
	unsigned int wpos;
	char data[0];
}rbuffer_t;


void* rbuffer_init(void* ptr,int len){
	if(len <= sizeof(rbuffer_t)){
		return NULL;
	}

	rbuffer_t* buffer = ptr;
	buffer->size = len - sizeof(rbuffer_t);
	buffer->rpos = 0;
	buffer->wpos = 0;

	return buffer;
}

unsigned int rbuffer_data_len(void* ptr){
	rbuffer_t* buffer = ptr;
	if(!buffer)return 0;

	if(buffer->rpos == buffer->wpos){
		return 0;
	}else if(buffer->wpos > buffer->rpos){
		return buffer->wpos - buffer->rpos;
	}else{
		return buffer->size - buffer->rpos + buffer->wpos;
	}
}

unsigned int rbuffer_free_len(void* ptr){
	rbuffer_t* buffer = ptr;
	if(!buffer)return 0;

	for(unsigned int i=1;i<REMAIN;i++){
		if(buffer->wpos + i == buffer->rpos){
			printf("%s error,wpos=%d rpos=%d",__FUNCTION__,buffer->rpos,buffer->wpos);
			buffer->wpos = 0;
			buffer->rpos = 0;
			return 0;
		}
	}

	if(buffer->wpos + REMAIN == buffer->rpos){
		return 0;
	}else if(buffer->rpos > buffer->wpos){
		return buffer->rpos - buffer->wpos - REMAIN;
	}else{
		int len = buffer->size - buffer->wpos + buffer->rpos;

		if(len <= REMAIN)return 0;
		else return len - REMAIN;
	}
}

unsigned int rbuffer_read(void* ptr,void* buf,unsigned int size){
	rbuffer_t* buffer = ptr;
	if(!buffer)return 0;

	unsigned int len = rbuffer_data_len(buffer);
	if(len == 0)return 0;

	if(len > size)len = size;

	if(buffer->wpos > buffer->rpos){
		memcpy(buf,&buffer->data[buffer->rpos],len);
		buffer->rpos += len;
		if(buffer->rpos >= buffer->size)buffer->rpos = 0;
	}else{    //rpos > wpos
		unsigned int l = buffer->size - buffer->rpos;
		if(l >= len){
			memcpy(buf,&buffer->data[buffer->rpos],len);
			buffer->rpos += len;
			if(buffer->rpos >= buffer->size)buffer->rpos = 0;
		}else{
			memcpy(buf,&buffer->data[buffer->rpos],l);
			memcpy(buf + l,&buffer->data[0],len - l);
			buffer->rpos = len - l;
		}
	}

	return len;
}

unsigned int rbuffer_write(void* ptr,void* data,unsigned int size){
	rbuffer_t* buffer = ptr;
	if(!buffer)return 0;

	unsigned int len = rbuffer_free_len(buffer);
	if(len == 0)return 0;

	if(len > size)len = size;

	if(buffer->wpos < buffer->rpos){
		memcpy(&buffer->data[buffer->wpos],data,len);
		buffer->wpos += len;
		if(buffer->wpos >= buffer->size)buffer->wpos = 0;
	}else{   //wpos >= rpos
		int l = buffer->size - buffer->wpos;
		if(l >= len){
			memcpy(&buffer->data[buffer->wpos],data,len);
			buffer->wpos += len;
			if(buffer->wpos >= buffer->size)buffer->wpos = 0;
		}else{
			memcpy(&buffer->data[buffer->wpos],data,l);
			memcpy(&buffer->data[0],data + l,len - l);
			buffer->wpos = len - l;
			if(buffer->wpos >= buffer->size)buffer->wpos = 0;
		}
	}

	return len;
}

int rbuffer_send(void* ptr,void* data,unsigned int size){
	if(!data)return 0;
	if(size == 0)return 0;

	if(rbuffer_free_len(ptr) < size + sizeof(unsigned int))return 0;
	rbuffer_write(ptr,&size,sizeof(unsigned int));
	rbuffer_write(ptr,data,size);
	return size;
}

int rbuffer_receive(void* ptr,void* buffer,unsigned int size){
	if(!buffer)return 0;
	if(size == 0)return 0;
	if(rbuffer_data_len(ptr) < sizeof(unsigned int))return 0;

	unsigned int len = 0;
	rbuffer_read(ptr,&len,sizeof(unsigned int));
	if(len == 0)return 0;

	unsigned int l = rbuffer_read(ptr,buffer,len);
	if(l != len){
		printf("%s read error,l=%d len=%d",__FUNCTION__,l,len);
	}
	return l;
}

