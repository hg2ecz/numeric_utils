/* half: 1 sign
	 5 exponent   !! -15
	10 bit fraction (1.xxxxx)
	exponent: 0 zero, 31: inf

  float: 1 sign
	 8 exp  !! - 127
	23 bit fraction (1.xxxxx)
	exponent: 0 zero, ff: inf
*/

float half2float(const void *f16) {
    unsigned int f16bin = *((const unsigned short *)f16);
    unsigned int f32bin = ((f16bin & 0x8000) << 16) | (((f16bin & 0x7fff) << 13) + ((127-15) << 23)); // exp 8-5 = 3, frac 23-10 bit -->  <<13 
    float *f32float = (float *)&f32bin;
    return *f32float;
}

void float2half(void *f16, const float *f32) {
    unsigned int f32bin = *((const unsigned int *)f32);
    unsigned short *outshort = (unsigned short *)f16;
    if (f32bin & 0x1000) f32bin += 1<<13; // round 0.5
    int exp = ((f32bin >> 13) & (0xff<<10)) - ((127-15)<<10);
    if (exp > (31<<10)) exp=31<<10; else if (exp < 0) exp=0;
    unsigned short f16bin = ((f32bin >> 16) & 0x8000) | exp | ((f32bin & 0x7fffff) >> 13); // exp 8-5 = 3, frac 23-10 bit -->  <<13 
    *outshort = f16bin;
}


#ifdef _HALF2FLOAT_TEST_
#include <stdio.h>
int main() {
    __fp16 halfval;
    halfval = -4.2; printf("%f -> %f\n", halfval, half2float(&halfval));
    halfval = 4.2;  printf("%f -> %f\n", halfval, half2float(&halfval));

    halfval = -0.42; printf("%f -> %f\n", halfval, half2float(&halfval));
    halfval = 0.42;  printf("%f -> %f\n", halfval, half2float(&halfval));

    puts("");
    float floatval;
    halfval = 0;
    floatval = -4.2; float2half(&halfval, &floatval); printf("%f -> %f\n", floatval, halfval);
    floatval = 4.2;  float2half(&halfval, &floatval); printf("%f -> %f\n", floatval, halfval);

    floatval = -0.42; float2half(&halfval, &floatval); printf("%f -> %f\n", floatval, halfval);
    floatval = 0.42;  float2half(&halfval, &floatval); printf("%f -> %f\n", floatval, halfval);

    for (int i=65000; i<65536; i++) {
	floatval = i;
	__fp16 halfval2 = i;
	float2half(&halfval, &floatval); printf("%f -> %f (%f)\n", floatval, halfval, halfval2);
    }

    for (int i=32500; i<32768; i++) {
	floatval = 1./i;
	__fp16 halfval2 = 1./i;
	float2half(&halfval, &floatval); printf("%f -> %f (%f)\n", floatval, halfval, halfval2);
    }

    return 0;
}
#endif
