
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "binhminh_varInt.h"
#include <vector>

varInt::varInt(int64_t origin) {
	data = (byte*)malloc(8);
	length = 8;
	for (int i = 0; i < 8; ++i) {
		data[i] = origin >> (i * 8);
	}
}
// void discard(void)
// free data array and set 'length' to 0
// be cautious as using free()
// common pitfall: double discard call (due to the use of "unsafe" method)
// must call after constructing a varInt
// Exception: heapCorruption
// Security level: highly-risky
// Usefulness level: mandatory (as varInts need manual memory management)

void varInt::discard() {
	free(data);
	length = 0;
	
}
// varInt insert(varInt dest, size_t nthByte, size_t value)
// insert/ add a byte of value 'value' to the specific index in the data array
// of varInt 'dest', shift the current byte in that spot to the left
// range 0 (append first) - dest.length (append back)
// return: new varInt which has been modified as your wish
// Exception: NullPointerException, OutBoundsForLength
// Security level: unsafe
// Usefulness level: dependent, mid
varInt varInt::insert(varInt a, size_t nthByte, byte b) {
	varInt c;
	c.length = a.length + 1;
	c.data = (byte*)malloc(c.length);
	int j = 0;
	for (int i = 0; i < c.length; ++i) {
		if (i == nthByte) {
			c.data[i] = b;
		}
		else {
			c.data[i] = a.data[j++];
		}
	}
	
	a.length = c.length;
	a.data = (byte*)malloc(a.length);
	memcpy(a.data, c.data, c.length);
	
	return a;
}
// void printHex(void)
// print hex sequence of the varInt
// Exception: no
// Security level: careless
// Usefulness level: useful
void varInt::printHex() {
	printf("0x");
	for (int i = length - 1; i > -1; --i) {
		printf("%0.2x", data[i]);
	}
	printf("\n");
}
// bool isNegative(varInt a)
// check if varInt is negative according to 2's complement rules
// Exception: NullPointerException
// Security level: unsafe
// Usefulness level: recommended
bool varInt::isNegative(varInt a) {
	return (a.data[a.length - 1] & 0x80) != 0;
}
// constructor varInt(int32_t v)
// creates a new varInt and copy all 4 bytes from int32_t v to it
// Exception: no
// Security level: safe
// Usefulness level: recommended
varInt::varInt(int32_t origin) {
	data = (byte*)malloc(4);
	length = 4;
	for (int i = 0; i < length; ++i) {
		data[i] = origin >> (i * 8);
	}
	
	//printHex();
}
// constructor varInt(byte* data, size_t srcLen, size_t destLen)
// create a new varInt of size destLen and deep copy 'srcLen' bytes from data
// Exception: NullPointerException, OutBoundsForLength
// Security level: unsafe
// Usefulness level: promoted (highly recommended)
varInt::varInt(byte* data, size_t dataLen, size_t length) {
	this->data = (byte*)malloc(length);
	memcpy(this->data, data, dataLen);
	bool negativity = (data[dataLen - 1] & 128) == 0x80;
	for (int i = dataLen; i < length; ++i) {
		this->data[i] = negativity ? 0xFF : 0x00;
	}
	this->length = length;

	//printHex();
}
// varInt normalize(void)
// normalize the current varInt and return a copy of it
// Security level: highly-risky, dangerous
// NOTE: implementor can accidently use normalize over parameterized varInt
// which absolutely normalizes THE INSTANCE of it in THE METHOD/ FUNCTION, the
// very original varInt gets discarded
// So please, if you are not intended to discard the root varInt, create a deep copy of it
// in the function using constructor varInt(byte*,size_t,size_t)
// or normalize the varInt before passing to the method/function
// Usefulness level: dependent, useful
varInt varInt::normalize() {
	bool negativity = isNegative(*this);
	size_t actualDataSize = length;
	for (int i = length - 1; i > 0; --i) {
		byte cB = data[i], nB = data[i - 1];
		if (negativity == false) {
			if (cB == 0 && (nB & 0x80) == 0) {
				actualDataSize--;
			}
			else break;
		}
		else {
			
			if (cB == 0xFF && (nB & 0x80) != 0) {
				actualDataSize--;
			}
			else
				break;
		}
	}
	if (length > actualDataSize)
	data = (byte*)realloc(data, actualDataSize);
	length = actualDataSize;
	return *this;
}
// varInt operator+(varInt b)
// perform addition of 2 varInt
// return: the sum
// Exception: NullPointerException, OverflowException
// Security level: mildly-unsafe
// Usefulness level: useful
varInt varInt::operator+ (varInt b){

	size_t sharedSize = std::max(length, b.length);
	
	varInt c(data, length, sharedSize), d(b.data,b.length,sharedSize);
	bool a_s_Negativity = varInt::isNegative(*this),
		b_s_Negativity = varInt::isNegative(b),
		sign_diff_mode = a_s_Negativity != b_s_Negativity;
	byte carry = 0;
	for (int i = 0; i < sharedSize; ++i) {
		uint16_t sum =(unsigned char) c.data[i] + (unsigned char)d.data[i] + carry;
		
		c.data[i] = (byte)sum;
		if (sum > 255)
			carry = 1;
		else
			carry = 0;
	}
	d.discard();
	if (!sign_diff_mode && carry != 0) {
		// if both signs arenot opposed and if padding_bytes are necessary
		varInt sum(c.data,c.length,c.length+1);
		if (a_s_Negativity) {
			// both neg
			sum.data[c.length] = 0xFF;
		}
		else {
			// both pos
			
			sum.data[c.length] = 0x01;
		}
		c.discard();
		return sum;
	}
	else
	{
		return c;
	}
}
// varInt operator+= (varInt b)
// performs addition and assign the sum to the current varInt
// return: the sum
// Exception: NullPointerException, OverflowException
// Security level: highly-risky, dangerous
// NOTE: perform this on a copy of a varInt leads to the original varInt to be
// discared, so consider creating a deep copy of the original and
// use this operator on it instead if you do not want
// the original to get discarded without your permit
// Usefulness: dependent, mid
varInt varInt::operator+= (varInt b) {
	
	varInt c = *this + b;
	discard();
	length = c.length;
	data = (byte*)malloc(length);
	memcpy(data, c.data, c.length);
	c.discard();
	return *this;
}
// constructor varInt(void)
// create a varInt with no intial value
// you must allocate memory for 'data' and assign bytes to it
// and remember to set the 'length'
// Security level: safe
// Usefulness level: useful
varInt::varInt() {}
// varInt operator-(varInt b)
// perform subtraction with the current varInt as minuend and
// 'b' as subtrahend
// return: the difference
// Exception: NullPointerException, OverflowException
// Security level: mildly-unsafe
// Usefulness level: useful
varInt varInt::operator- (varInt b) {
	
	int sharedSize = std::max(length, b.length);
	varInt c(data, length, sharedSize), d(b.data, b.length, sharedSize);
	
	bool a_s_Negativity = varInt::isNegative(*this), b_s_Negativity = varInt::isNegative(b),
		sign_diff_mode = a_s_Negativity != b_s_Negativity;
	byte borrow = 0;
	for (int i = 0; i < sharedSize; ++i) {
		int16_t diff = c.data[i] - d.data[i] - borrow;
		
		if (diff < 0) {
			borrow = 1;
		}
		else {
			borrow = 0;
		}
		c.data[i] = diff;
	}
	d.discard();
	if (!sign_diff_mode)
	{
		// if both minuend and subtrahend have the same sign
		// given a, b as abs of minuend and subtrahend respectively
		// in this case we have a - b or -a - (-b) = -(a - b), both reduces the
		// length of the final difference
		return c;
	}
	else {
		varInt diff(c.data, c.length, c.length + 1);
		if (a_s_Negativity) {
			// a is neg : -a - b = -(a + b) -> append 0xFF
			diff.data[c.length] = 0xFF;
		}
		else {
			// a is pos : a - b = a + + (-b) = a + c where c = abs(b) -> borrow -> carry and preserve
			diff.data[c.length] = 0x00;
		}
		c.discard();
		return diff;
	}
}
// varInt operator-= (varInt b)
// performs subtraction and assign the sum to the current varInt
// return: the difference
// Exception: NullPointerException, OverflowException
// Security level: highly-risky, dangerous
// NOTE: perform this on a copy of a varInt leads to the original varInt to be
// discared, so consider creating a deep copy of the original and
// use this operator on it instead if you do not want
// the original to get discarded without your permit
// Usefulness: dependent, mid
varInt varInt::operator-= (varInt b) {
	varInt c = *this - b;
	c.normalize();
	discard();
	length = c.length;
	data = (byte*)malloc(length);
	memcpy(data, c.data, length);
	c.discard();
	return *this;
}
// varInt naiveMult(varInt b)
// perform long multiplication with the the LONGER varInt as multiplicand,
// the other as multiplier
// return: the product
// Exception: NullPointerException,	OverflowException
// Security level: mildly unsafe
// Usefulness level: highly-useful
varInt varInt::naiveMult(varInt b) {
	varInt multiplicand(data,length,length), multiplier (b.data,b.length,b.length), result(0);
	multiplicand.normalize();
	multiplier.normalize();
	bool multiplicand_negativity = isNegative(multiplicand),multiplier_negativity = isNegative(multiplier),
		negativity = multiplicand_negativity ^ multiplier_negativity;
	if (multiplicand_negativity) {
		varInt c = abs(multiplicand);
		multiplicand.discard();
		multiplicand = c;
	}
	if (multiplier_negativity) {
		varInt c = abs(multiplier);
		multiplier.discard();
		multiplier = c;
	}
	std::vector<varInt> partials;
	for (int i = 0; i < multiplier.length; ++i) {
		byte multiplier_byte = multiplier.data[i];
		for (int bit = 0; bit < 8; ++bit) {
			int bitMask = 1 << bit;
			if ((multiplier_byte & bitMask) != 0) {
			 varInt c = multiplicand << (bit + (i * 8));
			 partials.push_back(c);
			}
		}
	}

	while( partials.size() > 0) {
		varInt c = partials.back();
		partials.pop_back();
		result += c;
		c.discard();
	}
	multiplier.discard();
	result.normalize();
	
	if(!negativity)
	{
		return result;
	}
	else {
		varInt c = ~result, d(1), e = c + d;
		c.discard();
		d.discard();
		result.discard();
		return e;
	}
}
// varInt operator<< (size_t shift)
// performs binary left shift on the current varInt
// return: left shifted varInt
// Flags within varInt class:
// _do_left_shift_expand_ : if true, this operator will expand (reallocate larger) the
// result varInt to hold bits on the leftmost, else keep the length and
// the bits on the leftmost will get erased
// _auto_assign_shift_arithmetic_ : if true, << is now equivalent to <<= (shift and assign)
// and you must not do this : varInt a; ... ; a = a << n
// Exception: NullPointerException
// Security level: unsafe (if _auto_assign_shift_arithmetic_ = false), 
// highly-risky, dangerous (if _auto_assign_shift_arithmetic_ = true)
// NOTE: if _auto_assign_shift_arithmetic_ is set to true, DO NOT use this operator
// on a copy of another varInt or the original one will get discarded and produce
// undefined behavior if being mistook as a valid one later on. It is
// recommended to perform this on a deep copy.
varInt varInt::operator<< (size_t by) {
	if (by == 0)return *this;

	varInt result;
	int _intended_rLength = length + ((by % 8 == 0) ? (by / 8) : (1 + by / 8));
	if (!_do_left_shift_expand_)_intended_rLength = length;
	result.length = _intended_rLength;
	result.data = (byte*)malloc(result.length);
	bool negativity = isNegative(*this);
	size_t paddedBitsLeft = by;
	int j = 8, x = 0;
	for (int i = 0; i < result.length; ++i) {
		if (paddedBitsLeft> 7) {
			result.data[i] = 0;
			paddedBitsLeft -= 8;
		}
		else if (((byte)(by << 5)) == 0) {
			// if paddedBitsLeft / 8 = 0 (mod 8)
			result.data[i] = data[x++];
		}
		else {
			byte b = 0;
			if (paddedBitsLeft > 0){
			j = paddedBitsLeft;
			b |= data[x] << j;
			
			result.data[i] = b;
			paddedBitsLeft = 0;
			}
			else {
				b |= data[x] >> (8 - j);
				if (++x < length)
				{
					b |= data[x] << j;
				}
				
				result.data[i] = b;
			}
		}
	}
	//result.normalize();
	if (negativity && !isNegative(result)) {
		// in this case there will be atlest 1 zero at the top of result
		for (int i = 7; i > 0; --i) {
			int bit = getBit(result, result.length - 1, i);
			if (bit != 0)break;
			setBit(result, result.length - 1, i, 1);
		}
	}else if(!negativity && isNegative(result)){
		varInt temp(result.data, result.length, result.length + 1);
		temp.data[result.length] = 0x00;
		result.discard();
		result = temp;
	}
	if (_auto_assign_shift_arithmetic_) {
		
		discard();
		length = result.length;
		data = (byte*)malloc(length);
		
		memcpy(data, result.data, length);
		result.discard();
		return *this;
	}
	return result;
}
// varInt operator>> (size_t shift)
// performs binary right shift on the current varInt
// and keep sign by append appropriate padding bits
// return: right-shifted varInt
// Flags within varInt class:
// _auto_assign_shift_arithmetic_ : if true, >> is now equivalent to >>= (shift and assign)
// and you must not do this : varInt a; ... ; a = a >> n
// Exception: NullPointerException
// Security level: unsafe (if _auto_assign_shift_arithmetic_ = false), 
// highly-risky, dangerous (if _auto_assign_shift_arithmetic_ = true)
// NOTE: if _auto_assign_shift_arithmetic_ is set to true, DO NOT use this operator
// on a copy of another varInt or the original one will get discarded and produce
// undefined behavior if being mistook as a valid one later on. It is
// recommended to perform this on a deep copy.
varInt varInt::operator>> (size_t by) {
	if (by == 0)return *this;
	
	varInt result;
	result.length = length ;
	result.data = (byte*)malloc(result.length);
	int  k = 0, x = length - 1;
	size_t paddedBitsLeft = by;
	bool negativity = isNegative(*this);
	
	for (int i = result.length - 1; i > -1; --i) {
		
		if (paddedBitsLeft > 7) {
			result.data[i] = negativity ? 0xFF : 0x00;
			paddedBitsLeft -= 8;
		}
		else if (((byte)(by << 5)) == 0) {
			result.data[i] = data[x--];
		}
		else {
			byte b = 0;
			if (paddedBitsLeft > 0) {
				k = paddedBitsLeft;
				byte padding = (negativity ? 0xFF : 0x00) << (8 - k);
				b |= padding;
			
				b|= data[x] >> k;
				result.data[i] = b;
				paddedBitsLeft = 0;
			}
			else {
				b |= data[x] << (8 - k);
				
				if (--x > -1)
					b |= data[x] >> k;
				
				result.data[i] = b;
			}
		}
	}
	result.normalize();
	if (_auto_assign_shift_arithmetic_) {
		
		discard();
		length = result.length;
		data = (byte*)malloc(length);
		memcpy(data, result.data, length);
		result.discard();
		return *this;
	}
	return result;

}
// bool operator> (varInt b)
// compares if the current varInt is bigger than varInt 'b'
// return: *this > b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator> (varInt b) {
	bool a_s_Negativity = isNegative(*this),
		b_s_Negativity = isNegative(b);
	if (a_s_Negativity && !b_s_Negativity)return false;
	else if (!a_s_Negativity && b_s_Negativity)return true;
	else {
		if (length != b.length) {
			if (a_s_Negativity) return length < b.length;
			else return length > b.length;
		}
		else {
			for (int i = length - 1; i > -1; --i) {
				int c = data[i], d = b.data[i];
				if (c > d)return true;
				else if(c < d)return false;
			}
			return false;
		}
	}
}
// bool operator> (varInt b)
// compares if the current varInt is equal to varInt 'b'
// return: *this == b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator== (varInt b) {
	
	if (length != b.length)return false;
	for (int i = length - 1; i > -1; --i) {
		byte c = data[i], d = b.data[i];
		
		if (c != d)return false;
	}
	return true;
}
// bool operator< (varInt b)
// compares if the current varInt is smaller than varInt 'b'
// return: *this < b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator< (varInt b) {
	bool a_s_Negativity = isNegative(*this),
		b_s_Negativity = isNegative(b);
	if (a_s_Negativity && !b_s_Negativity)return true;
	else if (!a_s_Negativity && b_s_Negativity)return false;
	else {
		if (length != b.length) {
			if (a_s_Negativity) return length > b.length;
			else return length < b.length;
		}
		else {
			for (int i = length - 1; i > -1; --i) {
				int c = data[i], d = b.data[i];
				if (c > d)return false;
				else if (c < d)return true;
			}
			return false;
		}
	}
}
// bool operator!= (varInt b)
// compares if the current varInt is not equal to varInt 'b'
// return: *this != b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator!= (varInt b) {
	return !(*this == b);
}
// varInt operator~ (void)
// return a copy of the current varInt with all bits reversed
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: useful
varInt varInt::operator~() {
	varInt result;
	result.length = length;
	result.data = (byte*)malloc(result.length);
	for (int i = 0; i < length; ++i) {
		result.data[i] = ~data[i];
	}
	return result.normalize();
}
// varInt abs(varInt a)
// return absolute value of varInt 'a'
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::abs(varInt a) {
	if (!isNegative(a)) {
		return varInt(a.data,a.length,a.length);
	}
	else {
		varInt b = ~a, c = varInt(1), d = b + c;
		b.discard();
		c.discard();
		return d;
	}
}
// int getBit(varInt src, size_t nthByte, size_t nthBit)
// get the bit at index 'nthBit' in the 'nthByte'-th byte of varInt 'src'
// return 0 , 1
// Exception: NullPointerException, OutBoundsForLength
// Security level: mildly unsafe
// Usefulness level: dependent
int varInt::getBit(varInt a, size_t nthByte, size_t nthBit) {
	return (a.data[nthByte] >> nthBit) & 0x01;
	// return bit 0 or 1
}
// void setBit(varInt dest, size_t nthByte, size_t nthBit)
// set the bit at index 'nthBit' in 'nthByte'-th byte of varInt 'dest'
// Exception: NullPointerException , OutBoundsForLength, InvalidArguments
// Security level: unsafe
// Usefulness level: dependent, useful
void varInt::setBit(varInt a, size_t nthByte, size_t nthBit, int bit) {
	byte adder = bit << nthBit;
	byte cleaner = ~(1 << nthBit);
	byte intermediate = a.data[nthByte] & cleaner;
	a.data[nthByte] = intermediate | adder;
}
// varInt naiveDivi(varInt b, varInt& [out, nullable] remainder)
// perform bit-by-bit divison with the current varInt as dividend and
// varInt 'b' as divisor, return the remainder as demand
// return: the quotient
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: promoted
varInt varInt::naiveDivi(varInt b, varInt* remainderOut) {
	varInt C(data, length, length), D(b.data, b.length, b.length);
	bool negativity = isNegative(C) ^ isNegative(D), remainder_negativity = isNegative(C);
	varInt dividend = abs(C), divisor = abs(D), remainder(0), quotient(0);
	dividend.normalize(); divisor.normalize();
	remainder.normalize(); quotient.normalize();
	if (divisor == remainder) {
		// divide by 0;
		C.discard();
		D.discard();
		dividend.discard(); divisor.discard(); remainder.discard(); quotient.discard();
		printf("Divide by 0 is invalid\n");
		return NULL;
	}
	int currentByte = dividend.length - 1;
	int bitInRemainder = 0;
	int bitInQuotient = 0;

	quotient._do_left_shift_expand_ = false;
	quotient._auto_assign_shift_arithmetic_ = true;
	remainder._auto_assign_shift_arithmetic_ = true;
	for (int i = 1; i < divisor.length; ++i) {
		remainder << 8;
		remainder.data[0] = dividend.data[dividend.length - i];
	}
	remainder._do_left_shift_expand_ = false;
	remainder.normalize();
	for (; currentByte > -1; currentByte--) {
		
		for (int i = 7; i > -1; --i) {

		int bit = getBit(dividend, currentByte, i);
		
		if (bitInQuotient % 8 == 0) {
			remainder._do_left_shift_expand_ = true;
			remainder << 1;
			remainder._do_left_shift_expand_ = false;
		}
		else
			remainder << 1;
		setBit(remainder, 0, 0, bit);
		
		bitInRemainder++;
		remainder.normalize();
		
		if(remainder < divisor)
		{
			
			if (bitInQuotient % 8 == 0) {
				quotient._do_left_shift_expand_ = true;
				quotient << 1;
				quotient._do_left_shift_expand_ = false;
			}
			else
				quotient << 1;
				
			setBit(quotient, 0, 0, 0);
			
			bitInQuotient++;
			
			
		}
		else {
			while (!(remainder < divisor)) {
			
				remainder -= divisor;
				if (bitInQuotient % 8 == 0) {
					quotient._do_left_shift_expand_ = true;
					quotient << 1;
					quotient._do_left_shift_expand_ = false;
				}
				else
					quotient << 1;
				setBit(quotient, 0, 0, 1);
				
				bitInQuotient++;
				
			}
			remainder.normalize();
		}
		}
	}
	remainder.normalize();
	
	if (negativity) {
		varInt E = negate(quotient);
		quotient.discard();
		quotient = E;
	}
	if (remainder_negativity) {
		varInt F = negate(remainder);
		remainder.discard();
		remainder = F;
	}
	quotient.normalize();
	if (remainderOut != NULL) {
		remainderOut->data = remainder.data;
		remainderOut->length = remainder.length;
	}
	else
		remainder.discard();
	C.discard(); 
	D.discard();
	dividend.discard(); divisor.discard();
	return quotient;
}
// varInt negate(varInt a)
// reverse sign of varInt a
// return a reverse version of 'a'
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::negate(varInt a) {
	varInt flipped = ~a;
	varInt one(1);
	varInt result = flipped + one;
	flipped.discard();
	one.discard();
	return result;
}
// varInt karatsubaMult(varInt b)
// perform multiplication of a and b using karatsuba's fast multiplication ( based
// on recursive calls)
// Exception: NullPointerException, OverflowException
// Security level: mildly-unsafe
// Usefulness level: recommended for multiplication involving >256-byte varInt
varInt varInt::karatsubaMult(varInt b) {
	if (length < NAIVE_MULT_THRESHOLD || b.length < NAIVE_MULT_THRESHOLD) {
		return (*this).naiveMult(b);
	}
	int sharedSize = std::max(length, b.length);
	varInt A(data, length, sharedSize), B(b.data, b.length, sharedSize);
	bool negativity = isNegative(A) ^ isNegative(B);
	{
		varInt c = abs(A);
		A.discard();
		A = c;
	}
	{
		varInt c = abs(B);
		B.discard();
		B = c;
	}
	int m = sharedSize / 2;

	varInt a_low(A.data, m, m), a_high(A.data + m, A.length - m, A.length - m), 
		b_low(B.data,m,m), b_high(B.data + m, B.length - m,B.length - m);
	varInt AA = a_low + a_high, BB = b_low + b_high;
	varInt Z0 = a_low.karatsubaMult(b_low), Z2 = a_high.karatsubaMult(b_high), ZZ = Z0 + Z2,Z1 =
		AA.karatsubaMult(BB) - ZZ;
	varInt c = Z2 << (2 * m * 8), d = Z1 << (m * 8), e = c + d, f = e + Z0;
	A.discard();
	B.discard();
	a_low.discard();
	a_high.discard();
	b_low.discard();
	b_high.discard();
	AA.discard();
	BB.discard();
	ZZ.discard();
	Z0.discard();
	Z2.discard();
	Z1.discard();
	c.discard();
	d.discard();
	e.discard();
	if (!negativity)
		return f;
	else {
		varInt g = negate(f);
		f.discard();
		return g;
	}
}
// void testf(varInt a)
// a blank function for arbitrary testing
void varInt::testf(varInt a) {
	a.normalize();
}
// varInt operator* (varInt b)
// perform hybrid multiplication (naive combined with karatsuba)
// return : the product
// Exception: NullPointerException , OverflowException
// Security level : mildly unsafe
// Usefulness level: promoted
varInt varInt::operator* (varInt b) {
	varInt A(data, length, length), B(b.data, b.length, b.length);
	A.normalize();
	B.normalize();
	varInt product;
	if (A.length < NAIVE_MULT_THRESHOLD && B.length < NAIVE_MULT_THRESHOLD) {
		product = A.naiveMult(B);
	}
	else {
		product = A.karatsubaMult(B);
	}
	A.discard();
	B.discard();
	return product;
}
// int countLeadingBit(varInt v, int Bit)
// counts the number of bits of value 'Bit' (0 or 1) from
// the leftmost point of the varInt
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: dependent
int varInt::countLeadingBit(varInt v, int Bit) {
	int count = 0;
	for (int i = v.length - 1, cont = 1; (i > -1) && (cont == 1); --i) {
		for (int j = 7; j > -1; --j) {
			int bit = getBit(v, i, j);
			if (bit != Bit) {
				cont = 0;
				break;
			}
			count++;
		}
	}
	return count;
}
// varInt operator*= (varInt b)
// calculate *this * b and assign the product to b
// return *this
// Exception: NullPointerException
// Security level: highly risky, dangerous
// NOTE: this method should be done on deep copy or the very orginal itself
// or it ould cause heap corruption, should not be applied on function-
// parameterized varInts
// Usefulness level: dependent
varInt varInt::operator*= (varInt b) {
	varInt result = *this * b;
	discard();
	length = result.length;
	data = (byte*)malloc(length);
	memcpy(data, result.data, length);
	result.discard();
	return *this;
}
// varInt po (varInt a, varInt b)
// naive method of calculating a^b
// due to multiplication optimization and varInt's nature
// this method is not much different from binaryPo in term
// of speed
// return: the po er
// Exception: NullPointerException, Overflo Exception
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::pow(varInt a, varInt b) {
	varInt result(a.data,a.length,a.length), one(1),a_copy(a.data,a.length,a.length),
		b_copy(b.data, b.length, b.length);
	result.normalize();
	one.normalize();
	b_copy.normalize();
	a_copy.normalize();
	while (b_copy > one) {
		b_copy -= one;
		b_copy.normalize();
		result *= a;
	}
	one.discard();
	a_copy.discard();
	b_copy.discard();
	return result;
}
varInt varInt::operator/=  (varInt b) {
	varInt result = (*this).naiveDivi(b,NULL);
	discard();
	length = result.length;
	data = (byte*)malloc(length);
	memcpy(data, result.data, length);
	result.discard();
	return *this;
}
// varInt naiveMod (varInt b)
// naive method of calculating *this mod b
// due to excellent division efficiency, this method is kinda fast
// return the remainder
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::naiveMod(varInt b) {
	varInt a(data, length, length), c(b.data, b.length, b.length);
	a.normalize();
	c.normalize();
	varInt remainder(0), quotient = a.naiveDivi(c, &remainder);
	quotient.discard();
	a.discard();
	c.discard();
	return remainder;
}
// varInt binaryPow(varInt a, varInt b)
// using binary exponentation to calculate a^b quickly
// return the power
// Exception: NullPointerException, OverflowException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::binaryPow(varInt a, varInt b) {
	varInt result(1), zero(0), one(1),two(2),  a_copy(a.data, a.length, a.length),
		b_copy(b.data, b.length, b.length);
	result.normalize();
	zero.normalize();
	one.normalize();
	two.normalize();
	b_copy.normalize();
	a_copy.normalize();
	b_copy._auto_assign_shift_arithmetic_ = true;
	while (b_copy > zero) {
		if ((b_copy.data[0] & 0x01) == 1) {
			result *= a_copy;
		}
		
		a_copy *= a_copy;
		b_copy >> 1;

	}
	zero.discard();
	one.discard();
	two.discard();
	a_copy.discard();
	b_copy.discard();
	return result;
}
// varInt barretReduction (varInt x, varInt m)
// calculate x mod m using barret technique
// unusable for x.length and m.length < 2
// return: the remainder
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: useful
varInt varInt::barretReduction(varInt x, varInt m) {
	varInt X(x.data, x.length, x.length), M(m.data, m.length, m.length);
	X.normalize();
	M.normalize();
	if (X < M) {
		return X;
	}
	
	varInt base(256), _2k((int32_t)M.length * 2), bp2k = binaryPow(base.normalize(), _2k.normalize()),
		micro = bp2k.naiveDivi(M,NULL), kmo((int64_t)M.length - 1), kpo((int64_t)M.length + 1);
	varInt bpkmo = binaryPow(base, kmo.normalize()).normalize(), bpkpo = binaryPow(base, kpo.normalize()).normalize();
	varInt q1 = X.naiveDivi(bpkmo, NULL), q2 = q1.normalize() * micro, q3 = q2.normalize().naiveDivi(bpkpo,NULL);
	varInt q3m = q3.normalize() * M, r = X - q3m.normalize();
	r.normalize();
	base.discard(); _2k.discard(); bp2k.discard(); micro.discard(); kmo.discard(); kpo.discard();
	bpkmo.discard(); bpkpo.discard(); q1.discard(); q2.discard(); q3.discard(); q3m.discard();
	while (!(r < M)) {
		r -= M;
		r.normalize();
		
	}
	M.discard();
	return r;
}
// varInt operator% (varInt b)
// give the result of *this mod b
// using naiveMod or barretReduction if the varInt
// is less than 65 bytes
// return remainder of *this / b
//Exception: NullPointerException
// Security level: mildly-unsafe
// Usefulness level: recommended
varInt varInt::operator% (varInt b) {
	if (b.length < 65)return (*this).naiveMod(b);
	else return barretReduction(*this, b);
}
// varInt fromString( const char* str)
// convert str to corresponding varInt
// str must be formated as an interger in base 10 (.e.g 100, 32378,etc) or
// in base 16 with 0x prepended like 0xFF, 0x0a
// return : varInt 
// Exception: NullPointerException
// Security level: mildly-unsafe
// Usefulness level: promoted
varInt varInt::fromString(const char* str) {
	bool hexadecimal = false;
	if (str[0] == '0' && str[1] == 'x')hexadecimal = true;
	varInt result(0);
	varInt b(hexadecimal ? 16 : 10);
	for (int i = strlen(str) - 1; i > (hexadecimal ? 1 : -1); --i) {
		varInt a(str[i] - 48 - ((hexadecimal && str[i] > 64) ?
			7 : 0)), c((int32_t)(strlen(str) - i - 1)), d = binaryPow(b.normalize(), c.normalize());
		
		varInt e = a.normalize()* d;
		result += e;
		a.discard(); c.discard(); d.discard(); e.discard();
	}
	b.discard();
	return result;
}
int64_t varInt::toInt64() {
	varInt copy(data, length, length);
	copy.normalize();
	int64_t result = 0;
	for (int i = 0; i < 4 && i < copy.length; ++i) {
		result |= ((int64_t)copy.data[i]) << (i * 8);
	}
	copy.discard();
	return result;
}
char* varInt::toString(varInt a) {
	varInt copy(a.data, a.length, a.length);
	copy.normalize();
	bool negativity = isNegative(copy);
	varInt b = abs(copy);
	copy.discard();
	copy = b;
	copy.normalize();
	varInt ten(10);
	ten.normalize();
	varInt zero(0);
	zero.normalize();
	if (copy == zero) {
		copy.discard();
		ten.discard();
		zero.discard();
		char* result = (char*)malloc(2);
		result[0] = 48;
		result[1] = 0;
		return result;
	}
	char* result = (char*)malloc(copy.length + 1);
	int i = 0 , currentSize = copy.length;
	while (copy > zero) {

		varInt remainder(0), c = copy.naiveDivi(ten, &remainder);
		copy.discard();
		copy = c;
		copy.normalize();
		result[i++] = (char)((int)remainder.toInt64() + 48);
		if (i == currentSize) {
			if (currentSize * 2 < INT_MAX)
				result = (char*)realloc(result, (currentSize *= 2) + 1);
			else if (currentSize + 2 < INT_MAX)
				result = (char*)realloc(result, currentSize += 2);
			else {
				printf("Literal base 10 conversion of varInt %p overflew\n", a);
				break;
			}
		}
	}
	copy.discard();
	ten.discard();
	zero.discard();
	char* finalResult = (char*)malloc(i + 1);
	for (int j = i - 1; j > -1; --j) {
		finalResult[i - j - 1] = result[j];
	}
	finalResult[i] = '\0';
	free(result);

	if (negativity) {
		char* ffResult = (char*)malloc(i + 2);
		memcpy(ffResult + 1, finalResult, i + 1);
		ffResult[0] = '-';
		free(finalResult);
		return ffResult;
	}
	return finalResult;
}
void varInt::printString() {
	char* lit = toString(*this);
	printf("%s\n", lit);
	free(lit);
}
varInt varInt::gcd(varInt a, varInt b) {
	varInt c(a.data, a.length, a.length), d(b.data, b.length, b.length); //deep copy varInt a and b
	c.normalize(), d.normalize();
	if (c == d) {
		d.discard();
		return c;
	}
	if (c < d) {
		swap(c, d);
	}
	varInt remainder(0), zero(0);
	remainder.normalize(); zero.normalize();
	
	while(true)
	{
		varInt quotient = c.naiveDivi(d, &remainder);
		quotient.discard();
		c.discard();
		if (remainder == zero) {
			remainder.discard();
			zero.discard();
			break;
		}
		else {
			c = d;
			d = remainder;
		}
	}
	return d;
}
void varInt::swap(varInt a, varInt b) {
	varInt c = b, d = a;
	a = c; b = d;
}
varInt varInt::bezoutIdentity(varInt a, varInt b, varInt* x, varInt* y) {
	varInt c(a.data, a.length, a.length), d(b.data, b.length, b.length); //deep copy varInt a and b
	c.normalize(), d.normalize();
	if (c == d) {
		d.discard();
		return c;
	}
	if (c < d) {
		swap(c, d);
	}
	varInt remainder(0), zero(0);
	remainder.normalize(); zero.normalize();
	varInt s0(1), s1(0), t0(0), t1(1);
	while (true)
	{
		varInt quotient = c.naiveDivi(d, &remainder);
		varInt s2, t2;
		varInt qisi = quotient * s1, qiti = quotient * t1;
		qisi.normalize(); qiti.normalize();
		s2 = s0 - qisi;
		t2 = t0 - qiti;
		qisi.discard();
		qiti.discard();
		s0.discard(); t0.discard();
		s0 = s1; t0 = t1;
		s1 = s2; t1 = t2;
		quotient.discard();
		c.discard();
		if (remainder == zero) {
			remainder.discard();
			zero.discard();
			s1.discard();t1.discard();
			break;
		}
		else {
			c = d;
			d = remainder;
		}
	}
	if (x != NULL)
	{
		x->discard();
		*x = s0;
	}
	else
		s0.discard();
	if (y != NULL) {
		y->discard();
		*y = t0;
	}
	else
		t0.discard();
	

	return d;
}
varInt varInt::modInverse(varInt a, varInt m) {
	varInt x(0), GCD = bezoutIdentity(a, m, &x, NULL);
	varInt one(1);
	varInt zero(0);
	one.normalize(); zero.normalize(); x.normalize();
	if (GCD != one) {
		one.discard();x.discard(); GCD.discard();
		printf("modInverse: a and m are not coprimes\n");
		
		return zero;
	}
	varInt c = x.naiveMod(m);
	x.discard();
	x = c;
	while (x < zero) {
		x += m;
		x.normalize();
	}
	zero.discard(); one.discard(); GCD.discard();
	return x;
}