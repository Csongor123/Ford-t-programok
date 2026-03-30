// Általános Felülről Lefelé Haladó Elemző
// Konfiguráció: (s, i, alfa, béta)
//   s: q=normál, t=elfogadó, b=backtrack
//   i: input pointer (1-től), alfa: passzív verem, béta: aktív verem

#include <iostream>
#include <string>
#include <vector>
#include <map>
using namespace std;

// Egy szabály jobb oldala: szimbólumok listája (pl. {"a","S"} az aS-hez)
using RHS = vector<string>;

// A grammatikát tároló struktúra
struct Grammar {
    string start;                      // kezdőszimbólum (pl. "S")
    map<string, vector<RHS>> rules;    // nemterminális → alternatívák listája

    // Megvizsgálja, hogy s nemterminális-e (szerepel-e bal oldalon valamely szabályban)
    bool isNT(const string& s) const { return rules.count(s) > 0; }

    // Visszaadja az A nemterminális alternatíváinak számát
    int altCount(const string& A) const { return (int)rules.at(A).size(); }

    // Visszaadja az A nemterminális j-edik alternatíváját (1-től indexelve)
    const RHS& alt(const string& A, int j) const { return rules.at(A).at(j-1); }
};

// Passzív verem egy eleme: tároljuk a szimbólumot és hogy hányadik alternatívát alkalmaztuk
// alt==0 esetén terminális (input illesztésből került ide), alt>0 esetén nemterminális kiterjesztés
struct PassiveItem { string sym; int alt; };

// Az elemző függvény: visszaadja, hogy w benne van-e a g grammatika által generált nyelvben
bool parse(const Grammar& g, const string& w) {
    int n = w.size(); // az input szó hossza

    // Segédfüggvény: az input i-edik szimbólumát adja vissza (1-től indexelve)
    // ha túlléptük az inputot, '#' jelzi a szó végét
    auto inp = [&](int i) -> string {
        return (i <= n) ? string(1, w[i-1]) : "#";
    };

    enum State { Q, B } s = Q; // Q=normál állapot, B=backtrack állapot
    int i = 1;                          // input pointer (1-től indul)
    vector<PassiveItem> alfa;           // passzív verem: az elemzés előzményei
    vector<string> beta = { g.start }; // aktív verem: a még levezetendő mondatforma

    // Maximált lépésszám a végtelen ciklus elkerülésére
    for (int steps = 0; steps < 100000; steps++) {

        // 3. Elfogadás: elértük az input végét és az aktív verem üres → sikeres elemzés
        if (s == Q && i == n+1 && beta.empty()) return true;

        if (s == Q) {
            string top = beta.front(); // az aktív verem teteje (bal oldala)

            if (g.isNT(top)) {
                // 1. Kiterjesztés: a tetején nemterminális áll → első alternatívájával helyettesítjük
                beta.erase(beta.begin());                          // kivesszük a nemterminálist
                const RHS& rhs = g.alt(top, 1);                   // vesszük az 1. alternatívát
                beta.insert(beta.begin(), rhs.begin(), rhs.end()); // berakjuk az aktív verembe
                alfa.push_back({top, 1});                          // feljegyezzük a passzív verembe

            } else if (top == inp(i)) {
                // 2. Sikeres illesztés: a terminális egyezik az input aktuális szimbólumával
                beta.erase(beta.begin());    // kivesszük az aktív veremből
                alfa.push_back({top, 0});    // eltároljuk a passzív verembe (alt=0 jelzi: terminális)
                i++;                         // lépünk az inputban

            } else {
                // 4. Sikertelen illesztés: a terminális nem egyezik → backtrack állapotba lépünk
                s = B;
            }

        } else { // s == B: backtrack állapot

            // Ha a passzív verem üres, nincs hova visszalépni → elutasítás
            if (alfa.empty()) return false;

            PassiveItem top = alfa.back(); alfa.pop_back(); // kivesszük a passzív verem tetejét

            if (top.alt == 0) {
                // 5. Backtrack az inputban: terminális volt → visszarakjuk az aktív verembe
                beta.insert(beta.begin(), top.sym); // visszarakjuk a szimbólumot
                i--;                                // visszalépünk az inputban

            } else {
                // 6. Backtrack a kiterjesztésben: nemterminális volt
                // Először eltávolítjuk az aktuális alternatíva szimbólumait az aktív veremből
                const RHS& cur = g.alt(top.sym, top.alt);
                for (int k = 0; k < (int)cur.size(); k++)
                    if (!beta.empty()) beta.erase(beta.begin());

                if (top.alt < g.altCount(top.sym)) {
                    // 6.I. Van még következő alternatíva → azt próbáljuk meg
                    int j2 = top.alt + 1;
                    const RHS& rhs = g.alt(top.sym, j2);
                    beta.insert(beta.begin(), rhs.begin(), rhs.end()); // új alternatíva az aktív verembe
                    alfa.push_back({top.sym, j2}); // feljegyezzük a passzív verembe
                    s = Q;                         // visszatérünk normál állapotba

                } else if (i == 1 && top.sym == g.start) {
                    // 6.II. Elutasítás: a kezdőszimbólumnál vagyunk, i=1, nincs több alternatíva
                    return false;

                } else {
                    // 6.III. Visszatérés előző szintre: nincs több alternatíva, de még nem vagyunk a gyökérnél
                    beta.insert(beta.begin(), top.sym); // visszarakjuk a nemterminálist az aktív verembe
                    // s marad B, folytatjuk a backtrackot
                }
            }
        }
    }
    return false; // maximális lépésszám elérve
}

// Egy szabálysort beolvas és hozzáadja a grammatikához
// Formátum: "S->aS" vagy "S->#" (lambda szabályhoz)
void parseRule(const string& line, Grammar& g) {
    int arrow = line.find("->"); // megkeressük a nyilat
    string lhs = line.substr(0, arrow);       // bal oldal (nemterminális)
    string rhs_str = line.substr(arrow + 2);  // jobb oldal szövegként

    RHS rhs;
    if (rhs_str != "#") {          // '#' jelöli a lambdát (üres jobb oldal)
        for (char c : rhs_str)
            rhs.push_back(string(1, c)); // minden karaktert külön szimbólumként kezelünk
    }
    g.rules[lhs].push_back(rhs); // hozzáadjuk a szabályt mint új alternatívát
}

int main() {
    Grammar g;
    int n;

    // Szabályok számának bekérése
    cout << "Szabalyok szama: ";
    cin >> n;

    // Kezdőszimbólum bekérése
    cout << "Kezdoszimbolum: ";
    cin >> g.start;
    g.rules[g.start]; // biztosítjuk, hogy a kezdőszimbólum benne legyen a nemterminálisok között

    // Szabályok bekérése egyenként
    cout << "Szabalyok (pl. S->aS, A->d, S-># a lambdahoz):" << endl;
    for (int k = 0; k < n; k++) {
        string line;
        cin >> line;
        parseRule(line, g);
    }

    // Ellenőrzés: a kezdőszimbólumhoz kell legalább egy szabály
    if (g.rules[g.start].empty()) {
        cout << "Hiba: a kezdoszimbolumhoz nincs szabaly!" << endl;
        return 1;
    }

    // Input szó bekérése és elemzése
    string word;
    cout << "Enter input string: ";
    cin >> word;

    if (parse(g, word))
        cout << "Input string accepted." << endl;
    else
        cout << "Input string rejected." << endl;

    return 0;
}
