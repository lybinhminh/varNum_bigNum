#include "binhminh_varInt.h"
#include "binhminh_varFloat.h"
#ifndef VARINT_TEST_H
#define VARINT_TEST_H
void stressTestMultiply(size_t, size_t);
varInt randomVarInt(int);
varFloat randomVarFloat(size_t, size_t, size_t);
void naiveDiviStressTest(int, int);
void benchmarkToom3(size_t, size_t);
void benchmarkNaiveMult(size_t, size_t);
#endif
