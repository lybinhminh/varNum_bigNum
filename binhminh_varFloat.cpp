#include <cstring>
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
	varInt c = varInt::abs(a.denominator), d = varInt::abs(b.denominator);
	c.normalize(); d.normalize();
	varInt GCD = varInt::gcd(c, d), cd = c * d, LCM = cd.naiveDivi(GCD, NULL);
	varInt aFactor = LCM.naiveDivi(c,NULL), bFactor = LCM.naiveDivi(d, NULL);
	a.numerator *= aFactor;
	a.denominator *= aFactor;
	b.numerator *= bFactor;
	b.denominator *= bFactor;
	c.discard();d.discard();GCD.discard();
	cd.discard();LCM.discard();aFactor.discard();bFactor.discard();
}
varFloat varFloat::normalize() {
	integer.normalize();
	numerator.normalize();
	denominator.normalize();
	varInt remainder(0), quotient = numerator.naiveDivi(denominator, &remainder);
	integer += quotient;
	numerator.discard();
	numerator = remainder;
	varInt GCD = varInt::gcd(numerator, denominator);
	varInt one(1); one.normalize();
	if (GCD != one) {
		numerator /= GCD;
		denominator /= GCD;
	}
	GCD.discard();
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
	e.numerator = c.numerator * d.numerator;
	e.denominator = c.denominator * d.denominator;
	c.discard();
	d.discard();
	e.normalize();
	// arithmetic type: 2 visible multiplication, 2 fullFraction(cheap mult and + of varInt)
	// , 1 varFloat normalize
	return e;
	
}
varFloat varFloat::operator/ (varFloat b) {
	varFloat c = fullFraction(*this), d = fullFraction(b), e;
	e.numerator = c.numerator * d.denominator;
	e.denominator = c.denominator * d.numerator;
	c.discard();
	d.discard();
	e.normalize();
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
	b.normalize();
	bool negativity = varInt::isNegative(b.integer) || varInt::isNegative(b.numerator)
		|| varInt::isNegative(b.denominator);
	if (negativity && !varInt::isNegative(b.integer)) {
		varInt c = varInt::negate(b.integer);
		b.integer.discard();
		b.integer = c;
	}
	char* integerPart = varInt::toString(b.integer);
	varInt zero(0), decimalSum(0);
	zero.normalize(); decimalSum.normalize();
	varInt ten(10); ten.normalize();
	varInt c = varInt::abs(b.numerator), d = varInt::abs(b.denominator);
	b.numerator.discard(); b.numerator = c;
	b.denominator.discard(); b.denominator = d;
	for (int i = 0; a.numerator != zero && i < max_precision; ++i) {
		decimalSum *= ten;
		b.numerator *= ten;
		varInt remainder(0), quotient = b.numerator.naiveDivi(b.denominator, &remainder);
		decimalSum += quotient;
		quotient.discard();
		b.numerator.discard();
		b.numerator = remainder;
	}
	b.discard();
	zero.discard(); ten.discard();
	char* decimalPart = varInt::toString(decimalSum);
	char* result = (char*)malloc(strlen(integerPart) + strlen(decimalPart) + 2);
	memcpy(result, integerPart, strlen(integerPart));
	result[strlen(integerPart)] = '.';
	memcpy(result + strlen(integerPart)+1, decimalPart, strlen(decimalPart));
	result[strlen(integerPart) + strlen(decimalPart) + 1] = '\0';
	return result;
}
varFloat varFloat::operator+=(varFloat b) {
	varFloat sum = *this + b;
	discard();
	integer = sum.integer;
	numerator = sum.numerator;
	denominator = sum.denominator;
	return *this;
}
varFloat varFloat::operator-=(varFloat b) {
	varFloat sum = *this + b;
	discard();
	integer = sum.integer;
	numerator = sum.numerator;
	denominator = sum.denominator;
	return *this;
}
varFloat varFloat::operator*=(varFloat b) {
	varFloat sum = *this * b;
	discard();
	integer = sum.integer;
	numerator = sum.numerator;
	denominator = sum.denominator;
	return *this;
}
varFloat varFloat::operator/=(varFloat b) {
	varFloat sum = *this / b;
	discard();
	integer = sum.integer;
	numerator = sum.numerator;
	denominator = sum.denominator;
	return *this;
}
void varFloat::printString(size_t precision) {
	char* literal = toString(*this, precision);
	printf("%s\n", literal);
	free(literal);
}