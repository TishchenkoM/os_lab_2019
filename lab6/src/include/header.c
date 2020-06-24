#include "header.h"

uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
    uint64_t result = 0;
    a = a % mod;
    while (b > 0) {
        if (b % 2 == 1)
            result = (result + a) % mod;
        a = (a * 2) % mod;
        b /= 2;
    }

    return result % mod;
}

uint64_t Factorial(const struct FactorialArgs *args) {
    uint64_t ans = 1;

    int i;
    for(i = args->begin; i < args->end; i++) {
        if (i % args->mod != 0) {
            ans *= i % args->mod;
        }
    }
    return ans;
}

void *ThreadFactorial(void *args) {
    struct FactorialArgs *fargs = (struct FactorialArgs *)args;
    return (void *)(uint64_t *)Factorial(fargs);
}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
    char *end = NULL;
    unsigned long long i = strtoull(str, &end, 10);
    if (errno == ERANGE) {
        fprintf(stderr, "Out of uint64_t range: %s\n", str);
        return false;
    }

    if (errno != 0)
        return false;

    *val = i;
    return true;
}