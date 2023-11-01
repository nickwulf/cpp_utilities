#include "Factor.h"

Prime* Factor::p = NULL;

Factor::Factor() {
	this->integer = 0;
	pFacts = NULL;
	pPowers = NULL;
	facts = NULL;
	setInteger(1);
}

Factor::Factor(int integer, Prime* p) {
	this->integer = 0;
	if (p != NULL) {
		if (this->p == NULL) this->p = p;
		else if (p->getMaxCount() > this->p->getMaxCount()) this->p = p;
	}
	pFacts = NULL;
	pPowers = NULL;
	facts = NULL;
	setInteger(integer);
}

Factor::~Factor() {
	free(pFacts);
	free(pPowers);
	free(facts);
}

void Factor::setInteger(int integer) {
	if (this->integer != integer && integer > 0) {
		this->integer = integer;
		pCount = 0;
		fCount = 0;
		free(pFacts);
		free(pPowers);
		free(facts);
		pFacts = NULL;
		pPowers = NULL;
		facts = NULL;
	}
}

void Factor::findPrimeFacts() {
	int pFactsTemp[100];
	int pPowersTemp[100];
	if (p == NULL) p = new Prime();
	int testPrimeIndex = 0;
	int remaining = integer;
	int testPrime = p->getPrime(testPrimeIndex);
	pCount = 0;

	while (remaining >= 2) {
		if (p->isPrime(remaining)) {
			pFactsTemp[pCount] = remaining;
			pPowersTemp[pCount] = 1;
			pCount++;
			break;
		}
		if (remaining%testPrime == 0) {
			remaining /= testPrime;
			pFactsTemp[pCount] = testPrime;
			pPowersTemp[pCount] = 1;
			while (remaining%testPrime == 0) {
				remaining /= testPrime;
				pPowersTemp[pCount]++;
			}
			pCount++;
		}
		testPrime = p->getPrime(++testPrimeIndex);
		if (testPrimeIndex == p->getMaxCount()) {
			printf("Error - Factor::findPrimeFacts - Ran out of primes!\n", integer);
			break;
		}
	}

	if (pCount > 0) {
		free(pFacts);
		free(pPowers);
		pFacts = (int*)malloc(pCount*sizeof(int));
		pPowers = (int*)malloc(pCount*sizeof(int));
		for (int i=0; i<pCount; i++) {
			pFacts[i] = pFactsTemp[i];
			pPowers[i] = pPowersTemp[i];
		}
	}
}


void Factor::findFacts() {
	if (integer < 1) {
		fCount = 0;
		return;
	} else if (integer > 1 && pCount == 0) findPrimeFacts();

	// Find total number of factors
	fCount = 1;
	for (int i=0; i<pCount; i++) fCount *= pPowers[i]+1;
	free(facts);
	facts = (int*)malloc(fCount*sizeof(int));

	// pPowersTemp is used to iterate through permutations
	int* pPowersTemp = (int*)malloc(pCount*sizeof(int));
	for (int i=0; i<pCount; i++) pPowersTemp[i] = 0;

	// Iterate through permutations of pPowersTemp
	int factIndex = 0;
	while (true) {
		int factor = 1;
		for (int i=0; i<pCount; i++) factor *= (int)pow((double)pFacts[i],pPowersTemp[i]);
		facts[factIndex] = factor;
		factIndex++;
		if (factIndex >= fCount) break;
		// Set pPowersTemp to next iteration
		int index = 0;
		while(pPowersTemp[index] == pPowers[index]) index++;
		if (index >= fCount) printf("ERROR - Factor::findFacts - Incorrect index!\n");
		for (int i=0; i<index; i++) pPowersTemp[i] = 0;
		pPowersTemp[index]++;
	}

	free(pPowersTemp);
}

int Factor::getTotient() {
	if (integer <= 0) return 0;
	if (integer == 1) return 1;
	if (integer > 1 && pCount == 0) findPrimeFacts();
	int totient = integer;
	for (int i=0; i<pCount; i++) {
		totient /= pFacts[i];
		totient *= pFacts[i] - 1;
	}
	return totient;
}

int Factor::getGCD(int b) {
	int a = integer;
	if (a == 0) return b;
	if (b == 0) return a;
	bool aGTb = a > b;
	while (a!=b) {
		if (aGTb) {
			a %= b;
			if (a == 0) a = b;
			else if (a == 1) b = 1;
		} else {
			b %= a;
			if (b == 0) b = a;
			else if (b == 1) a = 1;
		}
		aGTb = !aGTb;
	}
	return a;
}

int Factor::getGCD(int a, int b) {
	if (a == 0) return b;
	if (b == 0) return a;
	bool aGTb = a > b;
	while (a!=b) {
		if (aGTb) {
			a %= b;
			if (a == 0) a = b;
			else if (a == 1) b = 1;
		} else {
			b %= a;
			if (b == 0) b = a;
			else if (b == 1) a = 1;
		}
		aGTb = !aGTb;
	}
	return a;
}
