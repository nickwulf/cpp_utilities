#include <cstring>

#include "Dec.h"

int Dec::pieceSize = 9;
int Dec::pieceWorth = 1000000000;
int Dec::minSubDigits = 40;
int Dec::pieceExpMin = -5;
bool Dec::settingsLocked = false;

Dec::Dec() {
	settingsLocked = true;
	pieces = NULL;
	pieceMax = 0;
	*this = 0;
}

Dec::Dec(long long other) {
	settingsLocked = true;
	pieces = NULL;
	pieceMax = 0;
	*this = other;
}

Dec::Dec(const Dec& other) {
	settingsLocked = true;
	pieces = NULL;
	pieceMax = 0;
	*this = other;
}

Dec::Dec(double other, int sigFig) {
	settingsLocked = true;
	int pieceExpMinTemp = pieceExpMin;
	pieceExpMin -= 100;

	// Double structure: http://steve.hollasch.net/cgindex/coding/ieeefloat.html
	if (other == 0) {  // Special double sequence (handled differently)
		*this = 0;
		return;
	}

	pieces = NULL;

	pieceMax = 32/pieceSize;
	pieceMax += 4 - pieceMax%4;
	pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
	memset(pieces, 0, pieceMax*sizeof(int));

	long long* bitView = (long long*)&other;
	if (*bitView & 0x8000000000000000) positive = false;
	else positive = true;
	long long exponent = *bitView & 0x7FF0000000000000;
	exponent = (exponent >> 52) - 1023;
	long long mantissa = *bitView & 0x000FFFFFFFFFFFFF;
	mantissa += 0x0010000000000000;
	exponent -= 52;  // Accounts for the undivided mantissa
	pieceNum = 0;
	while (mantissa != 0) {
		pieces[pieceNum] = (int)(mantissa % pieceWorth);
		mantissa /= pieceWorth;
		pieceNum++;
	}
	pieceExp = 0;
	if (exponent > 0) {  // Need to multiply by 2^exponent
		Dec dTemp(0x0004000000000000);  // 2^50
		while (exponent >= 50) {
			*this *= dTemp;
			exponent -=50;
		}
		*this *= (long long)pow(2,(double)(exponent));
	} else if (exponent < 0) {  // Divide by 2 by dividing by 10 and multiply by 5
		exponent = -exponent;
		pieceExp -= (int)ceil(exponent/(double)pieceSize);
		*this *= (long long)pow(10,(double)(-pieceExp*pieceSize-exponent));
		Dec dTemp((long long)pow(5,20.0));  // 5^20
		while (exponent >= 20) {
			*this *= dTemp;
			exponent -=20;
		}
		*this *= (long long)pow(5,(double)(exponent));
	}
	// Chop off residuals based on sigFig
	int digitNum = (pieceNum-1)*pieceSize + (int)floor(log10((double)pieces[pieceNum-1]))+1;  // Assumes pieces[pieceNum-1] != 0
	int zeroNum = digitNum - sigFig;
	int zeroPieces = zeroNum/pieceSize;
	int zeroExtra = zeroNum%pieceSize;
	if (zeroNum >= digitNum) {
		*this = 0;
		return;
	} else if (zeroNum > 0) {
		bool roundUp = false;
		if (zeroExtra == 0) {
			int filterLow = (int)pow(10.0, pieceSize-1);  // Filters out everything under most sig digit
			if ((pieces[zeroPieces-1]/filterLow)%10 >= 5) roundUp = true;
		} else {
			int filterLow = (int)pow(10.0, zeroExtra - 1);  // Filters out everything under digit
			if ((pieces[zeroPieces]/filterLow)%10 >= 5) roundUp = true;
			pieces[zeroPieces] -= pieces[zeroPieces] % (filterLow*10);
		}
		shiftPiecesLeft(zeroPieces);
		if (roundUp) {
			Dec dAdd = (int)pow(10.0, zeroExtra);
			dAdd.pieceExp = pieceExp;
			*this += dAdd;
		}
	}

	pieceExpMin = pieceExpMinTemp;
	clean();
}

Dec::~Dec(void) {
	realloc(pieces, 0);
}

void Dec::print()const {
	if (pieceNum == 0) {
		printf("0");
	} else {
		if (!positive) printf("-");
		// Print pieces before decimal
		int posNum = pieceNum+pieceExp;
		if (posNum > 0) {
			for (int i=pieceNum-1; i>=-pieceExp; i--) {
				if (i == pieceNum-1) printf("%i", pieces[i]);
				else if (i >= 0) printf("%.*i", pieceSize, pieces[i]);
				else printf("%.*i", pieceSize, 0);
			}
		} else printf("0");
		// Print pieces after decimal
		if (pieceExp < 0) {
			printf(".");
			int zeroNum = 0;
			int zeroVal = 10;
			while (pieces[0]%zeroVal == 0) {
				zeroNum++;
				zeroVal *= 10;
			}
			zeroVal /= 10;
			for (int i=-pieceExp-1; i>=0; i--) {
				if (i < 0) printf("%.*i", pieceSize-zeroNum, pieces[i]/zeroVal);
				else printf("%.*i", pieceSize, pieces[i]);
			}
		}
	}
}

Dec& Dec::operator=(const Dec& other) {
	if (this == &other) return *this;
	if (other.pieceNum > pieceMax) {  // Does in have more contents than we have storage for?
		pieceMax = other.pieceMax;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
	}
	memset(pieces, 0, pieceMax*sizeof(int));
	pieceNum = other.pieceNum;
	for (int i=0; i<pieceNum; i++) pieces[i] = other.pieces[i];
	positive = other.positive;
	pieceExp = other.pieceExp;
	pieceExpMin = other.pieceExpMin;

	clean();
	return *this;
}

Dec& Dec::operator=(const long long& other) {
	if (pieceMax < 24/pieceSize) {
		pieceMax = 24/pieceSize;
		pieceMax += 4 - pieceMax%4;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
	}
	memset(pieces, 0, pieceMax*sizeof(int));

	long long integer = (long long)other;
	if (integer <= 0) {
		positive = false;
		integer = -integer;
	} else positive = true;
	pieceExp = 0;

	pieceNum = 0;
	while (integer != 0) {
		pieces[pieceNum] = (int)(integer % pieceWorth);
		integer /= pieceWorth;
		pieceNum++;
	}

	clean();
	return *this;
}

Dec& Dec::operator+=(const Dec& other) {
	if (this == &other) return *this;
	if (positive == other.positive) return helpAdd(other);
	else return helpSub(other);
}

Dec& Dec::operator-=(const Dec& other) {
	if (this == &other) {
		*this = 0;
		return *this;
	}
	if (positive == other.positive) return helpSub(other);
	else return helpAdd(other);
}

Dec& Dec::operator*=(const Dec& other) {
	if (this == &other) *this = other;
	pieceMax = pieceNum + other.pieceNum + 1;
	pieceMax += 4 - pieceMax%4;
	int* answer = (int*)calloc(pieceMax, sizeof(int));
	long long product;
	int place;
	for (int i=0; i<pieceNum; i++) {
		for (int j=0; j<other.pieceNum; j++) {
			product = (long long)pieces[i] * other.pieces[j];
			place = i+j;
			while (product != 0) {
				answer[place] += (int)(product%pieceWorth);
				product /= pieceWorth;  // Possible optimization here by combining % and /
				if (answer[place] >= pieceWorth) {
					answer[place] -= pieceWorth;
					product++;
				}
				place++;
			}
		}
	}
	realloc(pieces, 0);
	pieces = answer;
	if (positive == other.positive) positive = true;
	else positive = false;
	pieceExp += other.pieceExp;

	pieceNum = pieceMax;
	while (--pieceNum >= 0 && pieces[pieceNum] == 0);  // Find where pieceNum is
	pieceNum++;

	clean();
	return *this;
}

Dec& Dec::operator/=(const Dec& other) {
	if (this == &other) {
		*this = 1;
		return *this;
	}
	// Uses Newton-Rhapson method: http://en.wikipedia.org/wiki/Division_%28digital%29
	if (other.pieceNum == 0) {
		printf("ERROR: Divide by zero!\n");
		return *this;
	}
	if (pieceNum == 0) return *this;

	// Determine how many pieces will be needed
	int posNum = pieceNum+pieceExp - (other.pieceNum+other.pieceExp) + 1;
	int divPieceMax = posNum - pieceExpMin + (other.pieceNum+other.pieceExp);  // Add an extra other.Num+Exp for extra precision while dividing
	if (divPieceMax < 2) {
		*this = 0;
		return *this;
	}
	int pieceExpMinTemp = pieceExpMin;
	pieceExpMin = -divPieceMax;

	double initial = other.pieces[other.pieceNum-1];
	initial *= pieceWorth;
	if (other.pieceNum > 1) initial += other.pieces[other.pieceNum-2];
	initial = 1 / initial;
	if (initial == 1) initial = 0.99;  // This is actually impossible I think
	Dec xNext(initial, 100);  // This must be formated to match exponent of other
	xNext.pieceExp += 2 - (other.pieceNum+other.pieceExp);
	Dec xNow;
	while(xNow != xNext) {
		xNow = xNext;
		xNext *= other;
		// Algorithm messes up when other isn't positive so need to modify for this case
		xNext.positive ^= !other.positive;
		xNext.positive = !xNext.positive;
		xNext += 2;
		xNext *= xNow;
	}
	*this *= xNext;
	// Reset pieceExpMin and round accordingly
	pieceExpMin = pieceExpMinTemp;
	if (pieceExp < pieceExpMin) {
		if (pieces[pieceExpMin-pieceExp-1] >= pieceWorth/2) {
			Dec dAdd(1);
			dAdd.pieceExp = pieceExpMin;
			*this += dAdd;
		}
	}
	positive ^= !other.positive;
	clean();
	return *this;
}

Dec& Dec::operator%=(const Dec& other) {
	if (this == &other) {
		*this = 0;
		return *this;
	}
	if (pieceNum == 0) return *this;
	Dec quotient = *this/other;
	quotient.setToFloor();
	*this -= quotient*other;
	clean();
	return *this;
}

Dec Dec::modOff(const Dec& other) {
	if (pieceNum == 0) return *this;
	Dec quotient = *this/other;
	quotient.setToFloor();
	Dec answer = *this;
	answer -= quotient*other;
	*this = quotient;
	clean();
	return answer;
}

const Dec Dec::operator-()const {
	Dec temp = *this;
	temp.positive = !temp.positive;
	return temp;
}

const Dec Dec::operator+(const Dec& other)const {
	return Dec(*this) += other;
}

const Dec Dec::operator-(const Dec& other)const {
	return Dec(*this) -= other;
}

const Dec Dec::operator*(const Dec& other)const {
	return Dec(*this) *= other;
}

const Dec Dec::operator/(const Dec& other)const {
	return Dec(*this) /= other;
}

const Dec Dec::operator%(const Dec& other)const {
	return Dec(*this) %= other;
}

bool Dec::operator==(const Dec& other)const {
	if (positive != other.positive) return false;
	if (pieceNum+pieceExp != other.pieceNum+other.pieceExp) return false;
	int diff = other.pieceExp-pieceExp;
	if (diff >= 0) {
		for (int i=diff; i<pieceNum; i++) if (pieces[i] != other.pieces[i-diff]) return false;
		for (int i=0; i<diff; i++) if (pieces[i] != 0) return false;
	} else {
		for (int i=0; i<pieceNum; i++) if (pieces[i] != other.pieces[i-diff]) return false;
		for (int i=0; i<-diff; i++) if (other.pieces[i] != 0) return false;
	}
	return true;
}

bool Dec::operator!=(const Dec& other)const {
	return !(*this == other);
}

bool Dec::operator<(const Dec& other)const {
	if (positive && other.positive) return helpLessThan(other);
	else if (positive) return false;
	else if (other.positive) return true;
	else return helpGrtThan(other);
}

bool Dec::operator>(const Dec& other)const {
	if (positive && other.positive) return helpGrtThan(other);
	else if (positive) return true;
	else if (other.positive) return false;
	else return helpLessThan(other);
}

bool Dec::operator<=(const Dec& other)const {
	return !(*this > other);
}

bool Dec::operator>=(const Dec& other)const {
	return !(*this < other);
}

int Dec::calcDigitSum()const {
	int sum = 0;
	int integer;
	for (int i=0; i<pieceNum; i++) {
		integer = pieces[i];
		while (integer > 0) {
			sum += integer % 10;
			integer /= 10;
		}
	}
	return sum;
}

int Dec::toInt()const {
	if (pieceNum == 0) return 0;
	if (pieceExp+pieceNum <= 0) return 0;
	long long value = 0;
	long long scaler = 1;
	int index = 0;
	if (pieceExp < 0) index = -pieceExp;
	if (pieceExp > 0) scaler = (long long)pow((double)pieceWorth,pieceExp);
	while (value <= 2147483647 && index < pieceNum) {
		value += scaler*pieces[index];
		scaler *= pieceWorth;
		index++;
	}
	if (value > 2147483647) value = 2147483647;
	if (!positive) value = -value;
	return (int)value;
}

void Dec::setToFloor() {  // Removes decimal places from *this
	if (pieceNum == 0) return;
	bool positiveMem = positive;
	if (pieceExp < 0) shiftPiecesLeft(-pieceExp);
	clean();
	if (!positiveMem) *this -= 1;
}

void Dec::setPieceMagnitude(int magnitude) {
	if (!settingsLocked) {
		if (magnitude <= 9 && magnitude >= 1) {
			pieceSize = magnitude;
			pieceWorth = (int)pow(10.0,magnitude);
			pieceExpMin = (int)-ceil(minSubDigits/(double)pieceSize);
		}
	}
}

void Dec::setMinSubDigits(int digits){
	if (!settingsLocked) {
		if (digits >= 0) {
			minSubDigits = digits;
			pieceExpMin = (int)-ceil(minSubDigits/(double)pieceSize);
		}
	}
}

void Dec::shiftPiecesRight(int shift) {  // May grow pieceMax
	if (shift <= 0) return;
	int newPieceNum = pieceNum + shift;
	if (newPieceNum > pieceMax) {
		pieceMax = newPieceNum + 4 - newPieceNum%4;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
		for (int i=pieceNum; i<pieceMax; i++) pieces[i] = 0;
	}
	for (int i=pieceNum-1; i>=0; i--) pieces[i+shift] = pieces[i];
	memset(pieces, 0, shift*sizeof(int));
	pieceNum = newPieceNum;
	pieceExp -= shift;
}

void Dec::shiftPiecesLeft(int shift) {  // May remove low sig data
	if (shift <= 0) return;
	if (shift > pieceNum) shift = pieceNum;
	for (int i=0; i<pieceNum-shift; i++) pieces[i] = pieces[i+shift];
	memset(&pieces[pieceNum-shift], 0, shift*sizeof(int));
	pieceNum -= shift;
	pieceExp += shift;
}

void Dec::clean() {  // Removes excess precision and trailing low sig zeros
	if (pieceNum == 0) {
		positive = false;
		pieceExp = 0;
		return;
	}
	int shift = 0;
	if (pieceExp < pieceExpMin) shift = pieceExpMin - pieceExp;
	while(pieces[shift] == 0) {
		shift++;
		if (shift>=pieceNum) {
			*this = 0;
			return;
		}
	}
	shiftPiecesLeft(shift);
}

Dec& Dec::helpAdd(const Dec& other) {
	int pieceSkip = 0;
	if (other.pieceExp < pieceExp) shiftPiecesRight(pieceExp-other.pieceExp);
	else pieceSkip = other.pieceExp-pieceExp;

	if (other.pieceNum+pieceSkip >= pieceMax) {  // Does other have more contents than we have storage for?
		pieceMax = other.pieceMax+pieceSkip + 4;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
		for (int i=pieceNum; i<pieceMax; i++) pieces[i] = 0;
	} else if (pieceNum == pieceMax) {  // Is the answer potentially larger than we have storage for?
		pieceMax += 4;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
		for (int i=pieceNum; i<pieceMax; i++) pieces[i] = 0;
	}
	if (pieceNum < other.pieceNum+pieceSkip) pieceNum = other.pieceNum + pieceSkip;

	bool overflow = false;
	for (int i=pieceSkip; i<pieceNum; i++) {
		if (i-pieceSkip < other.pieceNum) pieces[i] += other.pieces[i-pieceSkip];
		if (overflow) pieces[i]++;
		if (pieces[i] >= pieceWorth) {
			pieces[i] -= pieceWorth;
			overflow = true;
		} else overflow = false;
	}
	if (overflow) {
		pieces[pieceNum]++;
		pieceNum++;
	}

	clean();
	return *this;
}

Dec& Dec::helpSub(const Dec& other) {
	int pieceSkip = 0;
	if (other.pieceExp < pieceExp) shiftPiecesRight(pieceExp-other.pieceExp);
	else pieceSkip = other.pieceExp-pieceExp;

	if (other.pieceNum+pieceSkip > pieceMax) {  // Does other have more contents than we have storage for?
		pieceMax = other.pieceMax+pieceSkip + 4;
		pieces = (int*)realloc(pieces, pieceMax*sizeof(int));
		for (int i=pieceNum; i<pieceMax; i++) pieces[i] = 0;
	}
	if (pieceNum < other.pieceNum+pieceSkip) pieceNum = other.pieceNum + pieceSkip;

	bool overflow = false;
	for (int i=pieceSkip; i<pieceNum; i++) {
		if (i-pieceSkip < other.pieceNum) pieces[i] -= other.pieces[i-pieceSkip];
		if (overflow) pieces[i]--;
		if (pieces[i] < 0) {
			pieces[i] += pieceWorth;
			overflow = true;
		} else overflow = false;
	}
	if (overflow) {  // This happens when other > this
		positive = !positive;
		int firstNZ = -1;
		while (pieces[++firstNZ] == 0);
		pieces[firstNZ] = pieceWorth - pieces[firstNZ];
		for (int i=firstNZ+1; i<pieceNum; i++) pieces[i] = pieceWorth - pieces[i] - 1;
	}
	while (--pieceNum >= 0 && pieces[pieceNum] == 0);  // Clean up empty pieces
	pieceNum++;

	clean();
	return *this;
}

bool Dec::helpLessThan(const Dec& other)const {
	if (pieceNum+pieceExp != other.pieceNum+other.pieceExp)
		return pieceNum+pieceExp < other.pieceNum+other.pieceExp;
	int diff = other.pieceExp-pieceExp;
	if (diff >= 0) {
		for (int i=pieceNum-1; i>=diff; i--) if (pieces[i] != other.pieces[i-diff]) return pieces[i] < other.pieces[i-diff];
	} else {
		for (int i=pieceNum-1; i>=0; i--) if (pieces[i] != other.pieces[i-diff]) return pieces[i] < other.pieces[i-diff];
		for (int i=0; i<-diff; i++) if (other.pieces[i] != 0) return true;
	}
	return false;
}

bool Dec::helpGrtThan(const Dec& other)const {
	if (pieceNum+pieceExp != other.pieceNum+other.pieceExp)
		return pieceNum+pieceExp > other.pieceNum+other.pieceExp;
	int diff = other.pieceExp-pieceExp;
	if (diff >= 0) {
		for (int i=pieceNum-1; i>=diff; i--) if (pieces[i] != other.pieces[i-diff]) return pieces[i] > other.pieces[i-diff];
		for (int i=0; i<diff; i++) if (pieces[i] != 0) return true;
	} else {
		for (int i=pieceNum-1; i>=0; i--) if (pieces[i] != other.pieces[i-diff]) return pieces[i] > other.pieces[i-diff];
	}
	return false;
}
