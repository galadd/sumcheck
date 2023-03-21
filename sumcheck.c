#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <gmp.h>

// Evaluates a polynomial at a point
void evaluate_polynomial(mpz_t result, mpz_t *coefficients, int degree, mpz_t x, mpz_t modulus) {
    mpz_set_ui(result, 0);

    for (int i = degree; i >= 0; i--) {
        mpz_mul(result, result, x);
        mpz_add(result, result, coefficients[i]);
        mpz_mod(result, result, modulus);
    }
}

// Computes the sum of a set of private inputs using the Sum Check protocol
void sum_check_protocol(mpz_t result, mpz_t *inputs, int num_inputs, mpz_t modulus) {
    gmp_randstate_t rand_state;
    gmp_randinit_mt(rand_state);
    gmp_randseed_ui(rand_state, time(NULL));

    // Choose a random x value
    mpz_t x;
    mpz_init(x);
    mpz_urandomm(x, rand_state, modulus);

    // Compute polynomial coefficients
    mpz_t *coefficients = (mpz_t *)malloc(num_inputs * sizeof(mpz_t));
    for (int i = 0; i < num_inputs; i++) {
        mpz_init(coefficients[i]);
        mpz_set_ui(coefficients[i], 0);
    }

    for (int i = 0; i < num_inputs; i++) {
        mpz_t term;
        mpz_init(term);
        mpz_set_ui(term, inputs[i]);

        for (int j = num_inputs - 1; j > i; j--) {
            mpz_mul(term, term, x);
            mpz_mod(term, term, modulus);
        }

        mpz_add(coefficients[i], coefficients[i], term);
        mpz_mod(coefficients[i], coefficients[i], modulus);
    }

    // Evaluate polynomial at x
    mpz_t sum;
    mpz_init(sum);
    evaluate_polynomial(sum, coefficients, num_inputs - 1, x, modulus);

    // Send sum to verifier
    mpz_set_ui(result, sum);

    // Cleanup
    for (int i = 0; i < num_inputs; i++) {
        mpz_clear(coefficients[i]);
    }

    free(coefficients);
    mpz_clear(x);
    mpz_clear(sum);
}

// Example usage
int main() {
    // Set up GMP library
    mpz_t modulus;
    mpz_init(modulus);
    mpz_set_str(modulus, "340282366920938463463374607431768211297", 10); // 2^128 - 159

    // Set up inputs
    mpz_t inputs[3];
    for (int i = 0; i < 3; i++) {
        mpz_init(inputs[i]);
        mpz_set_ui(inputs[i], i + 1);
    }

    // Compute sum using Sum Check protocol
    mpz_t sum;
    mpz_init(sum);
    sum_check_protocol(sum, inputs, 3, modulus);

    // Print result
    gmp_printf("Sum: %Zd\n", sum);

    // Cleanup
    mpz_clear(sum);
    for (int i = 0; i < 3; i++) {
        mpz_clear(inputs[i]);
    }
    mpz_clear(modulus);

    return 0;
}
