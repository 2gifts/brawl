/* modf(double x, double *iptr): fdlibm (Sun Microsystems, 1993), as built in the MSL math library. */

#define __HI(x) *(int*)&x
#define __LO(x) *(1 + (int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1 + (int*)x)

static const double one = 1.0;

double modf(double x, double *iptr) {
    int i0, i1, j0;
    unsigned i;
    i0 = __HI(x);
    i1 = __LO(x);
    j0 = ((i0 >> 20) & 0x7FF) - 0x3FF;

    if (j0 < 20) {
        if (j0 < 0) {
            __HIp(iptr) = i0 & 0x80000000;
            __LOp(iptr) = 0;
            return x;
        }
        else {
            i = (0x000FFFFF) >> j0;
            if (((i0 & i) | i1) == 0) {
                *iptr = x;
                __HI(x) &= 0x80000000;
                __LO(x) = 0;
                return x;
            }
            else {
                __HIp(iptr) = i0 & (~i);
                __LOp(iptr) = 0;
                return x - *iptr;
            }
        }
    }
    else if (j0 > 51) {
        *iptr = x * one;
        __HI(x) &= 0x80000000;
        __LO(x) = 0;
        return x;
    }
    else {
        i = ((unsigned)(0xFFFFFFFF)) >> (j0 - 20);
        if ((i1 & i) == 0) {
            *iptr = x;
            __HI(x) &= 0x80000000;
            __LO(x) = 0;
            return x;
        }
        else {
            __HIp(iptr) = i0;
            __LOp(iptr) = i1 & (~i);
            return x - *iptr;
        }
    }
}
