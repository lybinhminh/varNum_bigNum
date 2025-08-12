#define _CRT_SECURE_NO_WARNINGS
#include <iterator>
#include "binhminh_polynomial.h"
polynomial_in_x::polynomial_in_x( int DEGREE, varFloat* COEFFICIENTS) {
	degree = DEGREE;
	coefficients = (varFloat*)calloc(degree + 1, sizeof(varFloat));
	for (int i = 0; i <= degree; ++i) {
		
		//COEFFICIENTS[i].denominator.printHex();
		coefficients[degree - i] = varFloat(COEFFICIENTS[i].integer,COEFFICIENTS[i].numerator,
			COEFFICIENTS[i].denominator);
	}
}
polynomial_in_x::polynomial_in_x() {}
polynomial_in_x polynomial_in_x::operator+(polynomial_in_x b) {

	polynomial_in_x result;
	result.degree = std::max(degree, b.degree);
	varFloat zero = 0.0;
	result.coefficients = (varFloat*)calloc(result.degree + 1, sizeof(varFloat));

	for (int i = result.degree; i >= 0; --i) {
	
		varFloat c = i > degree ? zero : coefficients[i], d
			= i > b.degree ? zero : b.coefficients[i];

		result.coefficients [i] = c + d;
	
	}
	
	zero.discard();

	return result;
}
polynomial_in_x polynomial_in_x::operator-(polynomial_in_x b) {
	polynomial_in_x result;
	result.degree = std::max(degree, b.degree);
	varFloat zero = 0.0;
	result.coefficients = (varFloat*)calloc(result.degree + 1, sizeof(varFloat));
	for (int i = result.degree; i >= 0; --i) {
		varFloat c = i > degree ? zero : coefficients[i], d
			= i > b.degree ? zero : b.coefficients[i];
		result.coefficients[i] = c - d;
	}
	//result.normalize();
	zero.discard();
	return result;
}
polynomial_in_x polynomial_in_x::operator*(polynomial_in_x b) {
	polynomial_in_x result;
	result.degree = degree + b.degree;
	result.coefficients = (varFloat*)calloc(result.degree + 1, sizeof(varFloat));
	for (int i = 0; i < result.degree + 1; ++i) {
		result.coefficients[i] = 0.0;
	}
	for (int deg1 = degree; deg1 >= 0; --deg1) {
		for (int deg2 = b.degree; deg2 >= 0; --deg2) {
			varFloat prod = coefficients[deg1] * b.coefficients[deg2];
			result.coefficients[deg1 + deg2] += prod;
			prod.discard();
		}
	}
	//result.normalize();

	return result;
}
varFloat polynomial_in_x::evaluate(varFloat x) {
	varFloat result = 0.0;
	for (int i = degree; i >= 0; --i) {
		varInt DEGREE(i); DEGREE.normalize();
		
		varFloat poweredX = varFloat::pow(x, DEGREE);
		
		varFloat monomial = coefficients[i] * poweredX;
	
		
		result += monomial;
		monomial.discard();
		DEGREE.discard();
	}
	return result;
}
polynomial_in_x polynomial_in_x::normalize() {
	varFloat zero = 0.0;
	for (int i = degree; i > 0; --i) {
		varFloat coefficient = coefficients[i];
		if (coefficient == zero) {
			degree--;
			coefficient.discard();
			coefficients = (varFloat*)realloc(coefficients, sizeof(varFloat) * (degree + 1));
		}
		else
			break;
	}
	zero.discard();
	return *this;
}
void polynomial_in_x::discard() {
	if(degree > -1)
	{
		for (int i = degree; i >= 0; --i) {
			coefficients[i].discard();
		}
		degree = -1;
	}
}
polynomial_in_x polynomial_in_x::operator*=(polynomial_in_x b) {
	polynomial_in_x result = *this * b;
	discard();
	degree = result.degree;
	coefficients = result.coefficients;
	return *this;
}
polynomial_in_x polynomial_in_x::operator+= (polynomial_in_x b) {
	polynomial_in_x result = *this + b;
	discard();
	degree = result.degree;
	coefficients = result.coefficients;
	return *this;
}
polynomial_in_x polynomial_in_x::LagrangeInterpolation(int size, varFloat* set_x, varFloat* set_y) {
	varFloat zero = 0.0, one = 1.0;
	polynomial_in_x L(0, &zero);
	varFloat* weights = (varFloat*)calloc(size, sizeof(varFloat));
	polynomial_in_x *x_minus_xm_collection = (polynomial_in_x*)calloc(size,sizeof(polynomial_in_x));
	for (int i = 0; i < size; ++i)weights[i] = 1.0;
	for (int i = 0; i < size; ++i) {
		varFloat x_sub_i = set_x[i], neg_x_sub_i = varFloat::negate(x_sub_i),y_sub_i = set_y[i];
		varFloat a[] = {one, neg_x_sub_i};
		polynomial_in_x b(1, a);
		x_minus_xm_collection[i] = b;
		for (int j = 0; j < size; ++j) {
			if (i == j)continue;
			varFloat x_sub_j = set_x[j];
			varFloat diff = x_sub_i - x_sub_j;
			weights[i] *= diff;
			diff.discard();
		}
		weights[i] /= y_sub_i;
		neg_x_sub_i.discard();
	}
	for (int i = 0; i < size; ++i) {
		varFloat x_sub_i = set_x[i];
		polynomial_in_x a(one);
		for (int j = 0; j < size; ++j) {
			if (i == j)continue;
			a *= x_minus_xm_collection[j];
			
		}
		a.normalize();
		for (int j = a.degree; j >= 0; --j) {
			a.coefficients[j] /= weights[i];
		}
		L += a;
		weights[i].discard();
		a.discard();
	}
	for (int i = 0; i < size; ++i) {
		x_minus_xm_collection[i].discard();
	}
	free(weights); free(x_minus_xm_collection);
	zero.discard(); one.discard();
	return L;
}
char* polynomial_in_x::toString(bool signAdjust,int maxPrecision) {
	char* str = (char*)malloc(65535);
	int index = 0;
	varFloat one = 1.0, zero = 0.0, minus_one = varFloat::fromString("-1.0");
	bool firstVisibleTerm = true;
	for (int i = degree; i >= 0;--i) {
		
		varFloat coefficient = coefficients[i];
		if(coefficient != zero || (coefficient == zero && i == 0))
		{
			bool negativity = varFloat::isNegative(coefficient);
		
			if (!(i > 0 && (coefficient == one || coefficient == minus_one)))
			{
				if (negativity && !signAdjust) {
					memset(str + index, 40, 1);
					index++;
				}
				if (signAdjust && i != degree)
				{
					varFloat abs_coefficient = varFloat::abs(coefficient);
					char* lit_coefficient = coefficient.toString(abs_coefficient,maxPrecision);
					memcpy(str + index, lit_coefficient, strlen(lit_coefficient));
					index += strlen(lit_coefficient);
					free(lit_coefficient);
					abs_coefficient.discard();
				}
				else {
					char* lit_coefficient = coefficient.toString(coefficient, maxPrecision);
					memcpy(str + index, lit_coefficient, strlen(lit_coefficient));
					index += strlen(lit_coefficient);
					free(lit_coefficient);
				}
			}
			if (i > 0 && firstVisibleTerm && coefficient == minus_one) {
				if (i == 1) {
					char x[] = "-x";
					memcpy(str + index, x, strlen(x));
					index += strlen(x);
				}
				else {
					char xpow[] = "-x^";
					char lit_deg[3];
					_itoa(i, lit_deg, 10);
					memcpy(str + index, xpow, strlen(xpow));
					index += strlen(xpow);
					memcpy(str + index, lit_deg, strlen(lit_deg));
					index += strlen(lit_deg);
				}
			}
			else if (i > 1)
			{
				char xpow[] = "x^";
				char lit_deg[3];
				_itoa(i, lit_deg, 10);
				memcpy(str + index, xpow, strlen(xpow));
				index += strlen(xpow);
				memcpy(str + index, lit_deg, strlen(lit_deg));
				index += strlen(lit_deg);
			}
			else if (i == 1) {
				char x[] = "x";
				memcpy(str + index, x, strlen(x));
				index++;
			}
			if (negativity && !signAdjust) {
				memset(str + index, 41, 1);
				index++;
			}
			bool next_negativity = false;
			for (; i > 0;) {
				varFloat next_coefficient = coefficients[--i];
				if (next_coefficient != zero) {
					++i;
					next_negativity = varFloat::isNegative(next_coefficient);
					break;
				}
			}
			if (i > 0) {
				if (!next_negativity || !signAdjust)
				{

					memset(str + index, 32, 1);
					index++;
					memset(str + index, 43, 1);
					index++;
					memset(str + index, 32, 1);
					index++;
				}

				else {
					memset(str + index, 32, 1);
					index++;
					memset(str + index, 45, 1);
					index++;
					memset(str + index, 32, 1);
					index++;
				}
			}
			firstVisibleTerm = false;
		}
	}
	one.discard();  zero.discard(); minus_one.discard();
	str[index] = '\0';
	str = (char*)realloc(str, index + 1);
	return str;
}
void polynomial_in_x::printString(int maxPrecision) {
	char prefix[] = "f(x) = ";
	char* lit = toString(true,maxPrecision);
	char str[65535];
	memcpy(str, prefix,strlen(prefix));
	memcpy(str + strlen(prefix), lit,strlen(lit) + 1);
	printf("%s\n", str);
	free(lit);
}
polynomial_in_x polynomial_in_x::copy(const polynomial_in_x& original) {
	polynomial_in_x result;
	result.degree = original.degree;
	result.coefficients = (varFloat*)calloc(result.degree + 1, sizeof(varFloat));
	for (int i = result.degree; i >= 0; --i) {
		varFloat v = varFloat::copy( original.coefficients[i] );
		result.coefficients[i] = v;
	}
	return result;
}
bool polynomial_in_x::operator== (polynomial_in_x b) {
	normalize();
	polynomial_in_x b_copy = copy(b);
	b_copy.normalize();
	if (degree != b_copy.degree) {
		b_copy.discard();
		return false;
	}
	bool match = true;
	for (int i = degree; i >= 0; --i) {
		varFloat aCoe = coefficients[i], bCoe = b_copy.coefficients[i];
		if (aCoe != bCoe) {
			match = false;
			break;
		}
	}
	b_copy.discard();
	return match;
}
polynomial_in_x polynomial_in_x::naiveDivi(polynomial_in_x b, polynomial_in_x* remainderPtr) {
	polynomial_in_x a_copy = copy(*this), b_copy = copy(b);
		varFloat zero[] = { 0.0 };
	a_copy.normalize(); b_copy.normalize();
	if (a_copy.degree < b_copy.degree) {

		polynomial_in_x quotient(0, zero);
		
		b_copy.discard();
		if (remainderPtr != NULL) {
			remainderPtr->discard();
			*remainderPtr = a_copy;
		}
		else
			a_copy.discard();
		zero[0].discard();
		return quotient;
	}
	polynomial_in_x remainder = extract(a_copy, a_copy.degree, a_copy.degree - b_copy.degree-1);
	polynomial_in_x quotient(a_copy.degree);
	quotient.coefficients[quotient.degree].discard();
	quotient.coefficients[quotient.degree] = 0.0;
	int i = a_copy.degree - b_copy.degree - 1;
	while (remainder.degree >= b_copy.degree && i > -2) {
		polynomial_in_x po(remainder.degree - b_copy.degree);
		po.coefficients[po.degree] = remainder.coefficients[remainder.degree] / b_copy.coefficients[b_copy.degree];
		quotient.coefficients[po.degree] = varFloat::copy(po.coefficients[po.degree]);
		polynomial_in_x intermediate = b_copy * po;
		remainder -= intermediate;
		intermediate.discard();
		
		if (i > -1) {
			remainder.coefficients[i] = varFloat::copy(a_copy.coefficients[i]);
		}
		remainder.normalize();
		--i;
	}
	for (; i > -1; --i) {
		remainder.coefficients[i] = varFloat::copy(a_copy.coefficients[i]);
	}
	if (remainderPtr != NULL) {
		remainderPtr->discard();
		*remainderPtr = remainder;
	}
	return quotient;
}
polynomial_in_x fractional_polynomial_in_x::round() {
	cancel();
	varFloat one[] = { 1.0 };
	polynomial_in_x product(0, one), divisor(0, one);
	for (polynomial_in_x term : product_terms)product *= term;
	for (polynomial_in_x term : divisor_terms)divisor *= term;
	polynomial_in_x quotient = product.naiveDivi(divisor, NULL);
	one[0].discard();
	product.discard();
	divisor.discard();
	return quotient;
}
polynomial_in_x::polynomial_in_x(int DEGREE) {
	// generate x^degree
	degree = DEGREE;
	coefficients = (varFloat*)calloc(degree + 1, sizeof(varFloat));
	for (int i = 0; i < degree; ++i) {
		coefficients[i] = 0.0;
	}
	coefficients[degree] = 1.0;
}
polynomial_in_x polynomial_in_x::extract(polynomial_in_x poly, int biggerDegree, int smallerDegree) {
	int i = std::min(poly.degree, biggerDegree), j = std::max(-1, smallerDegree);
	polynomial_in_x result(i);
	result.coefficients[result.degree].discard();
	result.coefficients[result.degree] = 0.0;
	for (; i > j; --i) {
		varFloat coefficient = poly.coefficients[i];
		result.coefficients[i] = varFloat(coefficient.integer, coefficient.numerator, coefficient.denominator);
	}
	return result;
}
polynomial_in_x polynomial_in_x::operator-= (polynomial_in_x b) {
	polynomial_in_x result = *this - b;
	discard();
	degree = result.degree;
	coefficients = result.coefficients;
	return *this;
}
char* fractional_polynomial_in_x::toString(int maxPrecision) {
	char str1[16138], str2[16138];
	int i = 0, j = 0;
	bool pBracket = product_terms.size() > 1, dBracket = divisor_terms.size() > 1;
	for (polynomial_in_x pterm : product_terms) {
		char* lit = pterm.toString(true,maxPrecision);
		if(pBracket)
		memset(str1 + i++, 40, 1);
		memcpy(str1 + i, lit, strlen(lit)); i += strlen(lit);
		if (pBracket)
		memset(str1 + i++, 41, 1);
		free(lit);
	}
	str1[i] = 0;
	for (polynomial_in_x dterm : divisor_terms) {
		char* lit = dterm.toString(true,maxPrecision);
		if(dBracket)
		memset(str2 + j++, 40, 1);
		memcpy(str2 + j, lit, strlen(lit)); j += strlen(lit);
		if(dBracket)
		memset(str2 + j++, 41, 1);
		free(lit);
	}
	str2[j] = 0;
	int a = i - j < 0 ? (j - i) / 2 : 0, b = j - i < 0 ? (i - j) / 2 : 0;
	char* str3 = (char*)malloc(65535);
	int x = 0;
	if (a > 0) {
		memset(str3, 32, a);
		x += a;
	}
	memcpy(str3 + x, str1, i); x += i;
	if (a > 0) {
		memset(str3 + x, 32, j - i - a);
		x += j - i - a;
	}
	if (j > 0){
		memset(str3 + x++, 10, 1);
		memset(str3 + x, 45, std::max(i, j)); x += std::max(i, j);
		memset(str3 + x++, 10, 1);
		if (b > 0) {
			memset(str3 + x, 32, b);
			x += b;
		}
		memcpy(str3 + x, str2, j); x += j;
		if (b > 0) {
			memset(str3 + x, 32, i - j - b);
			x += i - j - b;
		}
		memset(str3 + x++, 10, 1);
		
	}
	str3[x++] = 0;
	str3 = (char*)realloc(str3, x);
	return str3;
}
void fractional_polynomial_in_x::printString(int maxPrecision) {
	char* lit = toString(maxPrecision);
	printf("%s\n", lit);
	free(lit);
}
fractional_polynomial_in_x fractional_polynomial_in_x::clone() {
	fractional_polynomial_in_x result;
	for (int i = 0; i < product_terms.size(); ++i) {
		polynomial_in_x term = polynomial_in_x::copy(product_terms[i]);
		result.product_terms.push_back(term);
	}
	for (int i = 0; i < divisor_terms.size(); ++i) {
		polynomial_in_x term = polynomial_in_x::copy(divisor_terms[i]);
		result.divisor_terms.push_back(term);
	}
	result.cloned = true;
	return result;
}
void fractional_polynomial_in_x::discard() {
	for (polynomial_in_x term : product_terms)term.discard();
	for (polynomial_in_x term : divisor_terms)term.discard();
}
barycentric_output fractional_polynomial_in_x::BarycentricLagrangeInterpolation(int size, varFloat* x_set, varFloat* y_set) {
	barycentric_output L;
	L.yw_ratios = (varFloat*)calloc(size-1, sizeof(varFloat));
	L.x_set = (varFloat*)calloc(size, sizeof(varFloat));
	L.y_set = (varFloat*)calloc(size, sizeof(varFloat));
	varFloat one = 1.0;
	polynomial_in_x polynomial_one(one);
	varFloat* weights = (varFloat*)calloc(size, sizeof(varFloat));
	for (int i = 0; i < size; ++i)weights[i] = 1.0;
	for (int i = 0; i < size; ++i) {
		varFloat x_sub_i = x_set[i], neg_x_sub_i = varFloat::negate(x_sub_i);
		L.x_set[i] = varFloat::copy(x_sub_i);
		L.y_set[i] = varFloat::copy(y_set[i]);
		varFloat a[] = { one,neg_x_sub_i };
		polynomial_in_x b(1, a);
		if(i > 0)
			L.l0.product_terms.push_back(b);
		neg_x_sub_i.discard();
		for (int j = 0; j < size; ++j) {
			if (i == j)continue;
			weights[i] *= x_sub_i - x_set[j];
		}
	}

	for (int i = 0; i < size; ++i) {
		if (i == 0) {
			varFloat b = weights[0] / y_set[i];
			polynomial_in_x c(b); b.discard();
			L.l0.divisor_terms.push_back(c);
		}
		else {
			varFloat b = weights[0] / weights[i];
			b *= y_set[i]; b /= y_set[0];
			L.yw_ratios[i - 1] = b;
			
		}
	}
	for (int i = 0; i < size; ++i) {
		weights[i].discard();
	}
	free(weights);
	polynomial_one.discard(); one.discard();
	return L;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator* (fractional_polynomial_in_x b) {
	fractional_polynomial_in_x result;
	fractional_polynomial_in_x a_copy = (*this).clone(), b_copy = b.clone();
	result.product_terms.reserve(a_copy.product_terms.size() + b_copy.product_terms.size());
	result.divisor_terms.reserve(a_copy.divisor_terms.size() + b_copy.divisor_terms.size());
	std::copy(a_copy.product_terms.begin(), a_copy.product_terms.end(), std::back_inserter(result.product_terms));
	std::copy(b_copy.product_terms.begin(), b_copy.product_terms.end(), std::back_inserter(result.product_terms));
	std::copy(a_copy.divisor_terms.begin(), a_copy.divisor_terms.end(), std::back_inserter(result.divisor_terms));
	std::copy(b_copy.divisor_terms.begin(), b_copy.divisor_terms.end(), std::back_inserter(result.divisor_terms));
	return result;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator*= (fractional_polynomial_in_x b) {
	fractional_polynomial_in_x b_copy = b.clone();
	product_terms.reserve(product_terms.size() + b_copy.product_terms.size());
	divisor_terms.reserve(divisor_terms.size() + b_copy.divisor_terms.size());
	std::copy(b_copy.product_terms.begin(), b_copy.product_terms.end(), std::back_inserter(product_terms));
	std::copy(b_copy.divisor_terms.begin(), b_copy.divisor_terms.end(), std::back_inserter(divisor_terms));
	return *this;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator*(polynomial_in_x b) {
	fractional_polynomial_in_x result = clone();
	result.product_terms.push_back(b);
	return result;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator*= (polynomial_in_x b) {
	product_terms.push_back(b);
	return *this;
}
fractional_polynomial_in_x fractional_polynomial_in_x::cancel() {
	for (int i = 0; i < divisor_terms.size();) {
		polynomial_in_x divisor_term = divisor_terms[i];
		bool cancelled = false;
		for (int j = 0; j < product_terms.size();j++) {
			polynomial_in_x product_term = product_terms[j];
			if (product_term == divisor_term) {
				if (cloned)product_term.discard();
				product_terms.erase(product_terms.begin() + j);
				cancelled = true;
				break;
			}
		}
		if (!cancelled)i++;
		else {
			if (cloned)divisor_term.discard();
			divisor_terms.erase(divisor_terms.begin() + i);
		}
	}
	return *this;
}
varFloat fractional_polynomial_in_x::evaluate(varFloat x) {
	varFloat product = 1.0;
	for (polynomial_in_x pterm : product_terms) {
		varFloat v = pterm.evaluate(x);
		product *= v;
		v.discard();
	}
	for (polynomial_in_x dterm : divisor_terms) {
		varFloat v = dterm.evaluate(x);
		product /= v;
		v.discard();
	}
	return product;
}
varFloat fractional_polynomial_in_x::evaluateSeries(std::vector<polynomial_in_x> series, varFloat x) {
	varFloat product = 1.0;
	for (polynomial_in_x term : series) {
		varFloat v = term.evaluate(x);
		product *= v;
		v.discard();
	}
	return product;
}
void fractional_polynomial_in_x::clearProductTerms() {
	if (cloned) {
		for (polynomial_in_x pterm : product_terms)pterm.discard();
	}
	product_terms.clear();
}
void fractional_polynomial_in_x::clearDivisorTerms() {
	if (cloned) {
		for (polynomial_in_x dterm : product_terms)dterm.discard();
	}
	divisor_terms.clear();
}
fractional_polynomial_in_x fractional_polynomial_in_x::evaluateProductTerms(varFloat x) {
	varFloat product = evaluateSeries(product_terms,x);
	clearProductTerms();
	polynomial_in_x pterm = product;
	product_terms.push_back(pterm);
	return *this;
}
fractional_polynomial_in_x fractional_polynomial_in_x::evaluateDivisorTerms(varFloat x) {
	varFloat product = evaluateSeries(divisor_terms, x);
	clearDivisorTerms();
	polynomial_in_x pterm = product;
	divisor_terms.push_back(pterm);
	return *this;
}
polynomial_in_x::polynomial_in_x(varFloat x) {
	degree = 0;
	coefficients = (varFloat*)calloc(1, sizeof(varFloat));
	coefficients[0] = varFloat::copy(x);
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator/(fractional_polynomial_in_x b) {
	fractional_polynomial_in_x a_copy = clone(), b_copy = b.clone();
	for (int i = 0; i < a_copy.product_terms.size();) {
		polynomial_in_x a_pterm = a_copy.product_terms[i];
		bool cancelled = false;
		for (int j = 0; j < b_copy.product_terms.size(); ++j ) {
			polynomial_in_x b_pterm = b_copy.product_terms[j];
			if (a_pterm == b_pterm) {
				b_pterm.discard();
				b_copy.product_terms.erase(b_copy.product_terms.begin() + j);
				cancelled = true;
				break;
			}
		}
		if (!cancelled)i++;
		else {
			a_pterm.discard();
			a_copy.product_terms.erase(a_copy.product_terms.begin() + i);
		}
	}
	for (int i = 0; i < a_copy.divisor_terms.size();) {
		polynomial_in_x a_dterm = a_copy.divisor_terms[i];
		bool cancelled = false;
		for (int j = 0; j < b_copy.divisor_terms.size(); ++j) {
			polynomial_in_x b_dterm = b_copy.divisor_terms[j];
			if (a_dterm == b_dterm) {
				b_dterm.discard();
				b_copy.divisor_terms.erase(b_copy.divisor_terms.begin() + j);
				cancelled = true;
				break;
			}
		}
		if (!cancelled)i++;
		else {
			a_dterm.discard();
			a_copy.divisor_terms.erase(a_copy.divisor_terms.begin() + i);
		}
	}
	a_copy.product_terms.reserve(a_copy.product_terms.size() + b_copy.divisor_terms.size());
	a_copy.divisor_terms.reserve(a_copy.divisor_terms.size() + b_copy.product_terms.size());
	std::copy(b_copy.divisor_terms.begin(), b_copy.divisor_terms.end(), std::back_inserter(a_copy.product_terms));
	std::copy(b_copy.product_terms.begin(), b_copy.product_terms.end(), std::back_inserter(a_copy.divisor_terms));
	b_copy.product_terms.clear();
	b_copy.divisor_terms.clear();
	return a_copy;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator/(polynomial_in_x b) {
	fractional_polynomial_in_x a_copy = clone();
	polynomial_in_x b_copy = polynomial_in_x::copy(b);
	a_copy.divisor_terms.push_back(b_copy);
	return a_copy;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator/=(polynomial_in_x b) {
	divisor_terms.push_back(b);
	return *this;
}
fractional_polynomial_in_x fractional_polynomial_in_x::operator/=(fractional_polynomial_in_x b) {
	fractional_polynomial_in_x b_copy = b.clone();
	for (int i = 0; i < product_terms.size();) {
		polynomial_in_x a_pterm = product_terms[i];
		bool cancelled = false;
		for (int j = 0; j < b_copy.product_terms.size(); ++j) {
			polynomial_in_x b_pterm = b_copy.product_terms[j];
			if (a_pterm == b_pterm) {
				b_pterm.discard();
				b_copy.product_terms.erase(b_copy.product_terms.begin() + j);
				cancelled = true;
				break;
			}
		}
		if (!cancelled)i++;
		else {
			a_pterm.discard();
			product_terms.erase(product_terms.begin() + i);
		}
	}
	for (int i = 0; i < divisor_terms.size();) {
		polynomial_in_x a_dterm = divisor_terms[i];
		bool cancelled = false;
		for (int j = 0; j < b_copy.divisor_terms.size(); ++j) {
			polynomial_in_x b_dterm = b_copy.divisor_terms[j];
			if (a_dterm == b_dterm) {
				b_dterm.discard();
				b_copy.divisor_terms.erase(b_copy.divisor_terms.begin() + j);
				cancelled = true;
				break;
			}
		}
		if (!cancelled)i++;
		else {
			a_dterm.discard();
			divisor_terms.erase(divisor_terms.begin() + i);
		}
	}
	product_terms.reserve(product_terms.size() + b_copy.divisor_terms.size());
	divisor_terms.reserve(divisor_terms.size() + b_copy.product_terms.size());
	std::copy(b_copy.divisor_terms.begin(), b_copy.divisor_terms.end(), std::back_inserter(product_terms));
	std::copy(b_copy.product_terms.begin(), b_copy.product_terms.end(), std::back_inserter(divisor_terms));
	b_copy.product_terms.clear();
	b_copy.divisor_terms.clear();
	return *this;
}
varFloat barycentric_output::evaluate(varFloat x) {
	for (int i = 0; i < l0.product_terms.size() + 1; ++i) {
		if (x_set[i] == x) {
			return y_set[i];
		}
	}
	varFloat l0_value = l0.evaluate(x);
	varFloat finalResult = varFloat::copy(l0_value);
	varFloat a = x - x_set[0];
	varFloat e = 0.0;
	
	for (int i = 0; i < l0.product_terms.size(); ++i) {
		varFloat b = x - x_set[i+1], c = a / b,d = c * yw_ratios[i];
		e += d;
		d.discard(); c.discard();  b.discard();
	}
	e *= l0_value;
	finalResult += e; e.discard();
	l0_value.discard();
	return finalResult;
}
void barycentric_output::discard() {
	for (int i = 0; i < l0.product_terms.size() + 1; ++i){
		x_set[i].discard();
		y_set[i].discard();
		if (i > 1) {
			yw_ratios[i - 1].discard();
		}
	}
	free(x_set); free(y_set); free(yw_ratios);
	l0.discard();
}