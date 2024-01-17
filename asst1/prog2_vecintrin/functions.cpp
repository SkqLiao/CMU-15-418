#include <stdio.h>
#include <algorithm>
#include <math.h>
#include "CMU418intrin.h"
#include "logger.h"
using namespace std;


void absSerial(float *values, float *output, int N) {
    for (int i = 0; i < N; i++) {
        float x = values[i];

        if (x < 0) {
            output[i] = -x;
        } else {
            output[i] = x;
        }
    }
}

// implementation of absolute value using 15418 instrinsics
void absVector(float *values, float *output, int N) {
    __cmu418_vec_float x;
    __cmu418_vec_float result;
    __cmu418_vec_float zero = _cmu418_vset_float(0.f);
    __cmu418_mask maskAll, maskIsNegative, maskIsNotNegative;

    //  Note: Take a careful look at this loop indexing.  This example
    //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
    //  Why is that the case?
    for (int i = 0; i < N; i += VECTOR_WIDTH) {

        // All ones
        maskAll = _cmu418_init_ones();

        // All zeros
        maskIsNegative = _cmu418_init_ones(0);

        // Load vector of values from contiguous memory addresses
        _cmu418_vload_float(x, values + i, maskAll);             // x = values[i];

        // Set mask according to predicate
        _cmu418_vlt_float(maskIsNegative, x, zero, maskAll);     // if (x < 0) {

        // Execute instruction using mask ("if" clause)
        _cmu418_vsub_float(result, zero, x, maskIsNegative);      //   output[i] = -x;

        // Inverse maskIsNegative to generate "else" mask
        maskIsNotNegative = _cmu418_mask_not(maskIsNegative);     // } else {

        // Execute instruction ("else" clause)
        _cmu418_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

        // Write results back to memory
        _cmu418_vstore_float(output + i, result, maskAll);
    }
}

// Accepts an array of values and an array of exponents
// For each element, compute values[i]^exponents[i] and clamp value to
// 4.18.  Store result in outputs.
// Uses iterative squaring, so that total iterations is proportional
// to the log_2 of the exponent
void clampedExpSerial(float *values, int *exponents, float *output, int N) {
    for (int i = 0; i < N; i++) {
        float x = values[i];
        float result = 1.f;
        int y = exponents[i];
        float xpower = x;

        while (y > 0) {
            if (y & 0x1) {
                result *= xpower;
            }

            xpower = xpower * xpower;
            y >>= 1;
        }

        if (result > 4.18f) {
            result = 4.18f;
        }

        output[i] = result;
    }
}

void print(__cmu418_vec_int &vec) {
    int *p = (int *) &vec;

    for (int i = 0; i < VECTOR_WIDTH; i++) {
        printf("%d ", p[i]);
    }

    puts("");
}

void print(__cmu418_vec_float &vec) {
    float *p = (float *) &vec;

    for (int i = 0; i < VECTOR_WIDTH; i++) {
        printf("%.5f ", p[i]);
    }

    puts("");
}

void print(__cmu418_mask &vec) {
    bool *p = (bool *) &vec;

    for (int i = 0; i < VECTOR_WIDTH; i++) {
        printf("%d ", (int)p[i]);
    }

    puts("");
}

void clampedExpVector(float *values, int *exponents, float *outputs, int N) {
    // Implement your vectorized version of clampedExpSerial here
    //  ...
    __cmu418_vec_float value, output;
    __cmu418_vec_int exp, active;
    __cmu418_vec_int zeros = _cmu418_vset_int(0);
    __cmu418_vec_int ones = _cmu418_vset_int(1);
    __cmu418_vec_float four = _cmu418_vset_float(4.18f);

    for (int i = 0; i < N; i += VECTOR_WIDTH) {
        int len = min(VECTOR_WIDTH, N - i);
        __cmu418_mask mask = _cmu418_init_ones(len), now_mask;

        _cmu418_vload_float(value, values + i, mask);
        _cmu418_vload_int(exp, exponents + i, mask);
        _cmu418_vset_float(output, 1.0, mask);

        int cnt = 0;

        while (true) {
            // active : y & 0x1
            _cmu418_vbitand_int(active, exp, ones, mask);
            // active y & 0x1: int -> mask
            _cmu418_veq_int(now_mask, active, ones, mask);
            // output *= x
            _cmu418_vmult_float(output, output, value, now_mask);
            // x *= x
            _cmu418_vmult_float(value, value, value, mask);
            // y >>= 1
            _cmu418_vshiftright_int(exp, exp, ones, mask);

            // count exp == 0
            _cmu418_veq_int(now_mask, exp, zeros, mask);
            // clean now_mask[len, VECTOR_WIDTH) to 0
            now_mask = _cmu418_mask_and(now_mask, mask);

            // all exp == 0
            if (_cmu418_cntbits(now_mask) == len)
                break;
        }

        _cmu418_vgt_float(now_mask, output, four, mask);
        _cmu418_vmove_float(output, four, now_mask);
        _cmu418_vstore_float(outputs + i, output, mask);
    }
}


float arraySumSerial(float *values, int N) {
    float sum = 0;

    for (int i = 0; i < N; i++) {
        sum += values[i];
    }

    return sum;
}

// Assume N % VECTOR_WIDTH == 0
// Assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N) {
    // Implement your vectorized version here
    //  ...
    __cmu418_vec_float value, nxt_value;
    __cmu418_mask mask = _cmu418_init_ones();
    __cmu418_mask mask2 = _cmu418_init_ones(VECTOR_WIDTH / 2);

    for (int n = N; n > VECTOR_WIDTH; n >>= 1) {
        for (int j = 0; j < n; j += VECTOR_WIDTH) {
            _cmu418_vload_float(value, values + j, mask);
            _cmu418_hadd_float(nxt_value, value);
            _cmu418_interleave_float(value, nxt_value);
            _cmu418_vstore_float(values + j / 2, value, mask2);
        }
    }

    _cmu418_vload_float(value, values, mask);

    for (int i = VECTOR_WIDTH; i > 1; i >>= 1) {
        _cmu418_hadd_float(nxt_value, value);
        _cmu418_interleave_float(value, nxt_value);
    }

    _cmu418_vstore_float(values, value, mask);
    return values[VECTOR_WIDTH - 1];
}
