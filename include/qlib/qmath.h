
#ifndef _QLIB_MATH_H_
#define _QLIB_MATH_H_

/* Perhaps QMath.h is the most ugly code in this lib. If you really want to 
 * use these functions, I have some advice for you: Think twice and give up
 * this idea!
 */

typedef struct QPolynomial_t* QPolynomial;
typedef struct QKBleastsquare_t* QKBleastsquare;

#define QKBLS_GET_K(kb) ((kb)->m_k)
#define QKBLS_GET_B(kb) ((kb)->m_b)

struct QPolynomial_t
{
    int m_num; //polynomial order
    double *m_x; //all known x values
    double *diff_quo;//difference quotient
};

struct QKBleastsquare_t
{
    double m_k;
    double m_b;
};

/*
 * The so called Newtown interpolation polynomial method: 
 *  1.An array of pre points: (x0, f(x0)), (x1, f(x1)), (x2, f(x2))...
 *  2.Interpolate z with these initial points to get f(z)
 *  3.Don't forget to release the QPolynomial or there will be memory leak
 */

QPolynomial QMath_polynomial(double *x, double *fx, int num);

int QMath_interpolate(QPolynomial poly, double z, double *fz);

void QMath_polyrelease(QPolynomial poly);

/*
 * The easies least square method, can only apply to y = k*x + b...
 */
QKBleastsquare QMath_kbleastsquare(double *x, double *fx, int num);

int QMath_kbfunc(QKBleastsquare kbls, double z, double *fz);

/*
 * coeff is the equation coeffients, for example: 6x^4 + 4x^3 + 0*x^2 + 5x - 8
 * The len  = 5, coeff: [0] = 6, [1] = 4, [2] = 0, [3] = 5, [4] = -8
 * User should provide the x0.... (This is really bullshit...)
 *
 * To understand the Newton's method, think about Taylor formula..
 * f(x) = f(x0) + f'(x0)(x0-x1) + o(x0-x1)...
 * If there is limf(x_n) = 0, there must be limf(x_n-1) = 0
 * So, break out when (abs(f(x_n) - f(x_n-1)) < precision)
 */
int QMath_equation(const int *coeff, int len, double *result,  double x0, double precision);
#endif //!_QLIB_MATH_H_