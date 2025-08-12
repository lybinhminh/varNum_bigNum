#include "binhminh_varInt.h"
#include "binhminh_varFloat.h"
namespace varIntConst {
	const varInt ZERO = varInt(0);
	const varInt ONE = varInt(1);
	const varInt TWO = varInt(2);
	const varInt TEN = varInt(10);
	const varInt HEX = varInt(16);
	const varInt BASE = varInt(256);
}
namespace varFloatConst {
	varFloat EPSILON(0.0000000000000001);
	varFloat ZERO(0.0);
	varFloat ONE(1.0);
	varFloat TWO(2.0);
	varFloat TEN(10.0);
}