#include <bits/stdc++.h>
using namespace std;

#define rand RAND
#define Z 16
int cnt[471], cur, cnt2[471];
char buf[471], buf2[471], buf3[471], buf4[471], SS[471] = "KQEECCSSGGNNRRBB", S[2][11] = {"KQECSGNRBP", "kqecsgnrbp"};
mt19937 rand(chrono::system_clock::now().time_since_epoch().count());

void show();
void doopt(int);
void doplace1(int);
void doplace2(int);
void domove(int);
void doup(int);
void doyes(int);

void simplehint(const char *S) {
	printf("---------------------------------------\n%s", S);
}

#define hint(...) sprintf(buf, __VA_ARGS__); simplehint(buf)

int tot;
class piece {
protected:
	int z, x, y, id, removed;
	char t;
public:
	piece(int Y, int X, int zz, char T='.') {
		id = ++tot;
		y = Y;
		x = X;
		z = zz;
		t = T;
		removed = 0;
	}
	virtual ~piece() {
	}
	virtual void print() {
		printf("%c%c", getS() == '.' ? ' ' : getS(), t == '.' ? ' ' : t);
	}
	virtual int canmove() {
		return z == cur || z == 3;
	}
	virtual int cantarget() {
		return z != cur && z != 3;
	}
	virtual void askpromote(int flag) {
		hint("%spromote? [Y]es/[N]o: ", flag == 0 ? "" : "bad option. ");
		fgets(buf, 471, stdin);
		sscanf(buf, "%1s", buf2);
		buf2[0] = toupper(buf2[0]);
		if (buf2[0] != 'Y' && buf2[0] != 'N') {
			return askpromote(1);
		}
		if (buf2[0] == 'Y') {
			dopromote();
		} else {
			t = '.';
		}
	}
	virtual void dopromote() {
		t = '+';
	}
	virtual void askmove(int flag);
	virtual void domove(int Y, int X);
	virtual void doremove(piece *src, int recycle);
	virtual char getS() = 0;
	virtual int badmove(int ty, int tx) = 0;
} *a[Z + 1][Z + 1];

piece *getpiece(int y, int x, char s='.', char t='.');

void piece::askmove(int flag) {
	hint("%sinput final location: ", flag == 0 ? "" : flag == 1 ? "bad format. " : flag == 2 ? "location cannot be targetted by you. " : "location cannot be targetted by the piece. ");
	fgets(buf, 471, stdin);
	int tx, ty;
	sscanf(buf, "%1s%d", buf2, &ty);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return ::domove(0);
	}
	if (buf2[0] < 'A' || buf2[0] > 'A' + Z - 1 || ty < 1 || ty > Z) {
		return askmove(1);
	}
	tx = buf2[0] - 'A' + 1;
	if (!a[ty][tx]->cantarget()) {
		return askmove(2);
	}
	if (badmove(ty, tx)) {
		return askmove(3);
	}
	domove(ty, tx);
}

void piece::domove(int Y, int X) {
	a[Y][X]->doremove(this, 1);
	if (removed) {
		return;
	}
	a[y][x] = getpiece(y, x);
	y = Y;
	x = X;
	a[y][x] = this;
	if (t == '.' && (cur ? y <= 3 : y >= Z - 2)) {
		t = '?';
		askpromote(0);
	}
}

void piece::doremove(piece *src, int recycle) {
	if (t == '-') {
		cnt[z ? 'P' : 'p']--;
	} else {
		cnt[getS()]--;
	}
	if (recycle) {
		if (t == '-') {
			cnt2[z ? 'p' : 'P']++;
		} else {
			cnt[getS() ^ 'a' ^ 'A']++;
		}
	}
	a[y][x] = getpiece(y, x);
	removed = 1;
	delete this;
}

class space : public piece {
public:
	space(int Y, int X) : piece(Y, X, 2) {
	}
	virtual char getS() {
		return '.';
	}
	virtual int badmove(int ty, int tx) {
		assert(0);
	}
};

class pawn : public piece {
public:
	pawn(int Y, int X, int z) : piece(Y, X, z) {
	}
	virtual char getS() {
		return z ? 'p' : 'P';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x, dy2 = dy * (cur ? -1 : 1);
		return !(dy2 == 1 && abs(dx) <= 1 && (dx ? a[ty][tx]->getS() != '.' : a[ty][tx]->getS() == '.'));
	}
	virtual void askpromote(int flag) {
		hint("%spromote to what? e[X]it/[B]ishop/[C]annon/[E]xplosive/[G]old/k[N]ight/[O]bstacle/[Q]ueen/[R]ook/[S]ilver: ", flag == 0 ? "" : "bad piece. ");
		fgets(buf, 471, stdin);
		sscanf(buf, "%1s", buf2);
		buf2[0] = toupper(buf2[0]);
		if (buf2[0] == 'X') {
			t = '.';
			return;
		}
		if (buf2[0] != 'B' && buf2[0] != 'C' && buf2[0] != 'E' && buf2[0] != 'G' && buf2[0] != 'N' && buf2[0] != 'O' && buf2[0] != 'Q' && buf2[0] != 'R' && buf2[0] != 'S') {
			return askpromote(1);
		}
		delete this;
		a[y][x] = getpiece(y, x, cur ? tolower(buf2[0]) : buf2[0], '-');
	}
};

class explosive : public piece {
public:
	explosive(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'e' : 'E';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		return !dx && !dy || max(abs(dx), abs(dy)) >= 2;
	}
	virtual void domove(int Y, int X) {
		if (a[Y][X]->getS() != '.' && t != '+') {
			a[Y][X]->doremove(this, 1);
			if (!removed) {
				doremove(NULL, 1);
			}
			return;
		}
		piece::domove(Y, X);
	}
	virtual void doremove(piece *src, int recycle) {
		piece::doremove(src, recycle);
		if (src) {
			src->doremove(NULL, 0);
		}
	}
};

class queen : public piece {
public:
	queen(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'q' : 'Q';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		if (!dy && !dx) {
			return 1;
		}
		if ((abs(dy) == 2 && abs(dx) == 1 || abs(dy) == 1 && abs(dx) == 2) && t == '+') {
			return 0;
		}
		if (dy && dx && abs(dy) != abs(dx)) {
			return 1;
		}
		int zy = dy ? dy / abs(dy) : 0, zx = dx ? dx / abs(dx) : 0, cnt = 0, yy = y, xx = x;
		while (yy != ty || xx != tx) {
			cnt += (a[yy][xx]->getS() != '.');
			yy += zy;
			xx += zx;
		}
		return cnt != 1;
	}
};

class cannon : public piece {
public:
	cannon(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'c' : 'C';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		if (!dy && !dx || dy && dx && abs(dy) != abs(dx)) {
			return 1;
		}
		int zy = dy ? dy / abs(dy) : 0, zx = dx ? dx / abs(dx) : 0, cnt = 0, yy = y, xx = x;
		while (yy != ty || xx != tx) {
			cnt += (a[yy][xx]->getS() != '.');
			yy += zy;
			xx += zx;
		}
		if (dx && dy && t != '+') {
			return 0;
		}
		return a[ty][tx]->getS() == '.' ? cnt != 1 : cnt != 2;
	}
};

class silver : public piece {
public:
	silver(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 's' : 'S';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x, dy2 = dy * (cur ? -1 : 1);
		return t ? !(dy2 == 1 && abs(dx) <= 1 || dy2 == 0 && abs(dx) == 1 || dy2 == -1 && dx == 0) : !(dy2 == 1 && abs(dx) <= 1 || dy2 == -1 && abs(dx) == 1);
	}
};

class gold : public piece {
public:
	gold(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'g' : 'G';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x, dy2 = dy * (cur ? -1 : 1);
		return t ? !(dy2 == 1 && abs(dx) <= 1 || dy2 == -1 && abs(dx) == 1) : !(dy2 == 1 && abs(dx) <= 1 || dy2 == 0 && abs(dx) == 1 || dy2 == -1 && dx == 0);
	}
};

class knight : public piece {
public:
	knight(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'n' : 'N';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		return !(abs(dy) == 2 && abs(dx) == 1 || abs(dx) == 1 && abs(dy) == 2);
	}
	virtual void dopromote() {
		piece::dopromote();
		askmove(0);
	}
};

class obstacle : public piece {
public:
	obstacle(int Y, int X, int z) : piece(Y, X, z) {
	}
	virtual char getS() {
		return z ? 'o' : 'O';
	}
	virtual int badmove(int ty, int tx) {
		return true;
	}
};

class king : public piece {
public:
	king(int Y, int X, int z) : piece(Y, X, z) {
	}
	virtual char getS() {
		return z ? 'k' : 'K';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		return !dy && !dx || abs(dy) > 2 || abs(dx) > 2;
	}
	virtual void doremove(piece *src, int recycle) {
		if (cnt2[getS()] == 1) {
			hint("%s won.\n", cur ? "black" : "white");
			exit(0);
		}
		piece::doremove(src, recycle);
	}
	virtual void askpromote(int flag) {
		t = '.';
	}
};

class rook : public piece {
public:
	rook(int Y, int X, int z, char t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'r' : 'R';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		if (!dx && !dy || dx && dy) {
			return 1;
		}
		int zy = dy ? dy / abs(dy) : 0, zx = dx ? dx / abs(dx) : 0, cnt = 0, yy = y, xx = x;
		while (yy != ty || xx != tx) {
			cnt += (a[yy][xx]->getS() != '.');
			yy += zy;
			xx += zx;
		}
		return cnt != 1;
	}
};

class bishop : public piece {
public:
	bishop(int Y, int X, int z, int t) : piece(Y, X, z, t) {
	}
	virtual char getS() {
		return z ? 'b' : 'B';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		if (!dx || !dy) {
			return 1;
		}
		int zy = dy ? dy / abs(dy) : 0, zx = dx ? dx / abs(dx) : 0, cnt = 0, yy = y, xx = x;
		while (yy != ty || xx != tx) {
			cnt += (a[yy][xx]->getS() != '.');
			yy += zy;
			xx += zx;
		}
		return cnt != 1;
	}
};

class lion : public piece {
public:
	lion(int Y, int X) : piece(Y, X, 3) {
	}
	virtual char getS() {
		return '@';
	}
	virtual int badmove(int ty, int tx) {
		int dy = ty - y, dx = tx - x;
		return !dy && !dx || abs(dy) > 2 || abs(dx) > 2;
	}
	virtual void askmove(int flag) {
		if (flag) {
			piece::askmove(flag);
		} else {
			piece::askmove(0);
			piece::askmove(0);
		}
	}
	virtual void askpromote(int flag) {
		t = '.';
	}
};

piece *getpiece(int y, int x, char s, char t) {
	if (s == '.') {
		return new space(y, x);
	} else if (toupper(s) == 'P') {
		return new pawn(y, x, isupper(s) ? 0 : 1);
	} else if (toupper(s) == 'E') {
		return new explosive(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'Q') {
		return new queen(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'C') {
		return new cannon(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'S') {
		return new silver(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'N') {
		return new knight(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'R') {
		return new rook(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'B') {
		return new bishop(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'O') {
		return new obstacle(y, x, isupper(s) ? 0 : 1);
	} else if (toupper(s) == 'K') {
		return new king(y, x, isupper(s) ? 0 : 1);
	} else if (toupper(s) == 'S') {
		return new silver(y, x, isupper(s) ? 0 : 1, t);
	} else if (toupper(s) == 'G') {
		return new gold(y, x, isupper(s) ? 0 : 1, t);
	} else if (s == '@') {
		return new lion(y, x);
	}
	printf("%d %c\n", s, s);
	assert(0);
}

void init() {
	for (int i = 1; i <= Z; i++) {
		for (int j = 1; j <= Z; j++) {
			a[i][j] = getpiece(i, j);
		}
	}
	for (int i = 0; i < 16; i++) {
		cnt[SS[i]] = cnt[tolower(SS[i])] = 1;
		cnt2[SS[i]] = cnt2[tolower(SS[i])] = 2;
	}
	copy_n(SS, 16, SS + 16);
	shuffle(SS, SS + 32, rand);
	for (int i = 0; i < 32; i++) {
		delete a[i / Z + 1][i % Z + 1];
		a[i / Z + 1][i % Z + 1] = getpiece(i / Z + 1, i % Z + 1, SS[i]);
		delete a[Z - i / Z][i % Z + 1];
		a[Z - i / Z][i % Z + 1] = getpiece(Z - i / Z, i % Z + 1, tolower(SS[i]));
	}
	for (int i = 1; i <= Z; i++) {
		delete a[3][i];
		a[3][i] = getpiece(3, i, 'P');
		delete a[Z - 2][i];
		a[Z - 2][i] = getpiece(Z - 2, i, 'p');
	}
	// TODO: when Z!=16
	for (auto y : {6, 11}) {
		for (auto x : {3, 7, 10, 14}) {
			a[y][x] = getpiece(y, x, '@');
		}
	}
	cnt2['p'] = cnt2['P'] = 16;
	cnt['p'] = cnt['P'] = 16;
	cnt['o'] = cnt['O'] = 0x3f3f3f3f;
	cnt['@'] = 8;
}

void show() {
	puts("=========================================");
	int flag = 0;
	for (auto i : S[cur ^ 1]) {
		if (toupper(i) != 'O') {
			for (int j = 0; j < cnt[i]; j++) {
				putchar(i);
				flag = 1;
			}
		}
	}
	if (flag) {
		printf("\n---------------------------------------\n");
	}
	if (cur) {
		printf("    ");
		for (int i = 'a' + Z - 1; i >= 'a'; i--) {
			printf(" %c", i);
		}
		printf("\n    +--------------------------------+\n");
		for (int i = 1; i <= Z; i++) {
			printf(" %2d |", i);
			for (int j = Z; j >= 1; j--) {
				a[i][j]->print();
			}
			printf("| %-2d\n", i);
		}
		printf("    +--------------------------------+\n    ");
		for (int i = 'a' + Z - 1; i >= 'a'; i--) {
			printf(" %c", i);
		}
		puts("");
	} else {
		printf("    ");
		for (int i = 'a'; i <= 'a' + Z - 1; i++) {
			printf(" %c", i);
		}
		printf("\n    +--------------------------------+\n");
		for (int i = Z; i >= 1; i--) {
			printf(" %2d |", i);
			for (int j = 1; j <= Z; j++) {
				a[i][j]->print();
			}
			printf("| %-2d\n", i);
		}
		printf("    +--------------------------------+\n    ");
		for (int i = 'a'; i <= 'a' + Z - 1; i++) {
			printf(" %c", i);
		}
		puts("");
	}
	flag = 0;
	for (auto i : S[cur]) {
		if (toupper(i) != 'O') {
			for (int j = 0; j < cnt[i]; j++) {
				flag = 1;
			}
		}
	}
	if (!flag) {
		return;
	}
	printf("---------------------------------------\n");
	for (auto i : S[cur]) {
		if (toupper(i) != 'O') {
			for (int j = 0; j < cnt[i]; j++) {
				putchar(i);
			}
		}
	}
	puts("");
}

void domove(int flag) {
	hint("%sinput initial location: ", flag == 0 ? "" : flag == 1 ? "bad format. " : flag == 2 ? "piece cannot be moved by you. " : "piece cannot be moved currently. ");
	fgets(buf, 471, stdin);
	int sy, sx;
	sscanf(buf, "%1s%d", buf2, &sy);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return doopt(0);
	}
	if (buf2[0] < 'A' || buf2[0] > 'A' + Z - 1 || sy < 1 || sy > Z) {
		return domove(1);
	}
	sx = buf2[0] - 'A' + 1;
	if (!a[sy][sx]->canmove()) {
		return domove(2);
	}
	for (int i = 1; i <= Z; i++) {
		for (int j = 1; j <= Z; j++) {
			if (a[i][j]->cantarget() && !a[sy][sx]->badmove(i, j)) {
				goto aaa;
			}
		}
	}
	return domove(3);
	aaa:;
	a[sy][sx]->askmove(0);
}

void doplace2(int flag, int up) {
	hint("%sinput location: ", flag == 0 ? "" : flag == 1 ? "bad format. " : "bad location. ");
	fgets(buf, 471, stdin);
	int sx, sy;
	sscanf(buf, "%1s%d", buf2, &sy);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return doplace1(0);
	}
	if (buf2[0] < 'A' || buf2[0] > 'A' + Z - 1 || sy < 1 || sy > Z) {
		return doplace2(1, up);
	}
	sx = buf2[0] - 'A' + 1;
	if (a[sy][sx]->getS() != '.') {
		return doplace2(2, up);
	}
	delete a[sy][sx];
	a[sy][sx] = getpiece(sy, sx, up);
	cnt[up]--;
	cnt2[up]++;
}

void doplace1(int flag) {
	hint("%sdrop what? e[Z]it/[B]ishop/[C]annon/[E]xplosive/[G]old/[K]ing/k[N]ight/[O]bstacle/[P]awn/[Q]ueen/[R]ook/[S]ilver: ", flag == 0 ? "" : flag == 1 ? "bad piece. " : "bad drop. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return doopt(0);
	}
	if (buf2[0] != 'B' && buf2[0] != 'C' && buf2[0] != 'E' && buf2[0] != 'G' && buf2[0] != 'K' && buf2[0] != 'N' && buf2[0] != 'O' && buf2[0] != 'P' && buf2[0] != 'Q' && buf2[0] != 'R' && buf2[0] != 'S' || buf2[1]) {
		return doplace1(1);
	}
	int up = cur ? tolower(buf2[0]) : buf2[0];
	if (!cnt[up]) {
		return doplace1(2);
	}
	doplace2(0, up);
}

void doopt(int flag) {
	hint("%s%s moves. choose option. [P]lace/[M]ove: ", flag == 0 ? "" : flag == 1 ? "bad format. " : "bad move. ", cur ? "black" : "white");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] != 'P' && buf2[0] != 'M' || buf2[1]) {
		return doopt(1);
	}
	if (buf2[0] == 'P') {
		doplace1(0);
	} else {
		domove(0);
	}
}

int main() {
	init();
	while (true) {
		show();
		doopt(0);
		cur ^= 1;
	}
}
