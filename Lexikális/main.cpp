#include <stdio.h>
#include <string.h>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

// Globalis valtozok
static char szamjegyek[] = "0123456789";
static char betuk[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Fuggveny, ami ellenorzi, hogy egy adott karakter szerepel-e egy karakterlancban
int StrContains(char value, char str[]) {
	for (size_t i = 0; i < strlen(str); i++) {
		if (str[i] == value) {
			return 1;
		}
	}
	return 0;
}
// 0: azon, 1: konst, 2:kommentA, 3:kommentB,-1:none
int ModeCheck(char c, char prevC, int m) {
	if (StrContains(c, betuk) && m == -1) {
		return 0;
	}
	if ((StrContains(c, szamjegyek) && m == -1)) {
		return 1;
	}
	if ((prevC== '(' && c == '*') && m == -1) {
		return 2;
	}
	if (c == '{' && m == -1) {
		return 3;
	}
	return m;
}

int LexikalisElemzes(char utasitas[]) {
	printf("utasitas: %s\n", utasitas);

	char prevChar = '-';

	char token[100] = "";
	int mode = -1;
	for (int i = 0; i <= strlen(utasitas); i++) {
		mode = ModeCheck(utasitas[i], prevChar, mode);

		// Azonosito ellenorzes
		if (mode == 0) {
			if (!(StrContains(utasitas[i], betuk)) && !(StrContains(utasitas[i], szamjegyek))) {
				printf("%s = <azonosito>\n", token);
				mode = ModeCheck(utasitas[i], prevChar, -1);
				strcpy(token, "");
			}
		}
		// Konstans ellenorzes
		if (mode == 1) {
			if (!(StrContains(utasitas[i], szamjegyek))) {
				printf("%s = <konstans>\n", token);
				mode = ModeCheck(utasitas[i], prevChar, -1);
				strcpy(token, "");
			}
		}
		strncat(token, &utasitas[i], 1);

		// (*Komment*) ellenorzes
		if (mode == 2) {
			if (prevChar == '*' && utasitas[i] == ')') {
				//printf(" = <komment>\n");
				mode = ModeCheck(utasitas[i], prevChar, -1);
				strcpy(token, "");
			}
		}
		// {Komment} ellenorzes
		if (mode == 3) {
			if (utasitas[i] == '}') {
				//printf(" = <komment>\n");
				mode = ModeCheck(utasitas[i], prevChar, -1);
				strcpy(token, "");
			}
		}

		// Ertekadas ellenorzes
		if (prevChar == ':' && utasitas[i] == '=') {
			printf("%s = <ertekadas>\n", token);
			strcpy(token, "");
		}

		// Relacio ellenorzes
		if ((prevChar == '<' && utasitas[i] == '=') ||
			(prevChar == '>' && utasitas[i] == '=') ||
			(prevChar == '<' && utasitas[i] == '>')) {
			printf("%s = <relacio>\n", token);
			strcpy(token, "");
		}

		// EoF ellenorzes
		if (utasitas[i] == '$') {
			//printf(" = <End of File>\n\n");
			printf("\n");
			mode = -1;
			return 0;
		}
		prevChar = utasitas[i];
	}

	return 0;
}

int main() {
	// LexikalisElemzes fuggveny tesztelese
	char utasitasA[] = "kiss23:=12>=nagy11144<>11144(*ize*)$";
	char utasitasB[] = "valami5uperAzon12:=(*{komment kommentbe?}*)684{erdekes komment}<>123>=(*valami*)unalmasAzon$";
	char utasitasC[] = "eofTest231:=0{234}$<>azon123<=42135";

	LexikalisElemzes(utasitasA);
	LexikalisElemzes(utasitasB);
	LexikalisElemzes(utasitasC);

	return 0;
}
