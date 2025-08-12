#pragma once
#include <cstdlib>
#include "binhminh_varInt.h"
#ifndef BINHMINH_VARFLOAT_H_
#define BINHMINH_VARFLOAT_H_

class varFloat {
public:
	varInt integer, numerator, denominator;
	varFloat(varInt);
	varFloat(varInt, varInt, varInt);
	varFloat();
	varFloat(float);
	varFloat(double);
	void discard();
	varFloat normalize();
	varFloat operator+(varFloat);
	varFloat operator-(varFloat);
	varFloat operator*(varFloat);
	varFloat operator/(varFloat);
	varFloat operator+=(varFloat);
	varFloat operator-=(varFloat);
	varFloat operator*= (varFloat);
	varFloat operator/= (varFloat);
	void cdc(varFloat&, varFloat&);
	static varFloat fullFraction(varFloat);
	static char* toString(varFloat, size_t);
	void printString(size_t);
	static varFloat fromString(const char*);
	static bool isNegative(varFloat);
	bool operator==(varFloat b);
	bool operator>(varFloat b);
	bool operator<(varFloat b);
	bool operator!=(varFloat b);
	static varFloat pow(varFloat,varInt);
	static varFloat NewtonSquareRoot(varFloat);
	static varFloat abs(varFloat);
	static varFloat negate(varFloat);
	static varFloat copy(const varFloat&);
};
#endif