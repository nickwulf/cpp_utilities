#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include "Prime.h"
#include "Factor.h"
#include "Dec.h"
#include "Perm.h"

using namespace std;

#define TEST_PERM true
#define TEST_DEC true
#define TEST_PRIME true

int main() {

	srand((unsigned int)time(NULL));
	srand(0);
	clock_t tStart, tEnd;
	long long check;

	if (TEST_PERM) {
		// Check Perm operations
		printf("Finding all permutations of the numbers 1-9...................");
		tStart = clock();
		Perm m(9);
		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		check = 0;
		for (int i=0; i<m.getPermNum(); i++) check += m.getPerm(i);
		if (check != 201599999798400) printf("ERROR: Incorrect Perm operations!\n");
	}

	if (TEST_DEC) {
		Dec::setPieceMagnitude(9);
		Dec::setMinSubDigits(20);

		// Check Dec operations
		printf("Computing sum((10^1000)*i-((10^1000)-1)*i) for i < 100,000....");
		Dec sum;
		Dec ten2Power;
		ten2Power = 1;
		int power = 1000;
		for (int i=0; i<power; i++) ten2Power *= 10;

		tStart = clock();
		for (int i=0; i<100000; i++) sum += (ten2Power+1)*i - (ten2Power)*i;

		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		check = 0;
		for (int i=0; i<100000; i++) check += i;
		if (sum != check) printf("ERROR: Incorrect Dec operations!\n");

		// Check Dec divide
		printf("Computing product and then quotient of 500 random shorts......");
		Dec prod(1);
		Dec numList[1000];
		for (int i=0; i<1000; i++) numList[i] = rand();

		tStart = clock();
		for (int i=0; i<500; i++) prod *= numList[i];
		for (int i=0; i<500; i++) prod /= numList[i];
		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		if (prod != 1) printf("ERROR: Incorrect Dec operations!\n");

		// Dec monster;
		// monster = 1;
		// for (int i=0; i<46; i++) monster *= 2;
		// for (int i=0; i<20; i++) monster *= 3;
		// for (int i=0; i<9; i++) monster *= 5;
		// monster *= (long long) pow(7,6);
		// monster *= 11 * 11;
		// monster *= 13 * 13 * 13;
		// monster *= 17 * 19 * 23;
		// monster *= 29 * 31 * 41;
		// monster *= 47 * 59 * 71;
		// printf("Monster: ");
		// Dec pow12 = 1;
		// for (int i=0; i<50; i++) pow12 *= 12;
		// Dec test = 12;
		// for (int i=0; i<50; i++) {
		// 	Dec digit;
		// 	digit = (monster / pow12);
		// 	digit.setToFloor();
		// 	printf("%i\n", digit.toInt());
		// 	monster %= pow12;
		// 	pow12 /= 12;
		// }
		// monster.print();
		// printf("\n");

	}

	if (TEST_PRIME) {
		// Check prime sieve
		printf("Finding all primes under 100,000,000..........................");
		tStart = clock();
		Prime p(100000000);
		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		check = 0;
		for (int i=0; i<p.getMaxCount(); i++) check += p.getPrime(i);
		if (check != 279209790387276) printf("ERROR: Incorrect prime sieve!\n");

		// Check primality tester
		printf("Checking the primality of all numbers under 10,000,000........");
		tStart = clock();
		for (int i=0; i<10000000; i++) p.isPrime(i);
		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		check = 0;
		for (int i=0; i<10000000; i++) if (p.isPrime(i)) check++;
		if (check != 664579) printf("ERROR: Incorrect primality test!\n");

		// Check factor finder
		printf("Finding prime factors of all numbers under 1,000,000..........");
		tStart = clock();
		Factor f(0);
		for (int i=0; i<1000000; i++) {
			f.setInteger(i);
			f.findPrimeFacts();
		}
		tEnd = clock();
		printf(" %.2f seconds\n", ((float)tEnd-tStart)/CLOCKS_PER_SEC);

		for (int i=1; i<1000000; i++) {
			check = 1;
			f.setInteger(i);
			f.findPrimeFacts();
			for (int j=0; j<f.getPCount(); j++) check *= (long long)pow(f.getPFact(j),(double)f.getPPower(j));
			if (check != i) {
				printf("ERROR: Incorrect prime factor finder %i!\n", i);
				break;
			}
		}
	}

}