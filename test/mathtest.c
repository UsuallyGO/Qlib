
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "qlib/qgeneral.h"
#include "qlib/qmath.h"

#define SIZE 20

void testPolynomial()
{
    double x[SIZE], fx[SIZE], z[SIZE];
    QPolynomial poly;
    double fz;
    int index, res;

    x[0] = -3.0, fx[0] = -5.0;
    x[1] = -2.0, fx[1] = -1.1;
    x[2] =  2.0, fx[2] =  1.9;
    x[3] =  3.0, fx[3] =  4.8;

    z[0] = -2.5, z[1] = -1.0, z[2] = 0.0;
    z[3] =  1.0, z[4] =  1.5, z[5] = 2.5;

    poly = QMath_polynomial(x, fx, 4);
    assert(poly != NULL);

    for(index = 0; index < 6; index++)
    {
        res = QMath_interpolate(poly, z[index], &fz);
        assert(res == QLIB_SUCCEEDED);
        printf("z:%lf fz:%lf\n", z[index], fz);
    }

    QMath_polyrelease(poly);
}

void testkbleastsquare()
{
    double x[SIZE], y[SIZE], z[SIZE];
    QKBleastsquare kbls;
    int index;

    x[0]  = 4.0, y[0]  = 197.0;
    x[1]  = 6.0, y[1]  = 272.5;
    x[2]  = 2.0, y[2]  = 100.5;
    x[3]  = 5.0, y[3]  = 228.0;
    x[4]  = 7.0, y[4]  = 327.0;
    x[5]  = 6.0, y[5]  = 279.0;
    x[6]  = 3.0, y[6]  = 148.0;
    x[7]  = 8.0, y[7]  = 377.0;
    x[8]  = 5.0, y[8]  = 238.0;
    x[9]  = 3.0, y[9]  = 142.0;
    x[10] = 1.0, y[10] = 66.0;
    x[11] = 5.0, y[11] = 239.0;

    kbls = QMath_kbleastsquare(x, y, 12);
    printf("Linear k:%+1.6e b:%+1.6e\n", QKBLS_GET_K(kbls), QKBLS_GET_B(kbls));
}

void testequation()
{
    int coeff[4], res;
    double x0, result, precision;

    //f(x) = 3x^3 + x^2 + 5x^1 + 8;
    coeff[0] = 3, coeff[1] = 1, coeff[2] = 5, coeff[3] = 8;
    precision = 0.001;
    x0 = 5;

    res = QMath_equation(coeff, 4, &result, x0, precision);
    if(res != QLIB_SUCCEEDED)
        printf("No root.\n");
    else
        printf("root is:%lf\n", result);
}

int main()
{
    testPolynomial();
    testkbleastsquare();
    testequation();
    return 0;
}
