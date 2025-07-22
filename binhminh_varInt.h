/*
Ly Binh Minh's bigNum or variable-length library
PreRelease1.0
 Caution: written by kid, manually memory management
*/


#include <cstdint>
#include <map>
#ifndef BINHMINH_VARINT_H
#define BINHMINH_VARINT_H
#define byte unsigned char
#define NAIVE_MULT_THRESHOLD 257
class varInt {
public:
	byte* data = nullptr;
	size_t length = 0;
	bool _do_left_shift_expand_ = true;
	bool _auto_assign_shift_arithmetic_ = false;
	varInt(int32_t);
	varInt(int64_t);
	varInt(byte*, size_t, size_t);
	varInt();
	void discard();
	varInt operator+ (varInt);
	varInt operator- (varInt);
	varInt operator* (varInt);
	varInt operator/ (varInt);
	varInt operator% (varInt);
	varInt normalize();
	static bool isNegative(varInt);
	varInt naiveMult(varInt);
	varInt naiveDivi(varInt, varInt*);
	void printHex();
	varInt operator<< (size_t);
	varInt operator>> (size_t);
	varInt operator+= (varInt);
	varInt operator -= (varInt);
	bool operator> (varInt);
	bool operator< (varInt);
	bool operator== (varInt);
	bool operator!= (varInt);
	static varInt abs(varInt);
	varInt operator~ ();
	static int getBit(varInt, size_t, size_t);
	static void setBit(varInt, size_t, size_t, int);
	static varInt insert(varInt, size_t, byte);
	static varInt negate(varInt);
	void testf(varInt);
	varInt karatsubaMult(varInt);
	static int countLeadingBit(varInt, int);
	static varInt pow(varInt, varInt);
	static varInt binaryPow(varInt, varInt);
	varInt operator*=(varInt);
	varInt operator/= (varInt);
	varInt naiveMod(varInt);
	static varInt barretReduction(varInt x, varInt m);
	static varInt fromString(const char*);
	static char* toString(varInt);
	int64_t toInt64();
	void printString();
	static varInt gcd(varInt a, varInt b);
	static void swap(varInt, varInt);
	static varInt bezoutIdentity(varInt a, varInt b, varInt* x, varInt* y);
	static varInt modInverse(varInt, varInt);
};
#endif
