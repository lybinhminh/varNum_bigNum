#include <cstdint>
#ifndef BINHMINH_HASHES_H
#define BINHMINH_HASHES_H
class sha256 {
private:
	static uint32_t smallSigma0(uint32_t);
	static uint32_t smallSigma1(uint32_t);
	static uint32_t bigSigma0(uint32_t);
	static uint32_t bigSigma1(uint32_t);
	static uint32_t choose(uint32_t, uint32_t,uint32_t);
	static uint32_t major(uint32_t, uint32_t, uint32_t);
	static unsigned char* hton_int(int32_t, size_t);
	static int32_t ntoh_int(unsigned char*, size_t);
public:
	static unsigned char* run(unsigned char*, int);
	static unsigned char* hash(unsigned char*, int);
	static unsigned char* over(unsigned char*, int);
	static unsigned char* text(unsigned char*, int);
	static unsigned char* of(unsigned char*, int);
};
#endif