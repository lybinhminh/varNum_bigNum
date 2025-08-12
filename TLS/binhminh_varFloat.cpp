#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <cstdio>
#include <cmath>
#include <chrono>
#include "binhminh_varFloat.h"

void varFloat::discard() {
	integer.discard();
	numerator.discard();
	denominator.discard();
}
varFloat::varFloat(varInt a) {
	integer = a;
	numerator = varInt(0);
	denominator = varInt(1);
}
varFloat::varFloat(varInt a, varInt b, varInt c) {
	integer = a;
	numerator = b;
	denominator = c;
}
void varFloat::cdc(varFloat& a, varFloat& b) {
	if (a.denominator == b.denominator)return; // cdc-ed
	varInt GCD = varInt::gcd(a.denominator, b.denominator), ab = a.denominator * b.denominator;
	varInt LCM = ab.abs().naiveDivi(GCD, NULL); GCD.discard();
	varInt aFactor = LCM.naiveDivi(a.denominator,NULL), bFactor = LCM.naiveDivi(b.denominator, NULL);
	a.numerator *= aFactor;
	a.denominator *= aFactor;
	b.numerator *= bFactor;
	b.denominator *= bFactor;
	LCM.discard();aFactor.discard();bFactor.discard();
}
varFloat& varFloat::normalize() {
	numerator.normalize();
	denominator.normalize();
	varInt remainder, quotient = numerator.naiveDivi(denominator, &remainder);
	integer += quotient;
	quotient.discard();
	numerator.discard();
	numerator = remainder;
	bool intNegativity = varInt::isNegative(integer),
		numeratorNegativity = varInt::isNegative(numerator),
		denominatorNegativity = varInt::isNegative(denominator);
	if (numerator == varIntConst::ZERO) {
		denominator.discard();denominator = varInt(1);
	}
	else if (denominatorNegativity) {
		denominator.negate(); numerator.negate();
	}
	numeratorNegativity = varInt::isNegative(numerator);
	denominatorNegativity = varInt::isNegative(denominator);
	// sign unify
	// e.g -int + +fraction -> -int + -fraction ; +int + -fraction -> +int + +fraction
	integer.normalize();
	if (integer != varIntConst::ZERO && numerator != varIntConst::ZERO) {

		if (intNegativity && !numeratorNegativity) {
			integer += varIntConst::ONE;
			numerator -= denominator;
		}
		if (!intNegativity && numeratorNegativity) {
			integer -= varIntConst::ONE;
			numerator += denominator;
		}
	}
	integer.normalize();
	numerator.normalize();
	if (numerator != varIntConst::ZERO) {
		varInt GCD = varInt::gcd(numerator, denominator);
		GCD.normalize();
		if (GCD != varIntConst::ONE) {
			numerator /= GCD;
			denominator /= GCD;
		}
		GCD.discard();
	}
	return *this;
}
varFloat::varFloat() {}
varFloat varFloat::operator+(varFloat& b) {
	varFloat sum;
	sum.integer = integer + b.integer;
	sum.integer.normalize();
	cdc(*this, b);
	sum.numerator = numerator + b.numerator;
	sum.numerator.normalize();
	sum.denominator = varInt(denominator.data,denominator.length,denominator.length);
	return sum;
}
varFloat varFloat::operator-(varFloat& b) {
	varFloat diff;
	diff.integer = integer - b.integer;
	diff.integer.normalize();
	cdc(*this, b);
	diff.numerator = numerator - b.numerator;
	diff.numerator.normalize();
	diff.denominator = varInt(denominator.data, denominator.length, denominator.length);
	return diff;
}
varFloat varFloat::operator*(varFloat b) {
	varFloat c = fullFraction(*this), d = fullFraction(b), e;
	e.integer = 0;
	e.numerator = c.numerator * d.numerator;
	e.denominator = c.denominator * d.denominator;
	c.discard();
	d.discard();
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
varFloat varFloat::fullFraction(varFloat& a) {
	varInt b = a.integer * a.denominator;
	b.normalize();
	varFloat result;
	result.integer = 0;
	result.numerator = b + a.numerator; b.discard();
	result.numerator.normalize();
	result.denominator = varInt(a.denominator.data, a.denominator.length, a.denominator.length);
	return result;
}
char* varFloat::toString(varFloat& a, size_t max_precision) {
	a.normalize();
	bool neg = false;
	if (a.integer == varIntConst::ZERO) {
		neg = varInt::isNegative(a.numerator);
	}
	varInt temp = varInt::abs(a.numerator), decimalSum(0);
	int leadingZero = 0; bool cLeadingZero = true;
	char* integerPart = varInt::toString(a.integer);
	for (int i = 0; i < max_precision && temp > varIntConst::ZERO; ++i) {
		temp *= varIntConst::TEN;
		varInt r, q = temp.naiveDivi(a.denominator, &r);

		if(q == varIntConst::ZERO && cLeadingZero)
		{
			leadingZero++;
		}
		else
		{
			decimalSum *= varIntConst::TEN;
			decimalSum += q;
			if(cLeadingZero)
			cLeadingZero = false;
		}
		q.discard(); temp.discard(); temp = r;
	}
	temp.discard();
	char* decimalPart = varInt::toString(decimalSum);
	char* str = (char*)malloc((neg ? 1 : 0) + strlen(integerPart) + 1 + leadingZero + strlen(decimalPart)
		+ 1);
	int i = 0;
	if (neg) {
		str[0] = '-';
		i++;
	}
	memcpy(str + i, integerPart, strlen(integerPart)); i += strlen(integerPart); free(integerPart);
	str[i++] = '.';
	if (leadingZero > 0) {
		memset(str + i, 48, leadingZero); i += leadingZero;
	}
	memcpy(str + i, decimalPart, strlen(decimalPart)); i += strlen(decimalPart); free(decimalPart);
	str[i] = NULL;
	return str;
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
		result.denominator = varInt::binaryPow(varIntConst::TEN, strlen(decimalPart));
	}
	else {
		result.numerator = 0;
		result.denominator = 1;
	}
	free(copy);
	return result;
}
varFloat& varFloat::operator+= (varFloat& b) {
	integer += b.integer;
	cdc(*this, b);
	numerator += b.numerator;
	return *this;
}
varFloat& varFloat::operator-= (varFloat& b) {
	integer -= b.integer;
	cdc(*this, b);
	numerator -= b.numerator;
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
	bool sign = extract.b[3] >> 7;
	uint8_t biased_exponent = extract.b[3] << 1 | extract.b[2] >> 7;
	int8_t unbiased_exponent = biased_exponent - 127;
	int32_t mantissa = ((int32_t)(extract.b[2] & 0x7f)) << 16 | (int32_t)extract.b[1] << 8 | (int32_t)extract.b[0];
	numerator = varInt(mantissa);
	integer = varInt(0);
	if (biased_exponent == 0 && mantissa != 0) {
		denominator = 1;
		denominator <<= 149;
	}
	else {
		denominator = 1;
		if (unbiased_exponent < 23) denominator <<= 23 - unbiased_exponent;
		varInt twoPow23 = 1; twoPow23 <<= 23;
		numerator += twoPow23;
		twoPow23.discard();
		if (unbiased_exponent > 23)
			numerator <<= unbiased_exponent - 23;
	}
	if (sign)numerator.negate();
	printf("Hi\n");
	normalize();
}
varFloat::varFloat(double d) {
	union {
		double d;
		unsigned char ch[8];
	} extract = { d };
	bool sign = extract.ch[7] >> 7;
	varInt SIGN(sign); SIGN.normalize();
	int16_t exponent = (((extract.ch[7] & 0x7f) << 4) | (extract.ch[6] >> 4)) - 1023;
	int64_t mantissa = (((uint64_t)extract.ch[6] & 0x0f) << 48) | ((uint64_t)extract.ch[5] << 40) | ((uint64_t)
		extract.ch[4] << 32)| ((uint64_t)extract.ch[3] << 24) | ((uint64_t)
			extract.ch[2] << 16) | ((uint64_t)extract.ch[1] << 8) | extract.ch[0];
	numerator = varInt(mantissa);
	integer = 0;
	if (exponent == -1023) {
		// subnormal (denormalized);
		denominator = 1;
		denominator << 1074;
	}
	else {
		denominator = 1;
		if (exponent < 52)denominator <<= 52 - exponent;
		varInt twoPow52 = 1; twoPow52 <<= 52;
		numerator += twoPow52; twoPow52.discard();
		if (exponent > 52)numerator <<= exponent - 52;
	}
	if (sign)numerator.negate();
	normalize();
}
bool varFloat::isNegative(varFloat a) {
	if (a.integer == varIntConst::ZERO) {
		return varInt::isNegative(a.numerator);
	}
	return varInt::isNegative(a.integer);
}
bool varFloat::operator==( varFloat& b){
	cdc(*this, b);
	varInt diffInt = integer - b.integer, diffN = numerator - b.numerator;
	diffInt *= denominator;
	diffInt.normalize(); diffN.normalize();
	varInt temp = diffInt + diffN; temp.normalize();
	bool result = temp == varIntConst::ZERO;
	diffInt.discard(); diffN.discard(); temp.discard();
	return result;
}
bool varFloat::operator!=(varFloat& b) {
	return !(*this == b);
}
bool varFloat::operator>(varFloat& b) {
	bool neg = isNegative(*this), 
		b_neg = isNegative(b);
	if (!neg && b_neg)return true;
	else if (neg && !b_neg)return false;
	this->fullFraction(); b.fullFraction();
	cdc(*this, b);
	bool e = isNegative(denominator), f = isNegative(b.denominator);
	if(e != f)
	{
		if (e) {
			denominator.negate();
			numerator.negate();
		}
		if (f) {
			b.denominator.negate();
			b.numerator.negate();
		}
	}
	return numerator > b.numerator;
}
bool varFloat::operator< (varFloat& b) {
	bool neg = isNegative(*this),
		b_neg = isNegative(b);
	if (!neg && b_neg)return true;
	else if (neg && !b_neg)return false;
	this->fullFraction(); b.fullFraction();
	cdc(*this, b);
	bool e = isNegative(denominator), f = isNegative(b.denominator);
	if (e != f)
	{
		if (e) {
			denominator.negate();
			numerator.negate();
		}
		if (f) {
			b.denominator.negate();
			b.numerator.negate();
		}
	}
	return numerator < b.numerator;
}
varFloat varFloat::pow(varFloat v, int exp) {
	if (exp == 0)return varFloat(varInt(1));
	varFloat result; result.integer = 0;
	if (exp < 0) {

		result.numerator = varInt::binaryPow(v.denominator, exp * -1);
		result.denominator = varInt::binaryPow(v.numerator, exp * -1);
	}
	else {
		result.numerator = varInt::binaryPow(v.numerator, exp);
		result.denominator = varInt::binaryPow(v.denominator, exp);
	}
	return result;
}
varFloat varFloat::pow(varFloat v, int exp, int root) {
	varFloat power;
	if (exp == 0)power = varFloat(varInt(1));
	power = pow(v, exp);
	return power;
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
	
	tenPow16.discard();
	bool cont = true;
	while(cont) {
		varFloat frac_a_x = a / x, x_plus_frac_a_x = x + frac_a_x;
		varFloat x0 = x;
		x = x_plus_frac_a_x / TWO;
		
		frac_a_x.discard(); x_plus_frac_a_x.discard();
		varFloat diff = x0 - x; varFloat abs_diff = abs(diff);
		if (abs_diff < varFloatConst::EPSILON) {
			cont = false;
		}
		diff.discard(); abs_diff.discard();
		x0.discard();
	}
	one.discard(); zero.discard(); two.discard(); TWO.discard();
	if (isNegative(x)) {
		varFloat abs_x = abs(x);
		x.discard();
		return abs_x;
	}
	return x;
}
varFloat varFloat::abs(varFloat a) {
	bool neg = isNegative(a);
	if (!neg)return copy(a);
	else return negate(a);
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
varFloat varFloat::_reciprocal(varFloat& D) {
	
	varFloat X; // 0 microsec
	//std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
	if (D.integer.length == 1 && D.integer.data[0] == 0) {
		varInt r, base;
		X.integer = D.denominator.naiveDivi(D.numerator, &r);
		X.denominator = varInt(10);
		r *= X.denominator;
		X.numerator = r.naiveDivi(D.numerator, &r);
		X.numerator *= X.denominator;
		r *= X.denominator;
		X.denominator *= X.denominator;
		X.numerator += r.naiveDivi(D.numerator, NULL);
		r.discard();
	}
	else {
	
	D.fullFraction(); // 6 - 9 microsec
	
		varInt a = D.denominator * varIntConst::TEN,r; // 6 - 9 microsec
		X.numerator = a.naiveDivi(D.numerator, &r); //48 - 50ms
		a.discard();
	
		X.denominator = varInt(varIntConst::TEN .data, 1, 1); // 1,1 - 1,2ms
		

	X.numerator *= varIntConst::TEN; //4,8 - 9,8microsec
	r *= varIntConst::TEN; // 4.8 - 8.4microsec
	//total : 16microsec  - 50microsec

		X.numerator += r.naiveDivi(D.numerator, &r); // 51 - 61microsec

		r *= varIntConst::TEN;
		X.numerator *= varIntConst::TEN;
		X.denominator *= varIntConst::TEN;
	
		X.numerator += r.naiveDivi(D.numerator, NULL); // 50micro - 124 microsec

	X.integer = varInt(varIntConst::ZERO.data, 1, 1);
		r.discard();
		
	}// total 500microsec - 144microsec
	/*
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	printf("time = %lld\n", std::chrono::duration_cast<std::chrono::nanoseconds>(end - start));
	*/

	return X;
}
varFloat varFloat::NewtonRaphsonDivision( varFloat& D) {
	varFloat reciprocal = _reciprocal(D);
	return *this * reciprocal;
}
varFloat& varFloat::abs() {
	bool neg = isNegative(*this);
	if(neg){
		integer.negate();
		numerator.negate();
	}
	return *this;
}
varFloat varFloat::floor(varFloat v, size_t precision) {
	varInt base = varInt::binaryPow(varIntConst::TEN, precision);
	varInt numerator = v.numerator * base;
	varInt newNumerator = numerator.naiveDivi(v.denominator, NULL);
	numerator.discard();
	varFloat rounded;
	rounded.integer = varInt(v.integer.data, v.integer.length, v.integer.length);
	rounded.numerator = newNumerator;
	rounded.denominator = base;
	return rounded;
}
varFloat& varFloat::floor(size_t precision) {
	varInt base = varInt::binaryPow(varIntConst::TEN, precision);
	numerator *= base; varInt newNumerator = numerator.naiveDivi(denominator, NULL);
	numerator.discard(); numerator = newNumerator;
	denominator.discard();  denominator = base;
	return *this;
}
varFloat varFloat::round(varFloat v, size_t precision) {
	varInt base = varInt::binaryPow(varIntConst::TEN, precision);
	varInt halfDen = v.denominator >> 1;
	varInt numerator = v.numerator * base;
	numerator += halfDen; halfDen.discard();
	varInt newNumerator = numerator.naiveDivi(v.denominator, NULL);
	numerator.discard();
	varFloat rounded;
	rounded.integer = varInt(v.integer.data, v.integer.length, v.integer.length);
	rounded.numerator = newNumerator;
	rounded.denominator = base;
	return rounded;
}
varFloat& varFloat::round(size_t precision) {
	varInt base = varInt::binaryPow(varIntConst::TEN, precision);
	numerator *= base; varInt halfDen = denominator >> 1;
	numerator += halfDen;varInt newNumerator = numerator.naiveDivi(denominator, NULL);
	numerator.discard(); numerator = newNumerator;
	denominator.discard();  denominator = base;
	return *this;
}
varFloat& varFloat::fullFraction() {
	if (integer == varIntConst::ZERO)return *this;
	varInt temp = integer * denominator;
	numerator += temp; temp.discard();
	numerator.normalize();
	integer.discard(); integer = 0;
	return *this;
}