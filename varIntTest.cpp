#include <iostream>
#include <windows.h>
#include <psapi.h>
#include <chrono>
#include <random>
#include "binhminh_varInt.h"
#include "varIntTest.h"
// Assume varInt has fromHex, toHexString, naiveMult, etc.

void stressTestMultiply(size_t bytesA, size_t bytesB) {
    varInt a, b;
    a.length = bytesA;
    a.data = (byte*)malloc(bytesA);
    b.length = bytesB;
    b.data = (byte*)malloc(bytesB);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (int i = 0; i < bytesA; ++i) {
        a.data[i] = dis(gen);
    }
    for (int i = 0; i < bytesB; ++i) {
        b.data[i] = dis(gen);
    }
    std::cout << "Multiplying A (" << bytesA << " bytes) × B (" << bytesB << " bytes) in hybrid mode: naive x karatsuba\n";
    PROCESS_MEMORY_COUNTERS pmc;
    auto start = std::chrono::high_resolution_clock::now();
    varInt result = a * b;
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    auto end = std::chrono::high_resolution_clock::now();
    long elapsed = std::chrono::duration_cast<std::chrono::
        milliseconds>(end - start).count();
    std::cout << "Time taken: " << elapsed << " ms\n";
    a.printString();
    b.printString();
    result.printString();
    // Optional: print result
    std::cout << "Result Length: " << result.length << "\n";
    a.discard();
    b.discard();
    result.discard();
    std::cout << "Working Set Size (RAM in use): " << pmc.WorkingSetSize / 1024 << " KB\n";

}
	
varInt randomVarInt(int byteSize) {
    std::vector<byte> data(byteSize);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (int i = 0; i < byteSize; ++i) {
        data[i] = dis(gen);
    }

    // Ensure it's not all zero
    if (std::all_of(data.begin(), data.end(), [](byte b) { return b == 0; })) {
        data[0] = 1;
    }

    return varInt(data.data(), byteSize, byteSize);
}

// Benchmark function
void naiveDiviStressTest(int n, int n2) {
    varInt dividend = randomVarInt(n);
    varInt divisor = randomVarInt(n2);
    dividend.normalize();
    divisor.normalize();
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    varInt remainder(0), quotient = dividend.naiveDivi(divisor, &remainder);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    printf("Time elapsed: %ds or %dms\n", std::chrono::duration_cast<std::chrono::seconds>(end - start),
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start));
    varInt a = divisor * quotient, b = a + remainder;
    b.normalize();
    printf("\nIntegrity %s\n", (b == dividend ? "yes" : "no"));
    b.printHex();
    dividend.printHex();
    varInt c = dividend - a;
    printf("Assuming quotient is correct, remainder should be ");
    c.printString();
    char* l[]  = {varInt::toString(dividend),varInt::toString(divisor) ,varInt::toString(quotient) ,varInt::toString(remainder)};
    printf("Dividend %s(%d char)\nDivisor %s(%d char)\nQuotient %s(%d char)\nRemainder %s(%d char)",
        l[0], strlen(l[0]), l[1], strlen(l[1]), l[2], strlen(l[2]), l[3], strlen(l[3]));
    for (int i = 0; i < 4; ++i) {
        free(l[i]);
    }

    a.discard();
    b.discard();
    c.discard();
    remainder.discard();
    quotient.discard();
    dividend.discard();
    divisor.discard();
}
void benchmarkToom3(size_t n, size_t n2) {
    varInt factor = randomVarInt(n), factor2 = randomVarInt(n2);
    printf("Stress Test Toom3 implementation\n");
    printf("Factor 1 size: %d , Factor 2 size: %d \n", n, n2);
    PROCESS_MEMORY_COUNTERS pmc;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    varInt product = factor.Toom3Mult(factor2);
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    printf("Time elapsed: %dms\nMemory usage:%d KB\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start),
        pmc.WorkingSetSize/1024);
    printf("Product length: %d bytes\n", product.length);
    //product.printString();
    product.discard();

    factor.discard(); factor2.discard();
}
void benchmarkNaiveMult(size_t n, size_t n2) {
    varInt factor = randomVarInt(n), factor2 = randomVarInt(n2);
    printf("Stress Test naiveMult\n");
    printf("Factor 1 size: %d , Factor 2 size: %d \n", n, n2);
    PROCESS_MEMORY_COUNTERS pmc;
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    varInt product = factor.naiveMult(factor2);
    GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    printf("Time elapsed: %dms\nMemory usage:%d KB\n", std::chrono::duration_cast<std::chrono::milliseconds>(end - start),
        pmc.WorkingSetSize / 1024);
    printf("Product length: %d bytes\n", product.length);
    //product.printString();
    product.discard();

    factor.discard(); factor2.discard();
}

