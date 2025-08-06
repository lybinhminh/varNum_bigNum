
#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <algorithm>
#include "binhminh_varInt.h"
#include <vector>

varInt::varInt(int64_t origin) {
	data = (byte*)malloc(8);
	bool neg = origin < 0;
	int j = 0;
	for (int i = 0; i < 8; ++i) {
		unsigned char current = origin >> (i * 8);
		if (i > 0) {
			unsigned char last = origin >> ((i - 1) * 8);
			if (current == 0 && !neg) {
				if ((last & 0x80) == 0x80) {
					data[j++] = current;
				}
			}
			else if (current == 255 && neg) {
				if ((last & 0x80) == 0) {
					data[j++] = current;
				}
			}
			else {
				data[j++] = current;
			}
		}
		else {
			data[j++] = current;
		}
	}
	length = j;
	if (j < 8)
		data = (unsigned char*)realloc(data, j);
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
	if(length > 0)
	{
		free(data);
		length = 0;
	}
	
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
	return (a.data[a.length - 1] & 0x80) == 0x80;
}
// constructor varInt(int32_t v)
// creates a new varInt and copy all 4 bytes from int32_t v to it
// Exception: no
// Security level: safe
// Usefulness level: recommended
varInt::varInt(int32_t origin) {
	data = (byte*)malloc(4);
	int j = 0;
	bool neg = origin < 0;
	unsigned char last = 1;
	unsigned char current = 0; 
	for (int i = 0; i < 4; ++i) {
		current = origin >> (i * 8);
		if (i > 0) {
			last = origin >> ((i - 1) * 8);
			if (current == 0 && !neg) {
				if ((last & 0x80) == 0x80) {
					data[j++] = current;
				}
			}
			else if (current == 255 && neg) {
				if ((last & 0x80) == 0) {
					data[j++] == current;
				}
			}
			else {
				data[j++] = current;
			}
		}
		else {
			data[j++] = current;
		}
	}
	length = j;
	if(j != 4)
	data = (unsigned char*)realloc(data, j);
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
	bool negativity = (data[length - 1] & 0x80) != 0;
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
	int b_length = b.length, max = std::max((int)length, b_length);
	bool neg = (data[length - 1] & 0x80) != 0, b_neg = (b.data[b_length - 1] & 0x80) != 0;
	varInt result;
	result.length = max;
	result.data = (unsigned char*)malloc(max);
	uint8_t carry = 0;
	for (int i = 0; i < max; ++i) {
		uint8_t c = neg ? 255 : 0 , d = b_neg ? 255 : 0;
		if (i < length)c = data[i];
		if (i < b_length)d = b.data[i];
		uint16_t sum = c + d + carry;
		result.data[i] = (uint8_t)sum;
		carry = sum >> 8;
	}
	if (!neg && !b_neg) {
		// pos + pos
		if ((result.data[max - 1] & 0x80) != 0) {
			// if the most significant bit is set
			result.length++;
			result.data = (unsigned char*)realloc(result.data, max + 1);
			result.data[max] = 0x00;
		}
	}
	if (neg && b_neg && (result.data[max - 1] & 0x80) == 0) {
		// neg + neg but the most significant bit is 0
		result.length++;
		result.data = (unsigned char*)realloc(result.data,max + 1);
		result.data[max] = 0xFF;
	}
	return result;
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
	int b_length = b.length, max = std::max((int)length, b_length);
	bool neg = (data[length - 1] & 0x80) != 0, b_neg = (b.data[b_length - 1] & 0x80) != 0;
	if(max > length)
	data = (unsigned char*)realloc(data, max);
	uint8_t carry = 0;
	for (int i = 0; i < max; ++i) {
		uint8_t c = neg ? 255 : 0, d = b_neg ? 255 : 0;
		if (i < length)c = data[i];
		if (i < b_length)d = b.data[i];
		uint16_t sum = c + d + carry;
		data[i] = (uint8_t)sum;
		carry = sum >> 8;
	}
	length = max;
	if (!neg && !b_neg) {
		if ((data[max - 1] & 0x80) != 0) {
			length++;
			data = (unsigned char*)realloc(data, max + 1);
			data[max] = 0x00;
		}
	}
	if (neg && b_neg && (data[max - 1] & 0x80) == 0) {
		length++;
		data = (unsigned char*)realloc(data, max + 1);
		data[max] = 0xFF;
	}
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
	int b_length = b.length, max = std::max((int)length, b_length);
	bool neg = (data[length - 1] & 0x80) != 0,
		b_neg = (b.data[b_length - 1] & 0x80) != 0;
	uint8_t borrow = 0;
	varInt result;
	result.length = max;
	result.data = (unsigned char*)malloc(max);
	for (int i = 0; i < max; ++i) {
		uint8_t c = neg ? 255 : 0, d = b_neg ? 255 : 0;
		if (i < length)c = data[i];
		if (i < b_length)d = b.data[i];
		uint16_t diff = c - d - borrow;
		result.data[i] = (uint8_t)diff;
		borrow = diff > 255;
	}
	if (!neg && b_neg && (result.data[max - 1] & 0x80) != 0) {
		result.length++;
		result.data = (unsigned char*)realloc(result.data, max + 1);
		result.data[max] = (uint8_t)0;
	}
	if (neg && !b_neg && (result.data[max - 1] & 0x80) == 0) {
		result.length++;
		result.data = (unsigned char*)realloc(result.data, max + 1);
		result.data[max] = (uint8_t)255;
	}
	return result;
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
	int b_length = b.length, max = std::max((int)length, b_length);
	bool neg = (data[length - 1] & 0x80) != 0,
		b_neg = (b.data[b_length - 1] & 0x80) != 0;
	if(max > length)
	data = (unsigned char*)realloc(data, max);
	uint8_t borrow = 0;
	for (int i = 0; i < max; ++i) {
		uint8_t c = neg ? 255 : 0, d = b_neg ? 255 : 0;
		if (i < length)c = data[i];
		if (i < b_length)d = b.data[i];
		uint16_t diff = c - d - borrow;
		data[i] = (uint8_t)diff;
		borrow = diff > 255;
	}
	length = max;
	if (!neg && b_neg && (data[max - 1] & 0x80) != 0) {
		length++;
		data = (unsigned char*)realloc(data, max + 1);
		data[max] = (uint8_t)0;
	}
	if (neg && !b_neg && (data[max - 1] & 0x80) == 0) {
		length++;
		data = (unsigned char*)realloc(data, max + 1);
		data[max] = (uint8_t)255;
	}
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
	bool negativity = ((data[length - 1] & 0x80) == 0x80) ^
		((b.data[b.length - 1] & 0x80) == 0x80);
	if ((length == 1 && data[0] == 0) || (b.length == 1 && b.data[0] == 0)) {
		varInt zero(0); zero.normalize();
		return zero;
	}
	varInt multiplicand, multiplier;
	if (length > b.length) {
		multiplicand = abs(*this);
		multiplier = abs(b);
	}
	else {
		multiplicand = abs(b);
		multiplier = abs(*this);
	}
	size_t lastShift = 0;
	varInt product = 0;
	for (int i = multiplier.length - 1; i > -1; --i) {
		for (int j = 7; j > -1; --j) {
			int bit = (multiplier.data[i] >> j) & 0x01;
			if (bit == 0)continue;
			if (lastShift == 0) {
				lastShift = i * 8 + j;
				multiplicand <<= lastShift;
				
				product += multiplicand;
			}
			else {
				uint16_t shift = lastShift - i * 8 - j;
				multiplicand >>= shift;
				product += multiplicand;
				lastShift -= shift;
			}
		}
	}
	multiplier.discard(); multiplicand.discard();
	if (negativity) {
		for (int i = 0; i < product.length; ++i) {
			product.data[i] = ~product.data[i];
		}
		product += one;
	}
	return product;
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
	if (by == 0) {
		if (_auto_assign_shift_arithmetic_)return *this;
		return varInt(data, length, length);
	}

	varInt result;
	int _intended_rLength = length + ((by & 7) != 0 ? 1 + by / 8 : by / 8);
	if (!_do_left_shift_expand_)_intended_rLength = length;
	result.length = _intended_rLength;
	result.data = (byte*)calloc(result.length,1);
	bool negativity = (data[length - 1] & 0x80) != 0;
	int j = by % 8, x = 0, y = by / 8;
	for (int i = 0; i < result.length;i++) {
		if (i < y) {
			result.data[i] = 0;
		}
		else if (j == 0) {
			result.data[i] = data[x++];
		}
		else {
			if (i == y) {
				result.data[i] = data[x] << j;
				continue;
			}
			result.data[i] = data[x++] >> (8 - j);
			if (x == length)break;
			result.data[i] |= data[x] << j;
		}
	}
	int a = result.length - 1;
	if (negativity && (result.data[a] & 0x80) == 0) {
		// in this case there will be atlest 1 zero at the top of result
		for (int i = 7; i > 0; --i) {
			int bit = (result.data[a] >> i) & 0x01;
			if (bit == 1)break;
			result.data[a] |= 1 << i;
		}
	}else if(!negativity && (result.data[a] & 0x80 )!= 0) {
		result.length++;
		result.data = (unsigned char*)realloc(result.data, result.length);
		result.data[result.length - 1] = 0x00;
	}
	if (_auto_assign_shift_arithmetic_) {
		free(data);
		data = result.data;
		length = result.length;
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
	if (by == 0) {
		if (_auto_assign_shift_arithmetic_)return *this;
		else return varInt(data, length, length);
	}
	size_t  a = by / 8, b = by % 8 , x = length - 1;
	varInt result;
	if (result.length <= a) {
		varInt zero(0); zero.normalize();
		if (_auto_assign_shift_arithmetic_) {
			free(data);
			data = zero.data;
			length = 1;
			return *this;
		}
		return zero;
	}
	else result.length -= a;
	result.data = (byte*)calloc(result.length,1);
	
	bool negativity = (data[length - 1] & 0x80) != 0;
	for (int i = result.length - 1; i > -1; --i) {
		if (b == 0) {
			result.data[i] = data[x--];
		}
		else {
			if (i == result.length - 1) {
				result.data[i] = data[x] >> b;
				result.data[i] |= (negativity ? 255: 0) << (8 - b);
				continue;
			}
			result.data[i] = data[x--] << (8 - b);
			if (x == -1)break;
			result.data[i] |= data[x] >> b;
		}
	}
	if (_auto_assign_shift_arithmetic_) {
		
		free(data);
		data = result.data;
		length = result.length;
		return *this;
	}
	return result;

}
// bool operator> (varInt b)
// compares if the current varInt is bigger than varInt 'b'
// return: *this > b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator> (const varInt& b) const {
	bool neg = (data[length - 1] & 0x80) == 0x80,
		b_neg = (b.data[b.length - 1] & 0x80) == 0x80;
	if (neg && !b_neg)return false;
	else if (!neg && b_neg)return true;
	if (length != b.length)
		if (neg)
			return length < b.length; // |a| > |b| so a < b (a,b < 0)
		else return length > b.length;
	else
	{
		for (int i = length -1; i > -1; --i) {
			uint16_t c = data[i], d = b.data[i];
			if (c > d)return true; //
			if (c < d)return false;
		}
		return false; // equals
	}
}
// bool operator> (varInt b)
// compares if the current varInt is equal to varInt 'b'
// return: *this == b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator== (const varInt& b) const {
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
bool varInt::operator< (const varInt& b) const {
	bool neg = (data[length - 1] & 0x80) == 0x80,
		b_neg = (b.data[b.length - 1] & 0x80) == 0x80;
	if (neg && !b_neg)return true;
	else if (!neg && b_neg)return false;
	if (length != b.length)
		if (neg)
			return length > b.length;
		else return length < b.length;
	else
	{
		for (int i = length - 1; i > -1; --i) {
			uint16_t c = data[i], d = b.data[i];
			if (c > d)return false; //
			if (c < d)return true;
		}
		return false; // equals
	}
}
// bool operator!= (varInt b)
// compares if the current varInt is not equal to varInt 'b'
// return: *this != b
// Security level: safe
// Usefulness level: recommended
bool varInt::operator!= (const varInt& b) const {
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
	return result;
}
// varInt abs(varInt a)
// return absolute value of varInt 'a'
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::abs(varInt a) {
	if ((a.data[a.length - 1] & 0x80) != 0x80) {
		return varInt(a.data,a.length,a.length);
	}
	else {
		varInt b = ~a; b += one;
		return b;
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
// varInt (varInt b, varInt& [out, nullable] remainder)
// perform bit-by-bit divison with the current varInt as dividend and
// varInt 'b' as divisor, return the remainder as demand
// return: the quotient
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: promoted
varInt varInt::naiveDivi(varInt b, varInt* remainderOut) {
	bool q_neg = ((data[length - 1] & 0x80) ^ (b.data[b.length - 1] & 0x80 )) == 0x80,
		remainder_neg = (data[length - 1] & 0x80) == 0x80;
	if (length < b.length) {
		if (remainderOut != NULL)
		{
			remainderOut->discard();
			*remainderOut = varInt(data, length, length);
		}
		return varInt(0);
	}
	if (b == zero) {
		printf("naiveDivi: divided by 0 spotted!\n");
		return NULL;
	}
	varInt dividend = abs(*this), divisor = abs(b), remainder, quotient(0);
	int currentByte = dividend.length - 1;
	int k = 0, j = 1;
	if(divisor.length > 1)
	{
		currentByte -= (divisor.length - 1);
		remainder.length = dividend.length - currentByte - 1;
		remainder.data = (unsigned char*)malloc(remainder.length);
		memcpy(remainder.data, dividend.data + currentByte + 1, dividend.length - currentByte - 1);
	}
	else
	{
		remainder.length = 1;
		remainder.data = (unsigned char*)calloc(1, 1);
	}
	remainder._do_left_shift_expand_ = false;
	quotient._do_left_shift_expand_ = false;
	for (; currentByte > -1; --currentByte) {
		for (int i = 7; i > -1; --i) {
			int bit = getBit(dividend, currentByte, i);
			if ((k & 7) != 0)
				remainder <<= 1;
			else 
			{
				remainder._do_left_shift_expand_ = true;
				remainder <<= 1;
				remainder._do_left_shift_expand_ = false;
			}
			if (isNegative(remainder)) {
				remainder.length++;
				remainder.data = (unsigned char*)realloc(remainder.data, remainder.length);
				remainder.data[remainder.length - 1] = 0x00;
			}
			remainder.normalize();
			k++;
			remainder.data[0] |= bit;
			if (remainder < divisor) {
				if ((j & 7) != 0)
					quotient <<= 1;
				else
				{
					quotient._do_left_shift_expand_ = true;
					quotient <<= 1;
					quotient._do_left_shift_expand_ = false;
				}
				j++;
			}
			else {
				remainder -= divisor;
				remainder.normalize();
				if ((j & 7) != 0)
					quotient <<= 1;
				else
				{
					quotient._do_left_shift_expand_ = true;
					quotient <<= 1;
					quotient._do_left_shift_expand_ = false;
				}
				j++;
				quotient.data[0] |= 1;
			}
		}
	}
	dividend.discard(); divisor.discard();
	quotient.normalize();
	if(remainderOut != NULL)
	{
		if (remainder_neg) {
			for (int i = 0; i < remainder.length; ++i) {
				remainder.data[i] = ~remainder.data[i];
			}
			remainder += one;
		}
		remainderOut->discard();
		*remainderOut = remainder;
	}
	else remainder.discard();
	if (q_neg) {
		for (int i = 0; i < quotient.length; ++i) {
			quotient.data[i] = ~quotient.data[i];
		}
		quotient += one;
	}
	return quotient;
}
// varInt negate(varInt a)
// reverse sign of varInt a
// return a reverse version of 'a'
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::negate(varInt a) {
	if (a.length == 1 && a.data[0] == 0) {
		return varInt(0);
	}
	varInt flipped = ~a;
	flipped += one;
	return flipped;
}
// varInt karatsubaMult(varInt b)
// perform multiplication of a and b using karatsuba's fast multiplication ( based
// on recursive calls)
// Exception: NullPointerException, OverflowException
// Security level: mildly-unsafe
// Usefulness level: recommended for multiplication involving >256-byte varInt
varInt varInt::karatsubaMult(varInt b) {
	if (length < NAIVE_MULT_THRESHOLD || b.length << NAIVE_MULT_THRESHOLD) {
		return (*this).naiveMult(b);
	}
	size_t max = std::max(length, b.length), k = (max + 1) / 2;
	varInt A = abs(*this), B = abs(b), a_low(A.data, k, k), a_high(A.data + k, A.length - k, max - k),
		b_low(B.data, k, k), b_high(B.data + k, B.length - k, max - k);
	A.discard(); B.discard();
	varInt Z0 = a_low * b_low, Z2 = a_high * b_high, temp0 = a_low + a_high,
		temp1 = b_low + b_high, Z1 = temp0 * temp1; Z1 -= Z0; Z1 -= Z2;

	temp0.discard(); temp1.discard();
	Z2 <<= 16 * k;
	Z1 <<= 8 * k;
	Z2 += Z1; Z2 += Z0;
	Z0.discard(); Z1.discard();
	return Z2;
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
	return this->karatsubaMult(b);
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
			int bit = (v.data[i] >> j) & 0x01;
			if (bit != Bit)break;
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
	data = result.data;
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
	if (b < zero)return varInt(0);
	varInt power(1); varInt exponent(b.data, b.length, b.length);
	while (exponent > zero) {
		power *= a;
		exponent-= one;
	}
	exponent.discard();
	return power;
}
varInt varInt::operator/=  (varInt b) {
	varInt result = this->naiveDivi(b,NULL);
	discard();
	length = result.length;
	data = result.data;
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
	varInt remainder;
	this->naiveDivi(b, &remainder).discard();
	return remainder;
}
// varInt binaryPow(varInt a, varInt b)
// using binary exponentation to calculate a^b quickly
// return the power
// Exception: NullPointerException, OverflowException
// Security level: mildly unsafe
// Usefulness level: recommended
varInt varInt::binaryPow(varInt a, varInt b) {
	varInt B(b.data, b.length, b.length), power(1), A(a.data, a.length, a.length);
	while (B > zero) {
		if ((B.data[0] & 0x01) != 0) {
			power *= A;
		}
		A *= A;
		B /= two;
	}
	A.discard(); B.discard();
	return power;
}
// varInt barretReduction (varInt x, varInt m)
// calculate x mod m using barret technique
// unusable for x.length and m.length < 2
// return: the remainder
// Exception: NullPointerException
// Security level: mildly unsafe
// Usefulness level: useful
varInt varInt::barretReduction(varInt x, varInt n) {
	if (x < n) {
		return varInt(x.data, x.length, x.length);
	}
	if (x == n) {
		return varInt(0);
	}
	size_t k = x.length * 8 - countLeadingBit(x, 0);
	varInt m;
	if (!n._barret_cache_ || (n._barret_cache_ && n.barret_cache == NULL))
	{
		varInt tpk(2); tpk <<= k/8; m = tpk.naiveDivi(n, NULL);

		if (n._barret_cache_) {
			n.barret_cache = &m;
		}
		tpk.discard();
	}
	else{
		m = *n.barret_cache;
	}
	varInt q = x * m;
	q >>= k; q *= n; varInt r = x - q;
	while (!(r < n)){
		r -= n;
		r.normalize();
	}
	if(!n._barret_cache_)m.discard(); 
	q.discard();
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
	bool negative = (str[0] == 45);
	if (str[0] == '0' && str[1] == 'x')hexadecimal = true;
	varInt result(0);
	varInt b(hexadecimal ? 16 : 10); b.normalize();
	for (int i = strlen(str) - 1; i > (hexadecimal ? 1 : -1) + negative ? 1 : 0; --i) {
		varInt a(str[i] - 48 - ((hexadecimal && str[i] > 64) ?
			7 : 0)), c((int32_t)(strlen(str) - i - 1)), d = binaryPow(b, c.normalize());
		
		varInt e = a.normalize()* d;
		result += e;
		a.discard(); c.discard(); d.discard(); e.discard();
	}
	b.discard();
	if (negative) {
		varInt signed_result = negate(result);
		result.discard();
		return signed_result;
	}
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
	bool negativity = isNegative(a);
	varInt copy = abs(a);
	varInt ten(10); ten.normalize();
	varInt zero(0); zero.normalize();
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
		result[i++] = remainder.data[0] + 48;
		if (i == currentSize) {
			if (currentSize * 2 < INT_MAX)
				result = (char*)realloc(result, (currentSize *= 2) + 1);
			else if (currentSize + 2 < INT_MAX)
				result = (char*)realloc(result, currentSize += 2);
			else {
				printf("Literal base 10 conversion of varInt %p overflew\n", &a);
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
	finalResult[i] = NULL;
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
	varInt e = abs(c), f = abs(d);
	c.discard(); c = e;
	d.discard(); d = f;
	varInt zero(0); zero.normalize();
	if (c == d) {
		d.discard();
		if (c == zero) {
			printf("gcd: both a and b are 0, returning -INT64_MAX\n");
			c.discard();
			c = varInt(-INT64_MAX);
		}
		zero.discard();
		return c;
	}
	if (c < d) {
		swap(c, d);
	}

	if (c == zero) {
		c.discard();
		zero.discard();
		return d;
	}
	if (d == zero) {
		d.discard();
		zero.discard();
		return c;
	}
	varInt remainder(0);
	remainder.normalize();
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
	varInt e = abs(c), f = abs(d);
	c.discard(); c = e;
	d.discard(); d = f;
	varInt zero(0); zero.normalize();
	if (c == d) {
		d.discard();
		if (c == zero) {
			printf("gcd: both a and b are 0, returning -INT64_MAX\n");
			c.discard();
			c = varInt(-INT64_MAX);
		}
		zero.discard();
		return c;
	}
	if (c < d) {
		swap(c, d);
	}
	varInt s0(1), s1(0), t0(0), t1(1);
	if (c == zero) {
		c.discard();
		zero.discard();
		if( x != NULL)
		{
			x->discard();
			*x = s1;
		}
		else s1.discard();
		if(y != NULL)
		{
			y->discard();
			*y = t1;
		}
		else t1.discard();
		s0.discard();
		t0.discard();
		return d;
	}
	if (d == zero) {

		d.discard();
		zero.discard();
		if (x != NULL)
		{
			x->discard();
			*x = s0;
		}
		else s0.discard();
		if (y != NULL)
		{
			y->discard();
			*y = t0;
		}
		else t0.discard();
		s1.discard();
		t1.discard();
		return c;
	}
	varInt remainder(0);
	remainder.normalize();
	// s(i + 1) = s(i-1) - q(i) * s(i);
	// t(i + 1) = t(i - 1) - q)i * t(i)
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
	GCD.discard();
	varInt one(1);
	varInt zero(0);
	one.normalize(); zero.normalize(); x.normalize();
	if (GCD != one) {
		one.discard();x.discard();
		printf("modInverse: a and m are not coprimes\n");
		return zero;
	}
	one.discard(); 
	varInt c = x.naiveMod(m);
	x.discard();
	x = c;
	while (x < zero) {
		x += m;
		x.normalize();
	}
	zero.discard(); 
	return x;
}
varInt varInt::Toom3Mult(varInt b) {
	if (length < KARATSUBA_THRESHOLD || b.length < KARATSUBA_THRESHOLD)
		return this->karatsubaMult(b);
	bool negativity = isNegative(*this) ^ isNegative(b);
	varInt c = abs(*this), d = abs(b);
	size_t m = std::max(c.length, b.length);
	size_t n = m / 3 + 1;
	varInt e(c.data, c.length, m), f(d.data, d.length, m);
	c.discard(); d.discard();
	varInt e0(e.data, n, n), e1(e.data + n, n, n), e2(e.data + 2*n, m - 2*n, m - 2*n),
		f0(f.data, n, n), f1(f.data + n, n, n), f2(f.data + 2*n, m - 2*n, m - 2*n);
	varInt p[5], q[5], r[5];
	for (int i = -2; i <= 2; ++i) {
		varInt sqI((int32_t)std::pow(i, 2)); sqI.normalize();
		varInt I(i); I.normalize();
		p[i + 2] = e2 * sqI;
		p[i + 2] += e1 * I;
		p[i + 2] += e0;
		q[i + 2] = f2 * sqI;
		q[i + 2] += f1 * I;
		q[i + 2] += f0;
		sqI.discard(); I.discard();
	}
	for (int i = 0; i < 5; ++i) {
		r[i] = p[i].Toom3Mult(q[i]);
	}
	return r[0];
	e0.discard(); e1.discard(); e2.discard(); f0.discard(); f1.discard(); f2.discard();
	e.discard(); f.discard();
}
varInt varInt::operator<<= (size_t by) {
	if (_auto_assign_shift_arithmetic_)
	{
		*this << by;
		return *this;
	}
	varInt result = *this << by;
	free(data);
	data = result.data;
	length = result.length;
	return *this;
}
varInt varInt::operator>>= (size_t by) {
	if (_auto_assign_shift_arithmetic_)
	{
		*this >> by;
		return *this;
	}
	varInt result = *this >> by;
	discard();
	data = result.data;
	length = result.length;
	return *this;
}
varInt varInt::binaryPow(varInt v, size_t exponent) {
	varInt power(1), V(v.data, v.length, v.length);
	while (exponent > 0) {
		if ((exponent & 0x01) != 0) {
			power *= V;
		}
		V *= V;
		exponent /= 2;
	}
	V.discard();
	return power;
}
varInt varInt::pow(varInt v, size_t exponent) {
	if (exponent < 0) {
		return varInt(0);
	}
	varInt power(1);
	while (exponent > 0) {
		power *= v;
		exponent--;
	}
	return power;
}
varInt varInt::barretFastDivision(varInt n,varInt* remainderOut) {
	varInt aq;
	if (!n._barret_cache_ || (n._barret_cache_ && n.barret_cache == NULL)) {
		aq = 1;
		uint64_t k = length * 8 + 1;
		aq <<= k; aq /= n;
		aq *= *this; aq >>= n.length;
		if (_barret_cache_) {
			varInt aqc = varInt(aq.data, aq.length, aq.length);
			n.barret_cache = &aqc;
		}
	}
	else
	{
		varInt cached = *n.barret_cache;
		aq = varInt(cached.data, cached.length, cached.length);
	}
	varInt aqn = aq * n, r = *this - aqn;
	varInt error(0);
	printf("Approximated R = ");
	r.printString();
	while (!(r < n)) {
		r -= n; r.normalize();
		error += one;
	}
	aq += error; error.discard();
	aq.normalize();
	if (remainderOut != NULL) {
		remainderOut->discard();
		*remainderOut = r;
	}
	else r.discard();
	return aq;
}