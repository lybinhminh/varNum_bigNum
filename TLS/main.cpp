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
	std::chrono::high_resolution_clock::time_point start, end;
	size_t sum = 0, time, last = 0;
	long long difference = 0, seed = clock();
	for (int i = 1; i <= 30; ++i) {
		printf("Round %d(varfloat initialization)\n", i);
		srand(seed);
		double a = rand() / (rand() + 100 + seed);
		seed += (int)a - i * 20;

		start = std::chrono::high_resolution_clock::now();
		varFloat b = a;
		b.printString(10);
		end = std::chrono::high_resolution_clock::now();
		time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		printf("Time elapsed = %lld microsecond\n", time);
		sum += time;
		difference += (time - last);
		difference /= 2;
		b.discard();
		last = time;
	}
	printf("Avg time = %lld microseconds\nAvg difference = %lld microseconds\n", sum / 30,difference);
	return 0;	
}


