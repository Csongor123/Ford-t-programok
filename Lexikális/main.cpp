#include <iostream>
#include <string>
using namespace std;

// Globalis valtozok
string szamjegyek = "0123456789";
string betuk = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Ellenorzi, hogy egy karakter szerepel-e egy sztringben
bool StrContains(char value, const string& str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == value) {
            return true;
        }
    }
    return false;
}

// 0: azonosito, 1: konstans, 2: komment (* *), 3: komment { }, -1: nincs
int ModeCheck(char c, char prevC, int m) {
    if (StrContains(c, betuk) && m == -1) {
        return 0;
    }
    if (StrContains(c, szamjegyek) && m == -1) {
        return 1;
    }
    if (prevC == '(' && c == '*' && m == -1) {
        return 2;
    }
    if (c == '{' && m == -1) {
        return 3;
    }
    return m;
}

void LexikalisElemzes(const string& utasitas) {
    cout << "utasitas: " << utasitas << endl;

    char prevChar = '-';
    string token = "";
    int mode = -1;

    for (size_t i = 0; i < utasitas.length(); i++) {
        mode = ModeCheck(utasitas[i], prevChar, mode);

        // Azonosito
        if (mode == 0) {
            if (!StrContains(utasitas[i], betuk)
                && !StrContains(utasitas[i], szamjegyek)) {
                cout << token << " = <azonosito>" << endl;
                token = "";
                mode = ModeCheck(utasitas[i], prevChar, -1);
            }
        }

        // Konstans
        if (mode == 1) {
            if (!StrContains(utasitas[i], szamjegyek)) {
                cout << token << " = <elojel_nelkuli_egesz>" << endl;
                token = "";
                mode = ModeCheck(utasitas[i], prevChar, -1);
            }
        }

        token += utasitas[i];

        // (* komment *)
        if (mode == 2) {
            if (prevChar == '*' && utasitas[i] == ')') {
                token = "";
                mode = -1;
            }
        }

        // { komment }
        if (mode == 3) {
            if (utasitas[i] == '}') {
                token = "";
                mode = -1;
            }
        }

        // Ertekadas
        if (prevChar == ':' && utasitas[i] == '=') {
            cout << token << " = <ertekadas>" << endl;
            token = "";
        }

        // Relaciok
        if ((prevChar == '<' && utasitas[i] == '=')
            || (prevChar == '<' && utasitas[i] == '>')
            || (prevChar == '>' && utasitas[i] == '=')) {
            cout << token << " = <relacio>" << endl;
            token = "";
        }

        // EOF
        if (utasitas[i] == '$') {
            cout << "$ = <EOF>" << endl << endl;
            return;
        }

        prevChar = utasitas[i];
    }
}

int main() {
    string utasitasA = "Nagy21<=10<>11:=Kiss22{Nagy23}(:aa=bb:)$";


    LexikalisElemzes(utasitasA);


    return 0;
}
