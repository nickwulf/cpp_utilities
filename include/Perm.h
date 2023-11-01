#pragma once
#include <stdio.h>
#include <math.h>
#include <iostream>

class Perm {
public:
	Perm();
	Perm(int n);
	~Perm();
	int getN()const;
	int getPermNum()const;
	int getPerm(int index)const;

private:
	int n;
	int permNum;
	int* perms;

	void makePermsStarter();
	void makePerms(int level, bool* used, int creation, int& index);

};

inline int Perm::getN()const {
	return n;
}

inline int Perm::getPermNum()const {
	return permNum;
}

inline int Perm::getPerm(int index)const {
	return perms[index];
}
