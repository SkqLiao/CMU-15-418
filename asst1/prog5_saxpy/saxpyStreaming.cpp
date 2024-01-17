#include <smmintrin.h> // For _mm_stream_load_si128
#include <emmintrin.h> // For _mm_mul_ps
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

extern void saxpySerial(int N,
                        float scale,
                        float X[],
                        float Y[],
                        float result[]);





void saxpyStreaming(int N,
                    float scale,
                    float X[],
                    float Y[],
                    float result[]) {
    // Replace this code with ones that make use of the streaming instructions
    saxpySerial(N, scale, X, Y, result);
    // auto s = _mm_set1_ps(scale);

    // for (int i = 0; i < N; i += 4) {
    //     _mm_stream_ps(&result[i], _mm_add_ps(_mm_mul_ps(_mm_load_ps(&X[i]), s), _mm_load_ps(&Y[i])));
    // }

    // for (int i = 0; i < N; ++i) {
    //     float r = scale * X[i] + Y[i];

    //     if (std::fabs(r - result[i]) > 1e-5) {
    //         printf("No!!!\n");
    //         exit(0);
    //     }

    //     printf("%d\n", (int)(std::fabs(result[i] - r) < 1e-5));
    // }
}

