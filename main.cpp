#define _CRTDBG_MAP_ALLOC

#include <crtdbg.h>

#include <windows.h>
#include <Psapi.h>
#include <chrono>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include "binhminh_hashes.h"
#include "binhminh_utils.h"
#include "binhminh_varInt.h"
#include "varIntTest.h"
#include "binhminh_varFloat.h"
#include "binhminh_polynomial.h"
#pragma comment(lib,"ws2_32")




int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	varInt n = randomVarInt(10), abs_n = varInt::abs(n);
	abs_n._barret_cache_ = true;
	for (int i = 0; i < 7; ++i) {
		varInt a = randomVarInt(15), abs_a = varInt::abs(a);
		bool negativity = varInt::isNegative(n) ^ varInt::isNegative(a);
		
		{
			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
			varInt remainder, quotient = a.naiveDivi(n, &remainder);
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
			printf("Time elapsed for naive: %d microsecs\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start));
			printf("Naive quotient = ");
			quotient.printString();
			printf("Naive remainder = ");
			remainder.printString();
			remainder.discard();
			quotient.discard();
		}
		
		
		{
			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
			varInt remainder, quotient = abs_a.barretFastDivision(abs_n, &remainder);
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
			printf("Time elapsed for barret: %d microsecs\n", std::chrono::duration_cast<std::chrono::microseconds>(end - start));
			if (negativity) {
				varInt neg_q = varInt::negate(quotient);
				quotient.discard();
				quotient = neg_q;
			}
			if (varInt::isNegative(a)) {
				varInt neg_r = varInt::negate(remainder);
				remainder.discard();
				remainder = neg_r;
			}
			printf("Barret quotient = ");
			quotient.printString();
			printf("Barret remainder = ");
			remainder.printString();
			remainder.discard();
			quotient.discard();
		}
		
		printf("\n");
		a.discard(); abs_a.discard();
	}
	return 0;
}


