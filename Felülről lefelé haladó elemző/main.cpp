#include <iostream>
#include <string>
#include <cctype>

using namespace std;

class TopDownParser {
private:
    string source;
    size_t position;

    char currentChar() const {
        if (position < source.length()) {
            return source[position];
        }
        return '\0';
    }

    bool consume(char symbol) {
        if (currentChar() == symbol) {
            position++;
            return true;
        }
        return false;
    }

    bool parseS() {
        if (currentChar() == 'a') {
            consume('a');
            return parseS();
        }

        if (currentChar() == 'b') {
            consume('b');
            if (!parseA()) {
                return false;
            }
            return consume('c');
        }

        return false;
    }

    bool parseA() {
        if (currentChar() == 'b') {
            consume('b');
            if (!parseA()) {
                return false;
            }
            return consume('c');
        }

        if (currentChar() == 'd') {
            consume('d');
            return true;
        }

        return false;
    }

public:
    explicit TopDownParser(const string& text)
        : source(text), position(0) {
    }

    bool parse() {
        bool valid = parseS();
        return valid && position == source.length();
    }
};

int main() {
    string inputText;

    cout << "Enter input string: ";
    getline(cin, inputText);

    TopDownParser parser(inputText);

    if (parser.parse()) {
        cout << "Input string accepted." << endl;
    } else {
        cout << "Input string rejected." << endl;
    }

    return 0;
}
