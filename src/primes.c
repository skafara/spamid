/**
 * \file primes.c
 * \brief Functions declared in primes.h are implemented in this file.
 */


#include <stdlib.h>
#include <time.h>

#include "primes.h"


/** Test case 1: numbers < 2047 are tested against this test numbers subset. */
const size_t TEST_A1[] = {2};
/** Test case 2: numbers >= 2047 and < 1373653 are tested against this test numbers subset. */
const size_t TEST_A2[] = {2, 3};


/**
 * \brief Performs actual testing for primality. 
 *        If test_a is NULL, algorithm is probabilistic, else deterministic.
 * \param n Number to be tested for primality.
 * \param test_a Test numbers subset against which the provided number will be tested.
 * \param test_cnt Number of tests the number must pass to be able to be classified as a prime number.
 * \return 
 */
int mil_rab_test(const size_t n, const size_t test_a[], const size_t test_cnt) {
    size_t nm;
    size_t k, m;
    size_t b, a;
    size_t i, j;
    int possibly_prime;

    nm = n - 1;

    k = 0;
    m = nm;
    while (m % 2 == 0) {
        k++;
        m /= 2;
    }

    i = 0;
    while (i < test_cnt) {
        if (test_a) {
            a = test_a[i];
        }
        else {
            a = (rand() % (nm - 2 + 1)) + 2;
        }

        b = 1;
        for (j = 0; j < m; j++) {
            b = (b * a) % n;
        }
        if (b == 1 || b == nm) {
            i++;
            continue;
        }

        possibly_prime = 0;
        for (j = 0; j < k; j++) {
            b = (b * b) % n;

            if (b == 1) {
                return 0;
            }
            if (b == nm) {
                i++;
                possibly_prime = 1;
                break;
            }
        }
        if (possibly_prime) {
            continue;
        }

        return 0;
    }

    return 1;
}


/**
 * \brief mil_rab Evaluates how to test provided number and calls the actual testing.
 *                Correctness of 100 % for numbers < 1373653.
 *                Correctness of 99.99847412109375 % for numbers >= 1373653.
 * \param num Number to be tested for primality (> 2).
 * \return 1 if num is determined to be prime, 0 if num is definetely not prime.
 */
int mil_rab(const size_t num) {
    if (num < P_TEST_N1) {
        return mil_rab_test(num, TEST_A1, P_TEST_A1_LEN);
    }
    else if (num < P_TEST_N2) {
        return mil_rab_test(num, TEST_A2, P_TEST_A2_LEN);
    }
    else {
        srand(time(NULL));
        return mil_rab_test(num, NULL, P_TEST_R_CNT);
    }
}


int is_prime(const size_t num) {
    if (num < 2) {
        return 0;
    }
    if (num == 2) {
        return 1;
    }
    if (num % 2 == 0) {
        return 0;
    }

    return mil_rab(num);
}


size_t next_prime(size_t num) {
    while (!is_prime(num)) {
        num++;
    }
    return num;
}
