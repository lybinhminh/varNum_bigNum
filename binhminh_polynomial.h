#pragma once
#include "vector"
#include "binhminh_varFloat.h"
#ifndef BINHMINH_POLYNOMIAL_H_
#define BINHMINH_POLYNOMIAL_H_

class polynomial_in_x {
public:
	int degree = 0;
	varFloat* coefficients;
	polynomial_in_x( int, varFloat*);
	polynomial_in_x();
	polynomial_in_x(int);
	polynomial_in_x(varFloat); // generate degree 0 polynomial
	polynomial_in_x operator+(polynomial_in_x);
	polynomial_in_x operator-(polynomial_in_x);
	polynomial_in_x operator*(polynomial_in_x);
	polynomial_in_x operator*= (polynomial_in_x);
	polynomial_in_x operator+= (polynomial_in_x);
	polynomial_in_x operator-= (polynomial_in_x);
	polynomial_in_x naiveDivi (polynomial_in_x, polynomial_in_x*);
	static polynomial_in_x copy(const polynomial_in_x&);
	static polynomial_in_x extract(polynomial_in_x, int, int);
	bool operator==(polynomial_in_x);
	
	polynomial_in_x normalize();
	varFloat evaluate(varFloat);
	void discard();
	static polynomial_in_x LagrangeInterpolation(int, varFloat*, varFloat*);

	char* toString(bool,int);
	void printString(int);
};
class barycentric_output;
class fractional_polynomial_in_x {
public:
	std::vector<polynomial_in_x> product_terms;
	std::vector<polynomial_in_x> divisor_terms;
	bool cloned = false;
	varFloat evaluate(varFloat);
	fractional_polynomial_in_x cancel();
	static varFloat evaluateSeries(std::vector<polynomial_in_x>,varFloat);
	fractional_polynomial_in_x evaluateProductTerms(varFloat);
	fractional_polynomial_in_x evaluateDivisorTerms(varFloat);
	void clearProductTerms();
	void clearDivisorTerms();
	polynomial_in_x round();
	fractional_polynomial_in_x operator* (polynomial_in_x);
	fractional_polynomial_in_x operator* (fractional_polynomial_in_x);
	fractional_polynomial_in_x operator*= (polynomial_in_x);
	fractional_polynomial_in_x operator*= (fractional_polynomial_in_x);
	fractional_polynomial_in_x operator/ (polynomial_in_x);
	fractional_polynomial_in_x operator/ (fractional_polynomial_in_x);
	fractional_polynomial_in_x operator/= (polynomial_in_x);
	fractional_polynomial_in_x operator/= (fractional_polynomial_in_x);
	fractional_polynomial_in_x clone();
	static barycentric_output BarycentricLagrangeInterpolation(int, varFloat*, varFloat*);
	char* toString(int);
	void printString(int);
	void discard();

};
class barycentric_output {
public:
	varFloat* x_set;
	varFloat* y_set;
	fractional_polynomial_in_x l0;
	varFloat* yw_ratios;
	void discard();
	varFloat evaluate(varFloat);
};
#endif