/**
 * \file primes.h
 * \brief Header file related to number primality test and prime numbers generation.
 *
 * Used for testing of number primality and generating prime numbers.
 * Operations are based on Miller-Rabin probabilistic algorithm.
 * Operations are optimized and 100 % correct (not probabilistic) for numbers < 1373653.
 * For numbers >= 1373653 the operations use the probabilistic approach
 * with 99.99847412109375 % accuracy of correct classification.
 */


#ifndef PRIMES_H
#define PRIMES_H

#include <stddef.h>


/**< Test case 1: numbers < 2047. */
#define P_TEST_N1 2047
/**< Test case 1: test subset length 1. */
#define P_TEST_A1_LEN 1
/**< Test case 2: numbers >= 2047 and < 1373653. */
#define P_TEST_N2 1373653
/**< Test case 2: test subset length 2. */
#define P_TEST_A2_LEN 2
/**< Test case 3: test subset length for numbers >= 1373653. */
#define P_TEST_R_CNT 8


/**
 * \brief is_prime Determines whether a number is prime.
 *                 Correctness of 100 % for numbers < 1373653.
 *                 Correctness of 99.99847412109375 % for numbers >= 1373653.
 * \param num Number to be tested for primality.
 * \return 1 if num is determined to be prime, 0 if num is definetely not prime.
 */
int is_prime(const size_t num);


/**
 * \brief next_prime Returns next prime number that is greater than or equal to provided number.
 *                   Uses the is_prime function for primality testing.
 * \param num Number used to find out the next prime number.
 * \return Lowest prime number >= num.
 */
size_t next_prime(size_t num);


#endif
