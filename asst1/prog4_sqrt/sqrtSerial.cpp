#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void sqrtSerial(int N,
                float initialGuess,
                float values[],
                float output[]) {

    static const float kThreshold = 0.00001f;

    for (int i = 0; i < N; i++) {
        // int cnt = 0;
        float x = values[i];
        float guess = initialGuess;

        float error = fabs(guess * guess * x - 1.f);

        while (error > kThreshold) {
            guess = (3.f * guess - x * guess * guess * guess) * 0.5f;
            error = fabs(guess * guess * x - 1.f);
            // cnt++;
        }

        // if (i == 0)
        //     printf("%d\n", cnt);

        output[i] = x * guess;
    }
}

