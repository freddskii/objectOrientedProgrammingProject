#include <bits/stdc++.h>
using namespace std;

class Calculator {
    using ld = long double;
    ld display;
    ld accumulator;
    char pendingOp;
    bool enteringNumber;
    string currentInput;
    bool hasDecimal;
    int digitCount;
    bool errorState;
    char lastOp;
    ld lastOperand;

public:
    Calculator() { clear(); }

    void clear() {
        display = 0.0L;
        accumulator = 0.0L;
        pendingOp = '=';
        enteringNumber = false;
        currentInput.clear();
        hasDecimal = false;
        digitCount = 0;
        errorState = false;
        lastOp = '=';
        lastOperand = 0.0L;
    }

    static bool isDigitKey(const string &k) {
        return k.size() == 1 && isdigit((unsigned char)k[0]);
    }

    static ld stold_safe(const string &s) {
        if (s.empty()) return 0.0L;
        try { return stold(s); } catch (...) { return 0.0L; }
    }

    void applyOp(char op, ld val) {
        if (op == '=') { accumulator = val; return; }
        if (op == '+') accumulator += val;
        else if (op == '-') accumulator -= val;
        else if (op == 'x') accumulator *= val;
        else if (op == ':') {
            if (fabsl(val) == 0.0L) { errorState = true; return; }
            accumulator /= val;
        } else accumulator = val;

        if (!errorState && fabsl(accumulator) >= 1e100L) errorState = true;
        if (!errorState && fabsl(accumulator) > 0.0L && fabsl(accumulator) < 1e-100L) accumulator = 0.0L;
    }

    void finalizeCurrentNumberIfAny(ld &outVal, bool &hadNumber) {
        if (enteringNumber) {
            outVal = stold_safe(currentInput.empty() ? "0" : currentInput);
            hadNumber = (digitCount > 0) || hasDecimal;
        } else {
            outVal = display;
            hadNumber = false;
        }
    }

    void pressKey(const string &key) {
        if (errorState && key != "C") return;
        if (key == "C") { clear(); return; }

        if (isDigitKey(key)) {
            if (!enteringNumber) {
                enteringNumber = true;
                currentInput.clear();
                hasDecimal = false;
                digitCount = 0;
            }
            if (digitCount >= 8) return;
            char d = key[0];
            if (d == '0' && currentInput.empty() && !hasDecimal && display == 0.0L) return;
            currentInput.push_back(d);
            digitCount++;
            display = stold_safe(currentInput);
            return;
        }

        if (key == ".") {
            if (!enteringNumber) {
                enteringNumber = true;
                currentInput.clear();
                hasDecimal = false;
                digitCount = 0;
            }
            if (!hasDecimal) {
                if (currentInput.empty()) currentInput = "0";
                currentInput.push_back('.');
                hasDecimal = true;
                display = stold_safe(currentInput);
            }
            return;
        }

        if (key == "+" || key == "-" || key == "x" || key == ":" || key == "=") {
            bool hadNumber = false;
            ld parsed = 0.0L;
            finalizeCurrentNumberIfAny(parsed, hadNumber);

            if (hadNumber) {
                if (pendingOp == '=') accumulator = parsed;
                else applyOp(pendingOp, parsed);
                lastOperand = parsed;
                lastOp = (pendingOp == '=') ? '=' : pendingOp;
            } else {
                if (key == "=") {
                    if (lastOp != '=') applyOp(lastOp, lastOperand);
                }
            }

            if (errorState) return;
            if (key == "=") {
                pendingOp = '=';
                display = accumulator;
            } else {
                pendingOp = key[0];
                if (hadNumber) display = accumulator;
            }

            enteringNumber = false;
            currentInput.clear();
            hasDecimal = false;
            digitCount = 0;
        }
    }

    string getDisplay() {
        if (errorState) return string(4, ' ') + "Error.";
        ld value = display, absValue = fabsl(value);
        if (absValue > 0.0L && absValue < 1e-100L) value = 0.0L;
        if (absValue >= 1e100L) { errorState = true; return string(4, ' ') + "Error."; }
        string normal = formatNormal(value);
        string trimmed = normal;
        trimmed.erase(0, trimmed.find_first_not_of(' '));
        bool normalIsZero = (trimmed == "0." || trimmed == "-0.");
        if (normalIsZero && fabsl(value) > 1e-100L) return formatScientific(value);
        return normal;
    }

private:
    string formatNormal(ld value) {
        ld absValue = fabsl(value);
        int intDigits = (absValue >= 1.0L) ? (int)floor(log10(absValue)) + 1 : 1;
        int decPlaces = 8 - intDigits;
        if (decPlaces < 0) return formatScientific(value);
        if (decPlaces > 7) decPlaces = 7;
        ld pow10 = 1.0L;
        for (int i = 0; i < decPlaces; ++i) pow10 *= 10.0L;
        ld rounded = roundl(value * pow10) / pow10;
        if (fabsl(rounded) >= 1e100L) return string(4, ' ') + "Error.";
        if (fabsl(rounded) > 0.0L && fabsl(rounded) < 1e-100L) rounded = 0.0L;
        ostringstream oss;
        oss.setf(ios::fixed);
        oss << setprecision(decPlaces) << (double)rounded;
        string s = oss.str();
        size_t dot = s.find('.');
        if (dot != string::npos) {
            size_t last = s.size() - 1;
            while (last > dot && s[last] == '0') --last;
            s = s.substr(0, last + 1);
            if (s.back() == '.') {}
        } else s += '.';
        while ((int)s.size() < 10) s = " " + s;
        return s;
    }

    string formatScientific(ld value) {
        if (fabsl(value) == 0.0L) {
            string z = "0.";
            while ((int)z.size() < 10) z = " " + z;
            return z;
        }
        int exp = (int)floor(log10(fabsl(value)));
        ld mant = fabsl(value) / powl(10.0L, exp);
        if (mant >= 10.0L) { mant /= 10.0L; ++exp; }
        ld mant_rounded = roundl(mant * 1e4L) / 1e4L;
        if (mant_rounded >= 10.0L) { mant_rounded /= 10.0L; ++exp; }
        ostringstream oss;
        oss.setf(ios::fixed);
        oss << setprecision(4) << (double)mant_rounded;
        string mantstr = oss.str();
        size_t dot = mantstr.find('.');
        if (dot != string::npos) {
            size_t last = mantstr.size() - 1;
            while (last > dot && mantstr[last] == '0') --last;
            mantstr = mantstr.substr(0, last + 1);
        }
        if (value < 0) mantstr = "-" + mantstr;
        char expSign = (exp < 0) ? '-' : ' ';
        int aexp = abs(exp);
        char expbuf[8];
        sprintf(expbuf, "%c%02d", expSign, aexp);
        string res = mantstr + string(expbuf);
        while ((int)res.size() < 10) res = " " + res;
        return res;
    }
};

int main() {
    int t;
    if (!(cin >> t)) return 0;
    string line;
    getline(cin, line);
    for (int i = 0; i < t; i++) {
        getline(cin, line);
        Calculator calc;
        istringstream iss(line);
        string token;
        while (iss >> token) {
            if (token.size() >= 2 && token.front() == '[' && token.back() == ']') {
                string key = token.substr(1, token.size() - 2);
                calc.pressKey(key);
            }
        }
        cout << calc.getDisplay() << "\n";
    }
    return 0;
}
