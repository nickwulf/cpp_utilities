#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>

// PIECESIZE is an integer 1-9
#define PIECESIZE 9
// PIECEEXPMIN is a negative integer
#define PIECEEXPMIN -10

class Dec {
public:
	Dec();
	Dec(long long other);
	Dec(const Dec& other);
	Dec(double other, int sigFig);
	~Dec();
	void print()const;
	Dec& operator=(const Dec& other);
	Dec& operator=(const long long& other);
	Dec& operator+=(const Dec& other);
	Dec& operator-=(const Dec& other);
	Dec& operator*=(const Dec& other);
	Dec& operator/=(const Dec& other);
	Dec& operator%=(const Dec& other);
	Dec modOff(const Dec& other);
	const Dec operator-()const;
	const Dec operator+(const Dec& other)const;
	const Dec operator-(const Dec& other)const;
	const Dec operator*(const Dec& other)const;
	const Dec operator/(const Dec& other)const;
	const Dec operator%(const Dec& other)const;
	bool operator==(const Dec& other)const;
	bool operator!=(const Dec& other)const;
	bool operator<(const Dec& other)const;
	bool operator>(const Dec& other)const;
	bool operator<=(const Dec& other)const;
	bool operator>=(const Dec& other)const;
	int calcDigitSum()const;
	bool getPositive()const;
	int getPieceNum()const;
	int getPiece(int index)const;
	int toInt()const;
	void setToFloor();  // Removes decimal places from *this
	static int getPieceWorth();
	static void setPieceMagnitude(int magnitude);
	static void setMinSubDigits(int digits);

private:
	static int pieceSize;  // How many digits per piece
	static int pieceWorth;  // Maximum value + 1 of each piece
	static int minSubDigits;  // Number must retain at least these many digits of precision after decimal
	static int pieceExpMin;  // Minimum value of pieceExp (precision)
	static bool settingsLocked;  // Locks in static values above
	bool positive;  // False if < 0
	int pieceMax;  // Pieces allocated for in pieces
	int pieceNum;  // Pieces actually used
	int* pieces;  // Least significant pieces first, Pieces represent 9 decimal digits
	int pieceExp;  // Number of implicit beginning pieces (negative for fractions)

	void shiftPiecesRight(int shift);
	void shiftPiecesLeft(int shift);
	void clean();  // Removes excess precision and trailing low sig zeros

	// These methods assume the positive members have been taken into account
	Dec& helpAdd(const Dec& other);
	Dec& helpSub(const Dec& other);
	bool helpLessThan(const Dec& other)const;
	bool helpGrtThan(const Dec& other)const;

};

inline bool Dec::getPositive()const {
	return positive;
}

inline int Dec::getPieceNum()const {
	return pieceNum;
}

inline int Dec::getPiece(int index)const {
	return pieces[index];
}

inline int Dec::getPieceWorth() {
	return pieceWorth;
}
