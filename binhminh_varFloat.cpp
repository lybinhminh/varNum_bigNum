#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <cstdio>
#include "binhminh_varFloat.h"
void varFloat::discard() {
	integer.discard();
	numerator.discard();
	denominator.discard();
}
varFloat::varFloat(varInt a) {
	integer = varInt(a.data, a.length, a.length);
	integer.normalize();
	numerator = varInt(0);
	numerator.normalize();
	denominator = varInt(1);
	denominator.normalize();
}
varFloat::varFloat(varInt a, varInt b, varInt c) {
	integer = varInt(a.data, a.length, a.length);
	
	numerator = varInt(b.data,b.length,b.length);
	
	denominator = varInt(c.data,c.length,c.length);
	
	normalize();
}
void varFloat::cdc(varFloat& a, varFloat& b) {
	a.denominator.normalize();
	b.denominator.normalize();
	if (a.denominator == b.denominator)return; // cdc-ed
	varInt GCD = varInt::gcd(a.denominator, b.denominator), ab = a.denominator * b.denominator;
	varInt c = varInt::abs(ab); ab.discard();
	varInt LCM = c.naiveDivi(GCD, NULL);
	varInt aFactor = LCM.naiveDivi(a.denominator,NULL), bFactor = LCM.naiveDivi(b.denominator, NULL);
	a.numerator *= aFactor;
	a.denominator *= aFactor;
	b.numerator *= bFactor;
	b.denominator *= bFactor;
	c.discard();GCD.discard();
	LCM.discard();aFactor.discard();bFactor.discard();
}
varFloat varFloat::normalize() {
	integer.normalize();
	numerator.normalize();
	denominator.normalize();
	varInt remainder(0), quotient = numerator.naiveDivi(denominator, &remainder);
	integer += quotient;
	quotient.discard();
	numerator.discard();
	numerator = remainder;
	bool intNegativity = varInt::isNegative(integer),
		numeratorNegativity = varInt::isNegative(numerator),
		denominatorNegativity = varInt::isNegative(denominator);
	if (denominatorNegativity) {

		varInt a = varInt::negate(denominator), b = varInt::negate(numerator);
		denominator.discard(); denominator = a;
		numerator.discard(); numerator = b;
	}
	numeratorNegativity = varInt::isNegative(numerator);
	denominatorNegativity = varInt::isNegative(denominator);
	if (intNegativity && !numeratorNegativity) {
		varInt one(1);
		integer += one;
		varInt a = one * denominator;
		numerator -= a;
		one.discard(); a.discard();
	}
	if (!intNegativity && numeratorNegativity) {

		varInt one(1);
		integer -= one;
		varInt a = one * denominator;
		numerator += a;
		one.discard(); a.discard();
	}
	integer.normalize();
	numerator.normalize();
	denominator.normalize();
	return *this;
}
varFloat::varFloat(){}
varFloat varFloat::operator+(varFloat b) {
	varFloat c(integer, numerator, denominator), d(b.integer, b.numerator, b.denominator);
	varFloat sum;
	sum.integer = c.integer + d.integer;
	sum.integer.normalize();
	cdc(c, d);
	sum.numerator = c.numerator + d.numerator;
	sum.numerator.normalize();
	c.denominator.normalize();
	sum.denominator = varInt(c.denominator.data,c.denominator.length,c.denominator.length);
	c.discard();
	d.discard();
	return sum;
}
varFloat varFloat::operator-(varFloat b) {
	varFloat c(integer, numerator, denominator), d(b.integer, b.numerator, b.denominator);
	varFloat diff;
	diff.integer = c.integer - d.integer;
	diff.integer.normalize();
	cdc(c, d);
	diff.numerator = c.numerator - d.numerator;
	diff.numerator.normalize();
	c.denominator.normalize();
	diff.denominator = varInt(c.denominator.data, c.denominator.length, c.denominator.length);
	c.discard();
	d.discard();
	return diff;
}
varFloat varFloat::operator*(varFloat b) {
	

	/*
	varFloat c(integer, numerator, denominator), d(b.integer, b.numerator, b.denominator);
		varInt e, f, g, h, i;
	e = c.integer * d.integer;
	f = c.integer * d.numerator;
	g = d.integer * c.numerator;
	h = c.numerator * d.numerator;
	i = c.denominator * d.denominator;
	e.normalize(); f.normalize(); g.normalize(); h.normalize(); i.normalize();
	varFloat m(e, f, d.denominator), n(0, g, c.denominator), o(0, h, i),
		p = m + n, q = p + o;
	c.discard(); d.discard(); e.discard(), f.discard(); g.discard(); h.discard(); i.discard();
	m.discard(); n.discard(); o.discard(); p.discard();
	// mutlipart type: 4 visible multiplications, 3 varFloat additions, 2 cdc atleast

	return q;
	*/

	varFloat c = fullFraction(*this), d = fullFraction(b), e;
	e.integer = 0;
	e.numerator = c.numerator * d.numerator;
	e.denominator = c.denominator * d.denominator;
	c.discard();
	d.discard();
	// arithmetic type: 2 visible multiplication, 2 fullFraction(cheap mult and + of varInt)
	// , 1 varFloat normalize
	return e;
	
}
varFloat varFloat::operator/ (varFloat b) {
	varFloat c = fullFraction(*this), d = fullFraction(b), e;
	e.integer = 0;
	e.numerator = c.numerator * d.denominator;
	e.denominator = c.denominator * d.numerator;
	c.discard();
	d.discard();
	return e;
}
varFloat varFloat::fullFraction(varFloat a) {
	varInt b = a.integer * a.denominator;
	b.normalize();
	varFloat result;
	result.integer = 0;
	result.numerator = b + a.numerator;
	result.numerator.normalize();
	result.denominator = varInt(a.denominator.data, a.denominator.length, a.denominator.length);
	result.denominator.normalize();
	b.discard();
	return result;
}
char* varFloat::toString(varFloat a, size_t max_precision) {
	varFloat b(a.integer, a.numerator, a.denominator);

	varInt zero(0), decimalSum(0), one(1);
	zero.normalize(); decimalSum.normalize(); one.normalize();
	bool negativity = b.integer == zero && varInt::isNegative(b.numerator);
	char* integerPart = varInt::toString(b.integer);
	varInt ten(10); ten.normalize();
	varInt c = varInt::abs(b.numerator), d = varInt::abs(b.denominator);
	b.numerator.discard(); b.numerator = c;
	b.denominator.discard(); b.denominator = d;
	for (int i = 0; b.numerator != zero && i < max_precision; ++i) {
		decimalSum *= ten;
		b.numerator *= ten;
		
		varInt remainder(0), quotient = b.numerator.naiveDivi(b.denominator, &remainder);
		
		decimalSum += quotient;
		quotient.discard();
		b.numerator.discard();
		b.numerator = remainder;
	}
	b.discard();
	zero.discard(); ten.discard(); one.discard();
	char* decimalPart = varInt::toString(decimalSum);
	char* result = (char*)malloc(strlen(integerPart) + strlen(decimalPart) + 2);
	memcpy(result, integerPart, strlen(integerPart));
	result[strlen(integerPart)] = '.';
	memcpy(result + strlen(integerPart)+1, decimalPart, strlen(decimalPart));
	result[strlen(integerPart) + strlen(decimalPart) + 1] = '\0';
	if (negativity) {
		char* finalResult = (char*)malloc(strlen(integerPart) + strlen(decimalPart) + 3);
		strcpy(finalResult + 1, result);
		finalResult[0] = '-';
		free(result);
		return finalResult;
	}
	return result;
}
void varFloat::printString(size_t precision) {
	char* literal = toString(*this, precision);
	printf("%s\n", literal);
	free(literal);
}
varFloat varFloat::fromString(const char* string) {
	char* copy = (char*)malloc(strlen(string) + 1);
	strcpy(copy, string);
	char* integerPart = strtok(copy, ".");
	char* decimalPart = strtok(NULL, ".");
	varFloat result;
	
	result.integer = varInt::fromString(integerPart);
	if (decimalPart != NULL)
	{
		result.numerator = varInt::fromString(decimalPart);
		varInt ten(10), power((int32_t)strlen(decimalPart));
		ten.normalize();  power.normalize();
		result.denominator = varInt::binaryPow(ten, power);
		ten.discard();
		power.discard();
	}
	else {
		result.numerator = 0;
		result.denominator = 1;
	}
	free(copy);
	return result;
}
varFloat varFloat::operator+= (varFloat b) {
	varFloat c = *this + b;
	discard();
	integer = c.integer;
	numerator = c.numerator;
	denominator = c.denominator;
	return *this;
}
varFloat varFloat::operator-= (varFloat b) {
	varFloat c = *this - b;
	discard();
	integer = c.integer;
	numerator = c.numerator;
	denominator = c.denominator;
	return *this;
}
varFloat varFloat::operator*= (varFloat b) {
	varFloat c = *this * b;
	discard();
	integer = c.integer;
	numerator = c.numerator;
	denominator = c.denominator;
	return *this;
}
varFloat varFloat::operator/= (varFloat b) {
	varFloat c = *this / b;
	discard();
	integer = c.integer;
	numerator = c.numerator;
	denominator = c.denominator;
	return *this;
}
varFloat::varFloat(float f) {
	union {
		float f;
		unsigned char ch[4];
	} extract = { f };
	int8_t sign = (extract.ch[3] >> 7) == 1 ? -1 : 1;
	int8_t exponent = ((extract.ch[3] << 1) | (extract.ch[2] >> 7)) - 127;
	int32_t mantissa = (extract.ch[2] & 0x7f ) << 16 | extract.ch[1] << 8 | extract.ch[0];
	numerator = (varInt)(int32_t)(sign * mantissa);
	denominator = (varInt)(int32_t)pow(2, 23);
	integer = sign;
	if (exponent < 0) {
		numerator._auto_assign_shift_arithmetic_ = true;
		numerator >> -exponent;
		integer._auto_assign_shift_arithmetic_ = true;
		integer >> -exponent;
		
	}
	else {
		numerator._auto_assign_shift_arithmetic_ = true;
		numerator << exponent;
		integer._auto_assign_shift_arithmetic_ = true;
		integer << exponent;
		
	}
	
	varInt a(sign); a.normalize();
	//integer *= a;
	//numerator *= a;
	a.discard();
	
}
varFloat::varFloat(double d) {
	union {
		double d;
		unsigned char ch[8];
	} extract = { d };
	int8_t sign = (extract.ch[7] >> 7) == 1 ? -1 : 1;
	int16_t exponent = (((extract.ch[7] & 0x7f) << 4) | (extract.ch[6] >> 4)) - 1023;
	int64_t mantissa = (((uint64_t)extract.ch[6] & 0x0f) << 48) | ((uint64_t)extract.ch[5] << 40) | ((uint64_t)
		extract.ch[4] << 32)| ((uint64_t)extract.ch[3] << 24) | ((uint64_t)
			extract.ch[2] << 16) | ((uint64_t)extract.ch[1] << 8) | extract.ch[0];
	numerator = (varInt)(int64_t)(sign * mantissa);
	denominator = (varInt)(int64_t)pow(2, 52);
	integer = sign;
	if (exponent < 0) {
		numerator._auto_assign_shift_arithmetic_ = true;
		numerator >> -exponent;
		integer._auto_assign_shift_arithmetic_ = true;
		integer >> -exponent;

	}
	else {
		numerator._auto_assign_shift_arithmetic_ = true;
		numerator << exponent;
		integer._auto_assign_shift_arithmetic_ = true;
		integer << exponent;

	}
}
bool varFloat::isNegative(varFloat a) {
	varFloat b(a.integer, a.numerator, a.denominator);
	varInt zero(0); zero.normalize();
	if (b.integer == zero) {
		if (varInt::isNegative(b.numerator)) {
			zero.discard();
			b.discard();
			return true;
		}
		else
			return false;
	}
	bool result = varInt::isNegative(b.integer);
	b.discard();
	return result;
}
bool varFloat::operator==(varFloat b) {
	varFloat c = fullFraction(*this), d = fullFraction(b);
	cdc(c, d);
	bool result = c.numerator == d.numerator && c.denominator == d.denominator;
	c.discard(); d.discard();
	return result;
}
bool varFloat::operator!=(varFloat b) {
	return !(*this == b);
}
bool varFloat::operator>(varFloat b) {
	varFloat c = fullFraction(*this), d = fullFraction(b);
	cdc(c, d);
	bool cNegativity = varInt::isNegative(c.numerator) ^ varInt::isNegative(c.denominator),
		dNegativity = varInt::isNegative(d.numerator) ^ varInt::isNegative(d.denominator);
	if (cNegativity && !dNegativity) {
		c.discard(); d.discard();
		return false;
	}
	else if (!cNegativity && dNegativity) {
		c.discard(); d.discard();
		return true;
	}
	varInt cN = varInt::abs(c.numerator), dN = varInt::abs(d.numerator);
	c.discard(); d.discard();
	cN.normalize(); dN.normalize();
	if (cN > dN) {
		cN.discard(); dN.discard();
		return true;
	}
	else {
		cN.discard(); dN.discard();
		return false;
	}
}
bool varFloat::operator< (varFloat b) {
	return *this != b && !(*this > b);
}