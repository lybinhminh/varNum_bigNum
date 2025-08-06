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
#define KARATSUBA_THRESHOLD 1025
class varInt {
public:
	byte* data = nullptr;
	size_t length = 0;
	bool _do_left_shift_expand_ = true;
	bool _auto_assign_shift_arithmetic_ = false;
	bool _barret_cache_ = false;
	varInt* barret_cache = NULL;
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
	bool operator> (const varInt&) const;
	bool operator< (const varInt&) const;
	bool operator== (const varInt&) const;
	bool operator!= (const varInt&) const;
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
	static varInt pow(varInt, size_t);
	static varInt binaryPow(varInt, size_t);
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
	varInt Toom3Mult(varInt);
	varInt operator<<= (size_t);
	varInt operator>>= (size_t);
	varInt barretFastDivision(varInt,varInt*);

};
static const varInt zero(0), one(1), two(2), ten(10), base(256);
#endif
