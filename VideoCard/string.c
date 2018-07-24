// Implementation of string and memory functions

#include <string.h>

int strcmp(const char* str1, const char* str2) {
	if (str1 > str2) {
		return 1;
	}else if (str1 < str2) {
		return -1;
	}else {
		return 0;
	}
}
char * strcpy(char * destination, const char * source) {
	while(*destination++ = *source++)
		;
	
	return destination;
}
errno_t strcpy_s(char *destination, size_t numberOfElements, const char *source){
	if(destination == NULL){
		return 22;
	}
	for(int i = 0; i < numberOfElements; i++){
		*destination = *source;
	}
	return 0;
}
size_t strlen(const char* source) {
	char * i;
	size_t leng = 0;
	for(i=source; *i; i++){
		leng++;
	}
	return leng;
}

void * memcpy(void * destination, const void * source, size_t count){
	char* d = (char*)destination;
	char* s = (char*)source;
	for(int i = 0; i < count; i++){
		d[i] = s[i];
	}
}
errno_t memcpy_s(void *destination, size_t destinationSize, const void *source, size_t count){
	if(destinationSize < count || destination == NULL){
		return 22;
	}
	char* d = (char*)destination;
	char* s = (char*)source;
	for(int i = 0; i < count; i++){
		d[i] = s[i];
	}
	return 0;
}
void * memset(void *destination, char val, size_t count){
	char* d = (char*)destination;
	for(int i = 0; i < count; i++){
		d[i] = val;
	}
}
unsigned short * memsetw(unsigned short * destination, unsigned short val, size_t count){
	for(int i = 0; i < count; i++){
		destination[i] = val;
	}
	return destination;
}
