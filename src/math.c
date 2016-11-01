
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "qlib/qgeneral.h"
#include "qlib/qmemory.h"
#include "qlib/qmath.h"

QPolynomial QMath_polynomial(double *x, double *fx, int num)
{
    QPolynomial poly;
    double *quotient, *temp;
    int i, j, inner;

    if(x == NULL || fx == NULL || num <= 0)
        goto Err_inval;

    poly = (struct QPolynomial_t*)QMem_Malloc(sizeof(struct QPolynomial_t));
    if(poly == NULL)
        goto Err_poly;

    quotient = (double*)QMem_Malloc(sizeof(double)*num);
    if(quotient == NULL)
        goto Err_quotient;

    temp = (double*)malloc(sizeof(double)*num);//just used inside, no memory leak
    if(temp == NULL)
        goto Err_temp;

    memcpy(temp, fx, sizeof(double)*num);//f[x0] = x0, f[x1] = x1...
    quotient[0] = temp[0];
    for(i = 1; i < num; i++)
    {
        for(inner = 0; inner < num - i; inner++)//f[xi,..xj] only needs f[x_i+1,..xj] and f[xi, x_j-1]
        {
            j = inner + i;
            temp[inner] = (temp[inner+1] - temp[inner])/(x[j] - x[inner]);
        }
        quotient[i] = temp[0];//quotient[i] is f[x0,x1,x2,...xi]
    }
    free(temp);

    poly->m_num = num;
    poly->m_x = x;
    poly->diff_quo = quotient;
    return poly;

Err_temp:
    QMem_Free(quotient);
Err_quotient:
    QMem_Free(poly);
Err_poly:
    //nothing has to do here
Err_inval:
    return NULL;
}

int QMath_interpolate(QPolynomial poly, double z, double *fz)
{
    int i, j;
    double tmp, term;

    if(poly == NULL || poly->diff_quo == NULL || fz == NULL)
        return QLIB_ERR_INVAL;

    tmp = 0;
    for(j = 0; j < poly->m_num; j++)
    {
        term = 1.0;
        for(i = 0; i < j; i++)
            term *= (z - poly->m_x[i]);

        tmp += poly->diff_quo[j]*term;
    }

    *fz = tmp;
    return QLIB_SUCCEEDED;
}

void QMath_polyrelease(QPolynomial poly)
{
    if(poly != NULL)
    {
        if(poly->diff_quo != NULL)
            QMem_Free(poly->diff_quo);
        QMem_Free(poly);
    }
}

QKBleastsquare QMath_kbleastsquare(double *x, double *y, int num)
{
    double k, b;
    double sumx, sumy, sumx2, sumxy;
    int index;
    QKBleastsquare kbls;

    if(x == NULL || y == NULL || num <= 0)
        return NULL;
    kbls = (struct QKBleastsquare_t*)QMem_Malloc(sizeof(struct QKBleastsquare_t));
    if(kbls == NULL)
        return NULL;

    sumx = sumy = sumx2 = sumxy = 0.0;
    for(index = 0; index < num; index++)
    {
        sumx += x[index]; //sumx- Sigma: x[0] + x[1] + x[2] + ...x[n]
        sumy += y[index]; //sumy- Sigma: y[0] + y[1] + y[2] + ...y[n]
        sumx2 += pow(x[index], 2.0); //sumx^2- Sigma: x[0]*x[0] + x[1]*x[1] + x[2]*x[2] + ...
        sumxy += x[index]*y[index]; //sumxy- Sigma: x[0]*y[0] + x[1]*y[1] + x[2]*y[2] + ...
    }

    //The formula of 'k': k=(n*sumxy - sumx*sumy)/(n*sumx2 - sumx*sumx)
    k = (sumxy - ((sumx*sumy)/(double)num))/ (sumx2 - (pow(sumx, 2.0)/(double)num));
    //The formula of 'b': b = (sumy - k*sumx)/n
    b = (sumy - (k*sumx))/(double)num;

    kbls->m_k = k;
    kbls->m_b = b;

    return kbls;
}

int QMath_kbfunc(QKBleastsquare kbls, double z, double *fz)
{
    if(kbls == NULL)
        return QLIB_ERR_INVAL;

    *fz = kbls->m_k*z + kbls->m_b;
    return QLIB_SUCCEEDED;
}

void _derivate(int *coeff, int len)
{
    int index;

    for(index = len-1; index >= 1; index--)
    {
        coeff[index] = coeff[index-1]*(len - index);
    }
    coeff[index] = 0;
}

double _func(const int *coeff, int len, double x0)
{
    int index;
    double tmp;

    tmp = 0.0;
    for(index = 0; index < len; index++)
    {
        if(coeff[index] == 0)
            continue;

        tmp += coeff[index] * pow(x0, len-index-1);
    }

    return tmp;
}

int QMath_equation(const int *coeff, int len, double *result,  double x0, double precision)
{
    double x1,fd, f;
    int count;
    const int UPPER_BOUND = 100;
    int *dupcoeff;

    if(coeff == NULL || len <= 0 || result == NULL)
        return QLIB_ERR_INVAL;

    count = 0;
    dupcoeff = (int*)malloc(sizeof(int)*len);
    while(count++ < UPPER_BOUND)
    {
        x1 = x0;

        f = _func(coeff, len, x1);
        memcpy(dupcoeff, coeff, sizeof(int)*len);
        _derivate(dupcoeff, len);
        fd = _func(dupcoeff, len, x1);
    
        if(fd >= 0 && (fd - 0) < precision)
            break;
        if(fd <= 0 && (0 - fd ) < precision)
            break;

        x0 = x1 - f/fd;
        if(((x1 - x0) >= 0 && (x1 - x0) < precision)
            ||((x1 - x0) <= 0 && (x0 - x1) < precision))
        {
            *result = x0;
            free(dupcoeff);
            return QLIB_SUCCEEDED;
        }
    }

    free(dupcoeff);
    return QLIB_ERR_DONE;
}
