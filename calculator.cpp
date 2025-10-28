#include <bits/stdc++.h>
using namespace std;

class Calculator {
    using ld = long double;
    ld display;
    ld accumulator;
    char pendingOp;
    bool enteringNumber;
    string currentInput; // only digits and maybe one '.'
    bool hasDecimal;
    int digitCount; // count of digit keys for current number
    bool errorState;

    // for repeating '='
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
        try {
            return stold(s);
        } catch(...) {
            return 0.0L;
        }
    }

    void applyOp(char op, ld val) {
        if (op == '=') {
            accumulator = val;
            return;
        }
        if (op == '+') accumulator += val;
        else if (op == '-') accumulator -= val;
        else if (op == 'x') accumulator *= val;
        else if (op == ':') {
            if (fabsl(val) == 0.0L) {
                errorState = true;
                return;
            }
            accumulator /= val;
        } else {
            accumulator = val;
        }
        if (!errorState && fabsl(accumulator) >= 1e100L) errorState = true;
        if (!errorState && fabsl(accumulator) > 0.0L && fabsl(accumulator) < 1e-100L) accumulator = 0.0L;
    }

    void finalizeCurrentNumberIfAny(ld &outVal, bool &hadNumber) {
        if (enteringNumber) {
            // If user typed only '.' we stored currentInput as "0." -> stold works
            outVal = stold_safe(currentInput.empty() ? "0" : currentInput);
            hadNumber = (digitCount > 0) || hasDecimal; // consider . as a started number
        } else {
            outVal = display;
            hadNumber = false;
        }
    }

    void pressKey(const string &key) {
        if (errorState && key != "C") return;

        if (key == "C") {
            clear();
            return;
        }

        if (isDigitKey(key)) {
            // digit key
            if (!enteringNumber) {
                enteringNumber = true;
                currentInput.clear();
                hasDecimal = false;
                digitCount = 0;
            }
            // ignore extra digits beyond 8
            if (digitCount >= 8) return;

            char d = key[0];
            // ignore leading zero when it's the first digit and display is zero and no decimal started
            if (d == '0' && currentInput.empty() && !hasDecimal && display == 0.0L) {
                return;
            }
            currentInput.push_back(d);
            digitCount++;
            // update display
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
                // even if no digits, display should reflect 0.
                display = stold_safe(currentInput);
            }
            return;
        }

        // operator or '='
        if (key == "+" || key == "-" || key == "x" || key == ":" || key == "=") {
            bool hadNumber = false;
            ld parsed = 0.0L;
            finalizeCurrentNumberIfAny(parsed, hadNumber);

            if (hadNumber) {
                // if there was a number being entered, use it to compute (or set accumulator)
                if (pendingOp == '=') {
                    accumulator = parsed;
                } else {
                    applyOp(pendingOp, parsed);
                }
                // for repeating =
                lastOperand = parsed;
                lastOp = (pendingOp == '=') ? '=' : pendingOp;
            } else {
                // no new number typed; if key is '=' and lastOp set -> repeat last operation
                if (key == "=") {
                    if (lastOp != '=' ) {
                        // repeat last operation: accumulator (lastOp) lastOperand
                        applyOp(lastOp, lastOperand);
                    } else {
                        // nothing to do: pressing = when no pending op and no lastOp -> keep accumulator
                    }
                } else {
                    // operator typed consecutively -> change pendingOp to last operator pressed (ignore previous)
                    // no compute here
                }
            }

            // update pendingOp and display after operation
            if (errorState) return;

            if (key == "=") {
                pendingOp = '=';
                // display = accumulator (but if lastOp was '=' maybe accumulator already)
                display = accumulator;
                // set lastOp to the operator used for repetition (if we just computed with pendingOp)
                if (hadNumber) {
                    // set lastOp to pendingOp before it was set to '=' if it was an arithmetic op
                    // but we've already set lastOp above
                } else {
                    // handled above
                }
            } else {
                // set new pending operator to this one
                pendingOp = key[0];
                if (hadNumber) display = accumulator;
            }

            // reset number entry state
            enteringNumber = false;
            currentInput.clear();
            hasDecimal = false;
            digitCount = 0;
        }
    }

    string getDisplay() {
        if (errorState) {
            return string(4, ' ') + "Error.";
        }
        ld value = display;
        ld absValue = fabsl(value);

        // tiny -> zero
        if (absValue > 0.0L && absValue < 1e-100L) {
            value = 0.0L;
            absValue = 0.0L;
        }
        // overflow
        if (absValue >= 1e100L) {
            errorState = true;
            return string(4, ' ') + "Error.";
        }

        // Try normal formatting first. If it would print zero but value is nonzero (>1e-100), use scientific.
        string normal = formatNormal(value);
        // if normal shows 0 (all digits zero) and actual value not zero -> maybe scientific
        bool normalIsZero = (normal.find_first_not_of(' ') != string::npos) && (normal.find_first_not_of(' ') == normal.find('0')) && normal.find('.') != string::npos &&
                            normal.find_first_not_of(' ') + 1 == normal.find('.');
        // simpler: check if after trimming spaces it's "0."
        string trimmed = normal;
        // trim left spaces
        trimmed.erase(0, trimmed.find_first_not_of(' '));
        if (trimmed.empty()) trimmed = "";
        if (trimmed == "0." || trimmed == "-0.") {
            normalIsZero = true;
        } else normalIsZero = false;

        if (normalIsZero && fabsl(value) > 1e-100L) {
            // use scientific
            return formatScientific(value);
        }
        return normal;
    }

private:
    string formatNormal(ld value) {
        // produce fixed format with up to 8 digits (digit count includes integer digits)
        // width total will be right-justified into 10 chars (including '.' and optional '-')
        ld absValue = fabsl(value);
        int intDigits;
        if (absValue >= 1.0L) {
            intDigits = (int)floor(log10(absValue)) + 1;
        } else {
            intDigits = 1; // leading zero
        }
        int decPlaces = 8 - intDigits;
        if (decPlaces < 0) {
            // cannot represent normally -> use scientific
            return formatScientific(value);
        }
        if (decPlaces > 7) decPlaces = 7;

        // round to decPlaces
        ld pow10 = 1.0L;
        for (int i = 0; i < decPlaces; ++i) pow10 *= 10.0L;
        ld rounded = roundl(value * pow10) / pow10;

        // after rounding, check overflow/underflow
        if (fabsl(rounded) >= 1e100L) return string(4, ' ') + "Error.";
        if (fabsl(rounded) > 0.0L && fabsl(rounded) < 1e-100L) rounded = 0.0L;

        // format fixed with decPlaces decimals
        ostringstream oss;
        oss.setf(ios::fixed);
        oss << setprecision(decPlaces) << (double)rounded;
        string s = oss.str();

        // remove trailing zeros after decimal, but keep decimal point
        size_t dot = s.find('.');
        if (dot != string::npos) {
            size_t last = s.size()-1;
            while (last > dot && s[last] == '0') --last;
            s = s.substr(0, last+1);
            // if decimal point is last char, keep it (we must always display '.')
            if (s.back() == '.') {
                // keep it
            }
        } else {
            s += '.';
        }

        // right-justify to width 10
        while ((int)s.size() < 10) s = " " + s;
        return s;
    }

    string formatScientific(ld value) {
        // handle zero separately
        if (fabsl(value) == 0.0L) {
            string z = "0.";
            while ((int)z.size() < 10) z = " " + z;
            return z;
        }
        int exp = 0;
        ld mant = fabsl(value);
        exp = (int)floor(log10(mant));
        mant = mant / powl(10.0L, exp);
        // ensure mant is in [1,10)
        if (mant >= 10.0L) { mant /= 10.0L; ++exp; }
        // we need sign and up to 4 decimals in mantissa with rounding
        // round mant to 4 decimals
        ld mant_rounded = roundl(mant * 1e4L) / 1e4L;
        if (mant_rounded >= 10.0L) { mant_rounded /= 10.0L; ++exp; }

        // build mantissa string with up to 4 decimals (trim trailing zeros)
        ostringstream oss;
        oss.setf(ios::fixed);
        oss << setprecision(4) << (double)mant_rounded;
        string mantstr = oss.str();
        // trim trailing zeros but keep decimal point; also keep exactly one digit before dot
        size_t dot = mantstr.find('.');
        if (dot != string::npos) {
            size_t last = mantstr.size()-1;
            while (last > dot && mantstr[last] == '0') --last;
            mantstr = mantstr.substr(0, last+1);
        }
        // add sign back
        if (value < 0) mantstr = "-" + mantstr;

        // exponent formatting: sign is '-' or ' ' (space) for positive exponent
        char expSign = (exp < 0) ? '-' : ' ';
        int aexp = abs(exp);
        // exponent must be two digits (leading zero if needed)
        char expbuf[8];
        sprintf(expbuf, "%c%02d", expSign, aexp);

        string res = mantstr + string(expbuf);
        // right-justify width 10
        while ((int)res.size() < 10) res = " " + res;
        return res;
    }
};


int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    int t;
    if (!(cin >> t)) return 0;
    string line;
    getline(cin, line); // consume endline

    for (int tc = 0; tc < t; ++tc) {
        getline(cin, line);
        Calculator calc;
        istringstream iss(line);
        string token;
        while (iss >> token) {
            if (token.size() >= 2 && token.front() == '[' && token.back() == ']') {
                string key = token.substr(1, token.size()-2);
                calc.pressKey(key);
            }
        }
        cout << calc.getDisplay() << "\n";
    }
    return 0;
}
