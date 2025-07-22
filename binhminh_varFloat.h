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
};
#endif