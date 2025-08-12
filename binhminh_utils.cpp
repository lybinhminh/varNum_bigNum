#include <cstdlib>
#include <cstring>
#include "binhminh_utils.h"
uint32_t ROTR(uint32_t origin, int by) {
	return origin << (32 - by) | origin >> by;
}
uint32_t ROTL(uint32_t origin, int by) {
	return origin >> (32 - by) | origin << by;
}
unsigned char* itob(int32_t origin) {
	// local to local
	unsigned char *result = (unsigned char*)malloc(4);
	for (int i = 0; i < 4; ++i) {
		result[i] = (unsigned char)(origin >> (i * 8));
	}
	return result;
}
unsigned char* flip(unsigned char*& byteArray, int length) {
	unsigned char* result = (unsigned char*)malloc(length);
	for (int i = 0; i < length; ++i) {
		result[i] = byteArray[length - i - 1];
	}
	free(byteArray);
	byteArray = (unsigned char*)malloc(length);
	memcpy(byteArray, result, length);
	free(result);
	return byteArray;
}
char* flip(char*& byteArray, int length) {
	char* result = (char*)malloc(length);
	for (int i = 0; i < length; ++i) {
		result[i] = byteArray[length - i - 1];
	}
	free(byteArray);
	byteArray = (char*)malloc(length);
	memcpy(byteArray, result, length);
	free(result);
	return byteArray;
}
