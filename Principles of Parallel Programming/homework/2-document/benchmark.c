#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#define M_PI_2        1.57079632679489661923	/* pi/2 */
#define M_PI_2_INV    (1.0/M_PI_2)
#define M_2_SQRTPI    1.12837916709551257390    /* 2/sqrt(pi) */
#define ERF_COEF      (1.0/M_2_SQRTPI)

const int SIZE=100;
const int CYCLES=10000000;

double with_atan(double x) {
	/* normalized atan */
	return M_PI_2_INV*atan(M_PI_2*x);
}

double with_exp(double x) {
	return 1.0/(1.0 + exp(-x));
}

double with_sqrt(double x) {
	return x/sqrt(1.0 + x*x);
}

double with_erf(double x) {
	return erf(ERF_COEF*x);
}

double with_fabs(double x) {
	return x/(1.0 + fabs(x));
}

double benchmark(const char* name, double (*fun)(double)){
    double err = 0;
    for(double i = 1000000; i <= 100000000; i += 1.0){
        err += fabs(fun(i) - with_exp(i));
    }
    printf("%-17s %6.10f\n", name, err);
    return err;
}

int main(int argc, char **argv) {
	benchmark("atan(pi*x/2)*2/pi", with_atan);
	benchmark("x/sqrt(1+x^2)", with_sqrt);
	benchmark("erf(sqrt(pi)*x/2)", with_erf);
	benchmark("tanh(x)", tanh);
	benchmark("x/(1+|x|)", with_fabs);
}
