#include <cstdint>
#ifndef BINHMINH_UTILS_H
#define BINHMINH_UTILS_H
uint32_t ROTR(uint32_t, int);
uint32_t ROTL(uint32_t, int);
unsigned char* itob(int32_t); // int32_t-to-bytes
unsigned char* flip(unsigned char*&, int);
char* flip(char*&, int);

#endif
