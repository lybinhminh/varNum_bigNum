# varNum_bigNum
Binh Minh Ly's crappy C/C++ library for arbitrary precision / big INTEGERS and DECIMAL/ REAL NUMBERS
Author: Copilot (assistant) and BinhMinh (main coder)
EXAMPLE CODE
EXAMPLE #1
varInt a(5), b = varInt::fromString("12"), c = a * b; // 2 ways of initialize varInt here, either using constructor: varInt(int32_t origin) or static function:
                                                      //  varInt fromString(const char*)
c.printString(); // in this case , would print 60\n
c.printHex(); // in this case, would print 0x3c\n
a.discard(); b.discard(); c.discard(); // must call to free up memory

EXAMPLE #2
union{
int32_t i;
char arr[4];
} ui;
ui.i = 11123423; // chop 11123423 to bytes via union
varInt c((int64_t)123456789), d(ui.arr,sizeof(int32_t),sizeof(32_t)); // init varInt using constructor: varInt(int64_t) and deep copy scheme varInt(unsigned char arr*,
                                                                      // int src_len, int dest_size)
d.normalize(); // remove trailing 0x00 or 0xFF                                                                      
c.discard(); d.discard();

#NOTE as varInt has 2 constructors using int32 or int64, you can do casting from normal integer, .e.g (varInt)5 , but is not recommended
as you cannot discard this 'seemingly-temporary' varInt of value 5, which can cause memory leak if is repetively called.
EXAMPLE #3
varInt integer(5), numerator(12), denominator(17);
varFloat a(integer,numerator,denominator), b(integer); // 2 ways of initializing a varFloat: varFloat(varInt,varInt,varInt) if the number has decimal part
                                                       // or in other words, not rounded, and varFloat(varInt) for otherwise (rounded number).
#Warning: denominator MUSTN'T be 0;
a.printString();
a.discard(); integer.discard(); numerator.discard(), denominator.discard(); // always remember to discard binhminh's types !!!
varInt ---> bigNum , bigInteger properties:
Base: 256 ( integers are stored as array of bytes (unsigned char) in little endian order)
Max Size: SIZE_T (unsigned long long) ( maximum number of bytes)
80% of methods have been noted/commented in the 'binhminh_varInt.cpp' file
NOTE: MANUAL MEMORY MANAGEMENT , discard() on varInt must be called after your own declarations of varInts.
varFloat ---> arbitrary precision decimal/ real number type
Format: integer + numerator/ denominator ( in other words, varFloat is a combination of 3 varInts)
Currently, basic methods/ operators have been installed
IMPORTANT NOTE: Guaranteed to work on Windows 10+, C++ iso 14+, but not on Linux, MacOS
NOTE: this is a side project on my journey of implement TLS 1.2 from pure C/C++ and Winsock2, it is not performant, can say that it is terrible at speed, but if you're a newcomer in programming or a hobbyist like me, it may be worth a sight
LICENSE: None, you can use, modify the lib, but DO NOT use it for 'shadow' purposes and 'harm' my profile! OK!!!
Additional features:
+ Fast multiplication with karatsuba (I myself cannot implement FFT or Toom-3 as it is too hard for my smooth brain)
+ Barret reduction
+ Modular inverseE
+ Euclidean GCD
+ toString()
+ printString()
+ fromString(const char*)
For varFloat:
+ Common denominator conversion
+ toString()
+ printString()
Worth-taking Notes:
+ printString() and toString() is heavy task load (takes lots of time), especially for varFloats

BENCHMARK DATA pointed out that binhminh's types' naiveDivi() complexity is O(n^1.5 - n^2), naiveMult() is O(n^2), karatsubaMult(n^1.5). AND time consumed for calculations with 256-byte varInts are around 6ms
Benchmark specs:
I7 10750H
RAM 16GB 2933MHz
Max CPU load: 12,7% , temp: 94 - 95 oC, Ram usage: 12 - 19MB, time: 6ms for( 256-byte * 256-byte ), time: 0ms for( 256-byte / 256-byte)
