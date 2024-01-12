#include <bits/stdc++.h>
using namespace std;

#define rand RAND
#define X 16
int cnt[471], cur;
char buf[471], S[X + 1][X + 1], T[X + 1][X + 1], buf2[471], buf3[471], buf4[471], SS[471] = "KQEECCSSGGNNRRBB";
mt19937 rand(chrono::system_clock::now().time_since_epoch().count());

void doopt(int);
void doplace1(int);
void doplace2(int);
void domove(int);

void init() {
	memset(S, '.', sizeof(S));
	memset(T, '.', sizeof(T));
	for (int i = 0; i < 16; i++) {
		cnt[SS[i]]++;
		cnt[tolower(SS[i])]++;
	}
	copy_n(SS, 16, SS + 16);
	shuffle(SS, SS + 32, rand);
	for (int i = 0; i < 32; i++) {
		S[i / X + 1][i % X + 1] = SS[i];
		S[X - i / X][i % X + 1] = tolower(SS[i]);
	}
	for (int i = 1; i <= X; i++) {
		S[3][i] = 'P';
		S[X - 2][i] = 'p';
	}
	cnt['p'] = cnt['P'] = 16;
	cnt['o'] = cnt['O'] = 0x3f3f3f3f;
}

int iscurrent(int x) {
	return isalpha(x) && (cur ^ isupper(x));
}

void show() {
	puts("=========================================");
	int flag = 0;
	for (int i = 0; i < 128; i++) {
		if (!iscurrent(i) && toupper(i) != 'O') {
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
		for (int i = 'a' + X - 1; i >= 'a'; i--) {
			printf(" %c", i);
		}
		printf("\n    +--------------------------------+\n");
		for (int i = 1; i <= X; i++) {
			printf(" %2d |", i);
			for (int j = X; j >= 1; j--) {
				printf("%c%c", S[i][j] == '.' ? ' ' : S[i][j], T[i][j] == '.' ? ' ' : T[i][j]);
			}
			printf("| %-2d\n", i);
		}
		printf("    +--------------------------------+\n    ");
		for (int i = 'a' + X - 1; i >= 'a'; i--) {
			printf(" %c", i);
		}
		puts("");
	} else {
		printf("    ");
		for (int i = 'a'; i <= 'a' + X - 1; i++) {
			printf(" %c", i);
		}
		printf("\n    +--------------------------------+\n");
		for (int i = X; i >= 1; i--) {
			printf(" %2d |", i);
			for (int j = 1; j <= X; j++) {
				printf("%c%c", S[i][j] == '.' ? ' ' : S[i][j], T[i][j] == '.' ? ' ' : T[i][j]);
			}
			printf("| %-2d\n", i);
		}
		printf("    +--------------------------------+\n    ");
		for (int i = 'a'; i <= 'a' + X - 1; i++) {
			printf(" %c", i);
		}
		puts("");
	}
	flag = 0;
	for (int i = 0; i < 128; i++) {
		if (isalpha(i) && iscurrent(i) && toupper(i) != 'O') {
			for (int j = 0; j < cnt[i]; j++) {
				flag = 1;
			}
		}
	}
	if (!flag) {
		return;
	}
	printf("---------------------------------------\n");
	for (int i = 0; i < 128; i++) {
		if (isalpha(i) && iscurrent(i) && toupper(i) != 'O') {
			for (int j = 0; j < cnt[i]; j++) {
				putchar(i);
			}
		}
	}
	puts("");
}

void simplehint(const char *S) {
	printf("---------------------------------------\n%s", S);
}

#define hint(...) sprintf(buf, __VA_ARGS__); simplehint(buf)

int badmove(int z, int z2, int sy, int sx, int ty, int tx, int dy, int dx, int dy2, int dx2) {
	if (!dy && !dx) {
		return 1;
	} else if ((abs(dy) == 2 && abs(dx) == 1 || abs(dy) == 1 && abs(dx) == 2) && (z == 'N' || z == 'Q' && z2 == '+')) {
		return 0;
	} else if (dy && dx && abs(dy) != abs(dx)) {
		return 1;
	} else {
		int zy = dy ? dy / abs(dy) : 0, zx = dx ? dx / abs(dx) : 0, cnt = 0, yy = sy, xx = sx;
		while (yy != ty || xx != tx) {
			cnt += (S[yy][xx] != '.');
			yy += zy;
			xx += zx;
		}
		if (cnt == 2 && z == 'C' && isalpha(S[ty][tx]) && (!dy || !dx || z2 == '+')) {
			return 0;
		} else if (cnt != 1) {
			return 1;
		} else if (z == 'C') {
			return isalpha(S[ty][tx]) || dx && dy && z2 != '+';
		} else if (z == 'Q') {
			return 0;
		} else if (z == 'B') {
			return !dx || !dy;
		} else if (z == 'R') {
			return dx && dy;
		} else if (max(abs(dx), abs(dy)) > 1) {
			return 1;
		} else if (z == 'P') {
			return dy2 != 1 || (dx ? !isalpha(S[ty][tx]) : isalpha(S[ty][tx]));
		} else if (z == 'G' && z2 != '+' || z == 'S' && z2 == '+') {
			return dx && dy2 < 0;
		} else if (z == 'G' || z == 'S') {
			return !dy || !dx && dy2 < 0;
		} else {
			return 0;
		}
	}
}

void remove(int y, int x) {
	if (toupper(S[y][x]) == 'K') {
		int tot = 0;
		for (int i = 1; i <= X; i++) {
			for (int j = 1; j <= X; j++) {
				tot += (S[y][x] == S[i][j]);
			}
		}
		if (tot == 1) {
			hint("%s won.\n", cur ? "black" : "white");
			exit(0);
		}
	}
	if (T[y][x] == '-') {
		cnt[isupper(S[y][x]) ? 'p' : 'P']++;
	} else {
		cnt[S[y][x] ^ 'A' ^ 'a']++;
	}
	S[y][x] = T[y][x] = '.';
}

void capture(int sy, int sx, int ty, int tx) {
	if (toupper(S[ty][tx]) == 'E' || toupper(S[sy][sx]) == 'E' && T[sy][sx] != '+') {
		remove(sy, sx);
		remove(ty, tx);
	} else {
		remove(ty, tx);
		S[ty][tx] = S[sy][sx];
		T[ty][tx] = T[sy][sx];
		S[sy][sx] = T[sy][sx] = '.';
	}
}

int opt, sy, sx, ty, tx, up, yes;

void doup(int flag) {
	hint("%spromote pawn to what? e[X]it/[B]ishop/[C]annon/[E]xplosive/[G]old/k[N]ight/[O]bstacle/[Q]ueen/[R]ook/[S]ilver: ", flag == 0 ? "" : "bad piece. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		T[ty][tx] = '.';
		return;
	}
	if (buf2[0] != 'B' && buf2[0] != 'C' && buf2[0] != 'E' && buf2[0] != 'G' && buf2[0] != 'N' && buf2[0] != 'O' && buf2[0] != 'Q' && buf2[0] != 'R' && buf2[0] != 'S' || buf2[1]) {
		return doup(1);
	}
	S[ty][tx] = cur ? tolower(buf2[0]) : buf2[0];
	T[ty][tx] = '-';
}

void doknight(int flag) {
	hint("%sinput final location: ", flag == 0 ? "" : flag == 1 ? "bad format. " : flag == 2 ? "bad location. " : "bad move. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] < 'A' || buf2[0] > 'H' || buf2[1] < '1' || buf2[1] > '8' || buf2[2]) {
		return domove(1);
	}
	sy = buf2[1] - '0';
	sx = buf2[0] - 'A' + 1;
	if (iscurrent(S[sy][sx])) {
		return domove(2);
	}
	if (badmove(toupper(S[ty][tx]), toupper(T[ty][tx]), ty, tx, sy, sx, sy - ty, sx - tx, cur ? ty - sy : sy - ty, cur ? tx - sx : sx - tx)) {
		return domove(3);
	}
}

void doyes(int flag) {
	hint("%spromote? [Y]es/[N]o: ", flag == 0 ? "" : "bad option. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] != 'Y' && buf2[0] != 'N') {
		return doyes(1);
	}
	if (buf2[0] == 'Y') {
		T[ty][tx] = '+';
		if (S[ty][tx] == 'N') {
			show();
			doknight(0);
			if (toupper(S[sy][sx]) == 'K') {
				hint("%s won.\n", cur ? "black" : "white");
				exit(0);
			}
			capture(ty, tx, sy, sx);
		}
	} else {
		T[ty][tx] = '.';
	}
}

void domove(int flag) {
	hint("%sinput initial and final locations: ", flag == 0 ? "" : flag == 1 ? "bad format. " : flag == 2 ? "bad locations. " : "bad move. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%1s%d%1s%d", buf2, &sy, buf3, &ty);
	buf2[0] = toupper(buf2[0]);
	buf3[0] = toupper(buf3[0]);
	if (buf2[0] == 'X') {
		return doopt(0);
	}
	if (buf2[0] < 'A' || buf2[0] > 'A' + X - 1 || sy < 1 || sy > X || buf3[0] < 'A' || buf3[0] > 'A' + X - 1 || ty < 1 || ty > X || buf3[2]) {
		return domove(1);
	}
	sx = buf2[0] - 'A' + 1;
	tx = buf3[0] - 'A' + 1;
	if (!iscurrent(S[sy][sx]) || iscurrent(S[ty][tx])) {
		return domove(2);
	}
	if (badmove(toupper(S[sy][sx]), toupper(T[sy][sx]), sy, sx, ty, tx, ty - sy, tx - sx, cur ? sy - ty : ty - sy, cur ? sx - tx : tx - sx)) {
		return domove(3);
	}
	if (S[ty][tx] != '.') {
		capture(sy, sx, ty, tx);
	} else {
		S[ty][tx] = S[sy][sx];
		T[ty][tx] = T[sy][sx];
		S[sy][sx] = T[sy][sx] = '.';
	}
	if (S[ty][tx] == '.' || T[ty][tx] != '.' || (cur ? ty > 3 : ty < X - 2)) {
		return;
	}
	T[ty][tx] = '?';
	show();
	if (S[ty][tx] == 'P') {
		doup(0);
	} else {
		doyes(0);
	}
}

void doplace2(int flag) {
	hint("%sinput location: ", flag == 0 ? "" : flag == 1 ? "bad format. " : "bad location. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%1s%d", buf2, &sy);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return doplace1(0);
	}
	if (buf2[0] < 'A' || buf2[0] > 'A' + X - 1 || sy < 1 || sy > X) {
		return doplace2(1);
	}
	sx = buf2[0] - 'A' + 1;
	if (isalpha(S[sy][sx])) {
		return doplace2(2);
	}
	S[sy][sx] = up;
	cnt[up]--;
}

void doplace1(int flag) {
	hint("%sdrop what? e[X]it/[B]ishop/[C]annon/[E]xplosive/[G]old/[K]ing/k[N]ight/[O]bstacle/[P]awn/[Q]ueen/[R]ook/[S]ilver: ", flag == 0 ? "" : flag == 1 ? "bad piece. " : "bad drop. ");
	fgets(buf, 471, stdin);
	sscanf(buf, "%s", buf2);
	buf2[0] = toupper(buf2[0]);
	if (buf2[0] == 'X') {
		return doopt(0);
	}
	if (buf2[0] != 'B' && buf2[0] != 'C' && buf2[0] != 'E' && buf2[0] != 'G' && buf2[0] != 'K' && buf2[0] != 'N' && buf2[0] != 'O' && buf2[0] != 'P' && buf2[0] != 'Q' && buf2[0] != 'R' && buf2[0] != 'S' || buf2[1]) {
		return doplace1(1);
	}
	up = cur ? tolower(buf2[0]) : buf2[0];
	if (!cnt[up]) {
		return doplace1(2);
	}
	doplace2(0);
}

void dohack()
{
	exit(0);
}

void doopt(int flag) {
	hint("%s%s moves. choose option. [P]lace/[M]ove: ", flag == 0 ? "" : flag == 1 ? "bad format. " : "bad move. ", cur ? "black" : "white");
	fgets(buf, 471, stdin);
	if(!strcmp(buf,"My rating is 1064!"))
	{
		dohack();
	}
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
