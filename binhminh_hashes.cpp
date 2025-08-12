#include "binhminh_hashes.h"
#include "binhminh_utils.h"
#include <cstdlib>
#include <cstring>
#include <winsock2.h>

#include <cstdio>
uint32_t sha256::smallSigma0(uint32_t v) {
	return ROTR(v, 7) ^ ROTR(v, 18) ^ (v >> 3);
}
uint32_t sha256::smallSigma1(uint32_t v) {
	return ROTR(v, 17) ^ ROTR(v, 19) ^ (v >> 10);
}
uint32_t sha256::bigSigma0(uint32_t v) {
	return ROTR(v, 2) ^ ROTR(v, 13) ^ ROTR(v, 22);
}
uint32_t sha256::bigSigma1(uint32_t v) {
	return ROTR(v, 6) ^ ROTR(v, 11) ^ ROTR(v, 25);
}
uint32_t sha256::choose(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) ^ (~x & z);
}
uint32_t sha256::major(uint32_t x, uint32_t y, uint32_t z) {
	return (x & y) ^ (x & z) ^ (y & z);
}
unsigned char* sha256::run(unsigned char* message, int length) {
	float a = (length * 8 + 66) / (float)512;
	int b = 0;
	for (; b < a; ++b);
	b = 512 * b;
	unsigned char* string = (unsigned char*)malloc(b / 8);
	unsigned char *network_order_int64_length = hton_int(length * 8, 64);
	for (int i = 0, j = 0; i < b / 8; ++i) {
		if (i < length)
		{
			string[i] = message[i];
		}
		else {
			if (i < b / 8 - 8)
			{
				if (i == length)
					string[i] = (unsigned char)0x80;
				else
					string[i] = (unsigned char)0x00;
			}
			else {
				string[i] = network_order_int64_length[i - b / 8 + 8];
			}
		}
	}
	int c = 0, d = 0;
	unsigned char** _512blocks = (unsigned char**)malloc((b /= 512) * sizeof(unsigned char*));
	for (int i = 0; i < b; ++i) {
		_512blocks[i] = (unsigned char*)malloc(64);
		while (d < 64) {
			_512blocks[i][d++] = *(string + c++);
		}
		d = 0;
	}
	uint32_t H[8] = {
	0x6a09e667, 0xbb67ae85,
	0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c,
	0x1f83d9ab, 0x5be0cd19
	};
	const uint32_t K[64] = { 0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
	for (int i = 0; i < b; ++i) {
		unsigned char* _512block = _512blocks[i];
		uint32_t W[64];
		for (int t = 0, k = 0; t < 64; ++t) {
			if (t < 16) {
				W[t] = ((uint32_t)(unsigned char)*(_512block + t * 4) << 24) | ((uint32_t)(unsigned char)*(_512block + t * 4 + 1) << 16) |
					((uint32_t)(unsigned char)*(_512block + t * 4 + 2) << 8) | ((uint32_t)(unsigned char)*(_512block + t * 4 + 3));
			}
			else {
				W[t] = smallSigma1(W[t - 2]) + W[t - 7] + smallSigma0(W[t - 15]) + W[t - 16];
			}
		}
		uint32_t a = H[0], b = H[1], c = H[2], d = H[3], e = H[4], f = H[5], g = H[6], h = H[7];
		for (int j = 0; j < 64; ++j) {
			uint32_t T1 = h + bigSigma1(e) + choose(e, f, g) + K[j] + W[j],
				T2 = bigSigma0(a) + major(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}
		H[0] += a; H[1] += b; H[2] += c; H[3] += d; H[4] += e; H[5] += f; H[6] += g; H[7] += h;
	}
	unsigned char* digest = (unsigned char*)malloc(32);
	for (int i = 0; i < 8; ++i) {
		unsigned char* str = hton_int(H[i],32);
		memcpy(digest + i * 4, str, 4);
		free(str);
	}
	for (int i = 0; i < b; ++i) {
		_512blocks[i] = NULL;
	}
	free(_512blocks);
	free(string);
	return digest;
}
unsigned char* sha256::hash(unsigned char* msg, int length) {
	return run(msg, length);
}
unsigned char* sha256::over(unsigned char* msg, int length) {
	return run(msg, length);
}
unsigned char* sha256::text(unsigned char* msg, int length) {
	return run(msg, length);
}
unsigned char* sha256::of(unsigned char* msg, int length) {
	return run(msg, length);
}
unsigned char* sha256::hton_int(int32_t origin, size_t destSize) {
	if (destSize % 8 != 0) {
		std::perror("sha256::hton_int destSize must be a multiple of 8");
		return NULL;
	}
	unsigned char* result = (unsigned char*)malloc(destSize / 8);
	size_t padding_len = destSize / 8 - 4;
	padding_len = padding_len > 0 ? padding_len : 0;
	bool negativity = origin < 0;
	for (int i = 0; i < destSize / 8; ++i) {
		if (i < padding_len) {
			result[i] = negativity ? 0xFF : 0x00;
		}
		else {
			result[i] = (unsigned char)(origin >> ((3 - i + padding_len) * 8));
		}
	}
	return result;
}
int32_t sha256::ntoh_int(unsigned char* buffer, size_t fromSize) {
	int32_t result = 0;
	size_t padding_len = fromSize / 8 - 4;
	padding_len = padding_len > 0 ? padding_len : 0;
	for (int i = 0; i < fromSize / 8; ++i) {
		if (i >= padding_len) {
			result |= (int32_t)(buffer[i] << ((3 - i + padding_len) * 8));
		}
	}
	return result;
}