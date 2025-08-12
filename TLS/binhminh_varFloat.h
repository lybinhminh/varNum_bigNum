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
	varFloat& normalize();
	varFloat operator+(varFloat&);
	varFloat operator-(varFloat&);
	varFloat operator*(varFloat);
	varFloat operator/(varFloat);
	varFloat& operator+=(varFloat&);
	varFloat& operator-=(varFloat&);
	varFloat operator*= (varFloat);
	varFloat operator/= (varFloat);
	void cdc(varFloat&, varFloat&);
	static varFloat fullFraction(varFloat&);
	static char* toString(varFloat&, size_t);
	void printString(size_t);
	static varFloat fromString(const char*);
	static bool isNegative(varFloat);
	bool operator==(varFloat& b);
	bool operator>(varFloat& b);
	bool operator<(varFloat& b);
	bool operator!=(varFloat& b);
	static varFloat pow(varFloat,int);
	static varFloat pow(varFloat, int,int);
	static varFloat NewtonSquareRoot(varFloat);
	static varFloat abs(varFloat);
	static varFloat negate(varFloat);
	static varFloat copy(const varFloat&);
	static varFloat _reciprocal(varFloat&);
	varFloat NewtonRaphsonDivision( varFloat&);
	varFloat& abs();
	static varFloat floor(varFloat,size_t);
	varFloat& floor(size_t);
	static varFloat round(varFloat, size_t);
	varFloat& round(size_t);
	varFloat& fullFraction();
};
namespace varFloatConst {
	extern varFloat EPSILON;
	extern varFloat ZERO;
	extern varFloat ONE;
	extern varFloat TWO;
	extern varFloat TEN;
}
#endif