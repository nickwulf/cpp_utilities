#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>
#include "Prime.h"

class Factor {
public:
	Factor();
	//Factor(Prime* p);
	Factor(int integer, Prime* p = NULL);
	~Factor();
	void setInteger(int integer);
	int getPFact(int index);
	int getPPower(int index);
	int getPCount();
	int getFact(int index);
	int getFCount();
	void findPrimeFacts();
	void findFacts();
	int getTotient();
	int getGCD(int b);
	static int getGCD(int a, int b);
	//bool isPFactor(int pFactor);  // Checks within pFacts
	//bool isFactor(int factor);  // Checks within facts

private:
	static Prime* p;  // Used so primes don't need to be recalculated
	int integer;
	int* pFacts;   // List of prime factors of integer
	int* pPowers;  // Number of each prime factor of integer
	int pCount;    // Number of unique prime factors of integer
	int* facts;     // List of all factors of integer
	int fCount;    // Number of factors for integer
};

inline int Factor::getPFact(int index) {
	return pFacts[index];
}

inline int Factor::getPPower(int index) {
	return pPowers[index];
}

inline int Factor::getPCount() {
	return pCount;
}

inline int Factor::getFact(int index) {
	return facts[index];
}

inline int Factor::getFCount() {
	return fCount;
}
