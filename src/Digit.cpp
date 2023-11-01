#include <cstring>

#include "Digit.h"

Digit::Digit() {
	digitMax = 16;
	digitNum = 0;
	digits = (char*)calloc(digitMax, sizeof(char));
	for(int i=0; i<10; i++) contents[i] = false;
}

Digit::Digit(long long integer) {
	digitMax = 0;
	*this = integer;
}

Digit::Digit(const Dec& d) {
	digitMax = 0;
	*this = d;
}

Digit::Digit(const Digit& other) {
	digitMax = 0;
	*this = other;
}

Digit::~Digit() {
	free(digits);
}

Digit& Digit::operator=(long long integer) {
	if (integer == 0) digitNum = 1;
	else digitNum = 1 + (int)floor(log10((float)integer));
	if (digitMax < digitNum) {
		if (digitMax != 0) free(digits);
		digitMax = digitNum - (digitNum%16) + 16;
		digits = (char*)calloc(digitMax, sizeof(char));
	} else memset(digits, 0, digitMax*sizeof(char));
	for(int i=0; i<10; i++) contents[i] = false;
	for (int i=0; i<digitNum; i++) {
		digits[i] = (char)(integer%10);
		contents[digits[i]] = true;
		integer /= 10;
	}
	return *this;
}

Digit& Digit::operator=(const Dec& d) {
	int pieceNum = d.getPieceNum();
	if (pieceNum == 0) digitNum = 1;
	else digitNum = 9*(pieceNum-1) + 1 + (int)floor(log10((float)d.getPiece(pieceNum-1)));
	if (digitMax < digitNum) {
		if (digitMax != 0) free(digits);
		digitMax = digitNum - (digitNum%16) + 16;
		digits = (char*)calloc(digitMax, sizeof(char));
	} else memset(digits, 0, digitMax*sizeof(char));
	for(int i=0; i<10; i++) contents[i] = false;
	for (int i=0; i<pieceNum; i++) {
		int integer = d.getPiece(i);
		int index = i*9;
		while (integer > 0) {
			digits[index] = (char)(integer%10);
			contents[digits[index]] = true;
			integer /= 10;
			index++;
		}
	}
	return *this;
}

Digit& Digit::operator=(const Digit& other) {
	if (this == &other) return *this;
	if (other.digitNum > digitMax || digitMax == 0) {  // Does other have more contents than we have storage for?
		if (digitMax != 0) free(digits);
		digitMax = other.digitMax;
		digits = (char*)calloc(digitMax, sizeof(char));
	} else memset(digits, 0, digitMax*sizeof(char));
	digitNum = other.digitNum;
	for (int i=0; i<digitNum; i++) digits[i] = other.digits[i];
	return *this;
}

Digit& Digit::operator+=(const Digit& other) {
	int totalNum = digitNum + other.digitNum;
	if (totalNum > digitMax) {
		digitMax = totalNum - (totalNum%16) + 16;
		char* newDigits = (char*)calloc(digitMax, sizeof(char));
		for (int i=0; i<digitNum; i++) newDigits[i+other.digitNum] = digits[i];
		free(digits);
		digits = newDigits;
	} else {
		for (int i=digitNum-1; i>=0; i--) digits[i+other.digitNum] = digits[i];
	}
	for (int i=0; i<other.digitNum; i++) digits[i] = other.digits[i];
	digitNum = totalNum;
	for (int i=0; i<10; i++) contents[i] = contents[i] || other.contents[i];
	return *this;
}

void Digit::swap(int a, int b) {
	if (a>=0 && a<digitNum && b>=0 && b<digitNum) {
		char temp = digits[a];
		digits[a] = digits[b];
		digits[b] = temp;
	} else printf("ERROR - Digit::switch - Index out of bounds!\n");
}

int Digit::calcSum() {
	int sum = 0;
	for (int i=0; i<digitNum; i++) sum += digits[i];
	return sum;
}

long long Digit::genValueL() {
	long long result = 0;
	for (int i=digitNum-1; i>=0; i--) {
		result *= 10;
		result += digits[i];
	}
	return result;
}

Dec Digit::genValueD() {
	Dec d;
	int integer = 0;
	bool first = true;
	int offset = digitNum%9;
	for (int i=digitNum-1; i>=0; i--) {
		integer *= 10;
		integer += digits[i];
		if ((digitNum-i)%9 == offset) {
			if (!first) d *= Dec::getPieceWorth();
			else first = false;
			d += integer;
			integer = 0;
		}
	}

	return d;
}
