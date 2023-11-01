#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>

using namespace std;

class Prime {
public:
	Prime();
	Prime(int maxPrimeVal);
	~Prime();
	int getPrime(int index);
	int getMaxCount();
	bool isPrime(int integer);
private:
	int maxPrimeVal;
	int maxCount;
	int* primes;

	void init(int maxPrimeVal);
};

inline int Prime::getPrime(int index) {
	return primes[index];
}

inline int Prime::getMaxCount() {
	return maxCount;
}
