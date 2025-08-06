#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <cstdio>
#include <cmath>
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
	numerator.normalize();
	bool intNegativity = varInt::isNegative(integer),
		numeratorNegativity = varInt::isNegative(numerator),
		denominatorNegativity = varInt::isNegative(denominator);
	varInt zero(0); zero.normalize();
	if (numerator == zero) {
		denominator.discard();denominator = varInt(1);denominator.normalize();
	}
	else if (denominatorNegativity) {
		varInt a = varInt::negate(denominator), b = varInt::negate(numerator);
		denominator.discard(); denominator = a;
		numerator.discard(); numerator = b;
	}
	numeratorNegativity = varInt::isNegative(numerator);
	denominatorNegativity = varInt::isNegative(denominator);
	// sign unify
	// e.g -int + +fraction -> -int + -fraction ; +int + -fraction -> +int + +fraction
	integer.normalize();
	numerator.normalize();
	varInt one(1); one.normalize();
	if (integer != zero && numerator != zero) {
		
		if (intNegativity && !numeratorNegativity) {
			integer += one;
			numerator -= denominator;
		}
		if (!intNegativity && numeratorNegativity) {
			integer -= one;
			numerator += denominator;
		}
	}
	numerator.normalize();
	denominator.normalize();
	if (numerator != zero) {
		varInt GCD = varInt::gcd(numerator, denominator);
		GCD.normalize();
		if (GCD != one) {
			numerator /= GCD;
			denominator /= GCD;
		}
		GCD.discard();
	}
	integer.normalize();
	numerator.normalize();
	denominator.normalize();
	zero.discard();
	one.discard();
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
	bool leadingZero = true;
	int leadingZeroCount = 0;
	for (int i = 0; b.numerator != zero && i < max_precision; ++i) {
		decimalSum *= ten;
		b.numerator *= ten;
		
		varInt remainder(0), quotient = b.numerator.naiveDivi(b.denominator, &remainder);
		
		decimalSum += quotient;
		if (quotient != zero && leadingZero)leadingZero = false;
		if (quotient == zero && leadingZero)leadingZeroCount++;
		quotient.discard();
		b.numerator.discard();
		b.numerator = remainder;
	}
	b.discard();
	zero.discard(); ten.discard(); one.discard();
	char* decimalPart = varInt::toString(decimalSum);
	if (leadingZeroCount > 0 ) {
		char* e = (char*)malloc(leadingZeroCount + strlen(decimalPart) + 1);
		memset(e, 48, leadingZeroCount);
		strcpy(e + leadingZeroCount, decimalPart);
		if (leadingZeroCount + strlen(decimalPart) > max_precision) {
			e = (char*)realloc(e,max_precision + 1);
			e[max_precision] = '\0';
		}
		free(decimalPart);
		decimalPart = e;
	}
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
		unsigned char b[4];
	} extract = { f };
	int8_t sign = extract.b[3] >> 7 == 0 ? 1 : -1;
	varInt SIGN(sign); SIGN.normalize();
	uint8_t biased_exponent = extract.b[3] << 1 | extract.b[2] >> 7;
	int8_t unbiased_exponent = biased_exponent - 127;
	int32_t mantissa = ((int32_t)(extract.b[2] & 0x7f)) << 16 | (int32_t)extract.b[1] << 8 | (int32_t)extract.b[0];
	varInt two(2); two.normalize();
	numerator = varInt(mantissa);
	numerator *= SIGN;
	integer = varInt(0);
	if (mantissa != 0 && biased_exponent == 0) {
		// subnormal (denormalized)
		varInt exp(149); exp.normalize();
		denominator = varInt::binaryPow(two, exp);
		exp.discard();
	}
	else {
		// sign . (2 ^ 23 + mantissa)/ 2^(23 - exp)
		varInt two_pow_23((int32_t)std::pow(2, 23)); two_pow_23.normalize();
		two_pow_23 *= SIGN;
		numerator += two_pow_23;
		two_pow_23.normalize();
		varInt denominator_exp(23 - unbiased_exponent); denominator_exp.normalize();
		denominator = varInt::binaryPow(two, denominator_exp);
		denominator_exp.discard();
	}
	two.discard(); SIGN.discard();
	normalize();
}
varFloat::varFloat(double d) {
	union {
		double d;
		unsigned char ch[8];
	} extract = { d };
	int8_t sign = (extract.ch[7] >> 7) == 1 ? -1 : 1;
	varInt SIGN(sign); SIGN.normalize();
	int16_t exponent = (((extract.ch[7] & 0x7f) << 4) | (extract.ch[6] >> 4)) - 1023;
	int64_t mantissa = (((uint64_t)extract.ch[6] & 0x0f) << 48) | ((uint64_t)extract.ch[5] << 40) | ((uint64_t)
		extract.ch[4] << 32)| ((uint64_t)extract.ch[3] << 24) | ((uint64_t)
			extract.ch[2] << 16) | ((uint64_t)extract.ch[1] << 8) | extract.ch[0];
	numerator = varInt(mantissa);
	numerator *= SIGN;
	integer = varInt(0);
	varInt two(2); two.normalize();
	if (exponent == -1023) {
		// subnormal (denormalized);
		varInt exp(1022); exp.normalize();
		denominator = varInt::binaryPow(two, exp);
		exp.discard();
	}
	else {
		varInt two_pow_52((int64_t)std::pow(2, 52)); two_pow_52.normalize();
		two_pow_52 *= SIGN;
		numerator += two_pow_52;
		varInt exp(52 - exponent); exp.normalize();
		denominator = varInt::binaryPow(two, exp);
		exp.discard();
		two_pow_52.discard();
	}

	two.discard(); SIGN.discard();
	normalize();
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
varFloat varFloat::pow(varFloat base, varInt exponent) {
	varFloat result;
	varInt a(exponent.data, exponent.length, exponent.length);
	a.normalize();
	varInt zero(0); zero.normalize();
	if (a == zero) {
		result = fromString("1.0");
	}
	else {
		varFloat b = fullFraction(base);
		
		if (exponent > zero) {
			result.numerator = varInt::binaryPow(b.numerator, a);
			result.denominator = varInt::binaryPow(b.denominator, a);
			result.integer = varInt(0);
		}
		else {
			result.numerator = varInt::binaryPow(b.denominator, a);
			result.denominator = varInt::binaryPow(b.numerator, a);
			result.integer = varInt(0);
		}
		b.discard();
	}
	zero.discard();
	a.discard();
return result;
}
varFloat varFloat::NewtonSquareRoot(varFloat a) {
	if (isNegative(a)) {
		printf("varFloat's NewtonSquareRoot input varFloat cannot be negative!");
		return fromString("0.0");
	}
	varInt zero(0); zero.normalize();
	varInt one(1); one.normalize();
	char* literal = varInt::toString(a.integer);
	varInt exponent((int64_t)strlen(literal)); 
	free(literal);
	exponent.normalize();
	varInt two(2); two.normalize();
	varInt twoPowExponent = varInt::binaryPow(two, exponent);
	varFloat x(twoPowExponent);
	exponent.discard(); twoPowExponent.discard();
	varFloat TWO(two);
	varInt tenPow16((int64_t)std::pow(10, 16)); tenPow16.normalize();
	varFloat epsilon(zero, one, tenPow16); // 1 / 10^16
	tenPow16.discard();
	bool cont = true;
	while(cont) {
		varFloat frac_a_x = a / x, x_plus_frac_a_x = x + frac_a_x;
		varFloat x0 = x;
		x = x_plus_frac_a_x / TWO;
		
		frac_a_x.discard(); x_plus_frac_a_x.discard();
		varFloat diff = x0 - x; varFloat abs_diff = abs(diff);
		if (abs_diff < epsilon) {
			cont = false;
		}
		diff.discard(); abs_diff.discard();
		x0.discard();
	}
	one.discard(); zero.discard(); two.discard(); TWO.discard(); epsilon.discard();
	if (isNegative(x)) {
		varFloat abs_x = abs(x);
		x.discard();
		return abs_x;
	}
	return x;
}
varFloat varFloat::abs(varFloat a) {
	varFloat b = fullFraction(a);
	if (varInt::isNegative(b.numerator)) {
		varInt c = varInt::negate(b.numerator);
		b.numerator.discard();
		b.numerator = c;
	}
	if (varInt::isNegative(b.denominator)) {
		varInt c = varInt::negate(b.denominator);
		b.denominator.discard();
		b.denominator = c;
	}
	b.normalize();
	return b;
}
varFloat varFloat::negate(varFloat a) {
	varFloat result;
	result.integer = varInt::negate(a.integer);
	result.numerator = varInt::negate(a.numerator);
	result.denominator = varInt(a.denominator.data,a.denominator.length,a.denominator.length);
	return result;
}
varFloat varFloat::copy(const varFloat& original) {
	return varFloat(original.integer, original.numerator, original.denominator);
}