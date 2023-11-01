#include "Perm.h"

Perm::Perm() {
	n = 9;
	makePermsStarter();
}

Perm::Perm(int n) {
	this->n = n;
	makePermsStarter();
}

Perm::~Perm() {
	free(perms);
}

void Perm::makePermsStarter() {
	permNum = 1;
	for (int i=1; i<=n; i++) permNum *= i;
	perms = (int*)calloc(permNum, sizeof(int));
	bool* used = (bool*)malloc((n+1)*sizeof(bool));
	for (int i=0; i<=n; i++) used[i] = false;
	int index = 0;
	makePerms(1, used, 0, index);
}

void Perm::makePerms(int level, bool* used, int creation, int& index) {
	for (int i=1; i<=n; i++) {
		if (used[i]) continue;
		used[i] = true;
		if (level == n) perms[index++] = creation*10+i;
		else makePerms(level+1, used, creation*10+i, index);
		used[i] = false;
	}
}

