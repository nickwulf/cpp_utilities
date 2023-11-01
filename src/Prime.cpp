#include "Prime.h"

Prime::Prime() {
	init(20000000);
}

Prime::Prime(int maxPrimeVal) {
	init(maxPrimeVal);
}

Prime::~Prime() {
	free(primes);
}

bool Prime::isPrime(int integer) {
	int minIndex = 0;
	int maxIndex = maxCount - 1;
	while (minIndex != maxIndex) {
		int testIndex = (maxIndex + minIndex) >> 1;
		if (primes[testIndex] < integer) minIndex = testIndex + 1;
		else maxIndex = testIndex;
	}
	if (primes[minIndex] == integer) return true;
	else return false;
}

void Prime::init(int maxPrimeVal) {
	this->maxPrimeVal = maxPrimeVal;
	// Filter out composite numbers
	int maxIndex = ((maxPrimeVal-3) >> 1) + 1;
	bool* isPrimes = (bool*)malloc(maxIndex*sizeof(bool));
	for (int i=0; i<maxIndex; i++) isPrimes[i] = true;
	int primeNum = 0;
	int index = -1;
	while (pow(primeNum,2.0) < maxPrimeVal) {
		while (!isPrimes[++index]);
		primeNum = (index<<1) + 3;
		int stepper = primeNum * primeNum;
		while (stepper < maxPrimeVal) {
			isPrimes[(stepper-3)>>1] = false;
			stepper += primeNum << 1;
		}
	}
	// Convert filtered list of values into list of primes
	int primeCt = 1;  // Starts at one to account for 2 as a prime
	for (int i=0; i<maxIndex; i++) if (isPrimes[i]) primeCt++;
	maxCount = primeCt;
	primes = (int*)malloc(primeCt*sizeof(int));
	index = -1;
	primes[0] = 2;
	for (int i=1; i<primeCt; i++) {
		while (!isPrimes[++index]);
		primes[i] = (index<<1) + 3;
	}
	free(isPrimes);
}

