#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

double apply_op(char op, double a, double b, bool &error) {
    double result;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case 'x': result = a * b; break;
        case ':': 
            if (b == 0) {
                error = true;
                return 0;
            }
            result = a / b;
            break;
    }
    if (fabs(result) >= 1e100) {
        error = true;
        return 0;
    }
    return result;
}

string format_fixed(double x) {
    if (x == 0.0) {
        return "0.";
    }
    bool negative = (x < 0);
    x = fabs(x);
    
    int integer_digits = (x < 1.0) ? 1 : static_cast<int>(floor(log10(x))) + 1;
    int decimal_places = 8 - integer_digits;
    if (decimal_places < 0) decimal_places = 0;
    
    char buffer[50];
    sprintf(buffer, "%.*f", decimal_places, x);
    string s(buffer);
    
    size_t dot = s.find('.');
    if (dot != string::npos) {
        s.erase(s.find_last_not_of('0') + 1);
        if (s.back() == '.') {
            // keep the decimal point
        }
    } else {
        s += '.';
    }
    
    if (negative) {
        s = '-' + s;
    }
    return s;
}

string format_scientific(double x) {
    if (x == 0.0) {
        return "0.";
    }
    bool negative = (x < 0);
    x = fabs(x);
    
    int exponent = static_cast<int>(floor(log10(x)));
    double mantissa = x / pow(10.0, exponent);
    
    mantissa = round(mantissa * 10000.0) / 10000.0;
    
    if (mantissa >= 10.0) {
        mantissa /= 10.0;
        exponent++;
    }
    
    char buffer[20];
    sprintf(buffer, "%.4f", mantissa);
    string s_m(buffer);
    
    size_t dot = s_m.find('.');
    if (dot != string::npos) {
        s_m.erase(s_m.find_last_not_of('0') + 1);
        if (s_m.back() == '.') {
            // Keep decimal point, no trailing zeros
        }
    }
    
    string exp_str;
    if (exponent >= 0) {
        exp_str = " ";
    } else {
        exp_str = "-";
        exponent = -exponent;
    }
    char exp_buffer[3];
    sprintf(exp_buffer, "%02d", exponent);
    exp_str += exp_buffer;
    
    string result = s_m + exp_str;
    if (negative) {
        result = '-' + result;
    }
    return result;
}

string format_value(double x) {
    if (fabs(x) <= 1e-100) {
        return "0.";
    } else if (fabs(x) >= 1e100) {
        return "Error.";
    } else {
        double abs_x = fabs(x);
        
        // Check if we need scientific notation
        // If rounded integer needs > 8 digits, use scientific
        double rounded_int = round(abs_x);
        if (rounded_int >= 1e8) {
            return format_scientific(x);
        }
        
        // Check if rounding to 8 digits would give all zeros for small numbers
        if (abs_x < 5e-8) {
            // Would round to 0 in fixed format with 8 digits
            if (abs_x > 1e-100) {
                return format_scientific(x);
            } else {
                return "0.";
            }
        }
        
        return format_fixed(x);
    }
}

string pad_to_10(const string &s) {
    int len = s.length();
    if (len < 10) {
        return string(10 - len, ' ') + s;
    } else {
        return s;
    }
}

int main() {
    int t;
    cin >> t;
    string line;
    getline(cin, line); // skip newline after t
    
    for (int i = 0; i < t; i++) {
        getline(cin, line);
        stringstream ss(line);
        string key;
        
        double current_value = 0.0;
        string current_input = "";
        char last_operator = 0;
        bool error = false;
        bool has_input = false; // Track if we're currently inputting a number

        while (ss >> key) {
            if (error && key != "[C]") {
                continue;
            }
            
            if (key == "[C]") {
                current_value = 0.0;
                current_input = "";
                last_operator = 0;
                error = false;
                has_input = false;
            } else {
                char c = key[1];
                
                if (c == '.' || (c >= '0' && c <= '9')) {
                    // Digit or decimal point
                    
                    if (c == '.') {
                        if (!has_input) {
                            // Start new number with leading decimal point
                            current_input = "0.";
                            has_input = true;
                        } else if (current_input.find('.') == string::npos) {
                            // Add decimal point if not already present
                            current_input += '.';
                        }
                        // Otherwise ignore duplicate decimal point
                    } else if (c == '0') {
                        if (!has_input) {
                            // Start inputting, but don't add leading zeros yet
                            current_input = "0";
                            has_input = true;
                        } else if (current_input == "0") {
                            // Multiple leading zeros - ignore
                        } else {
                            // Count digits
                            int digit_count = 0;
                            for (char ch : current_input) {
                                if (ch >= '0' && ch <= '9') {
                                    digit_count++;
                                }
                            }
                            if (digit_count < 8) {
                                current_input += c;
                            }
                        }
                    } else {
                        // Non-zero digit
                        if (!has_input) {
                            current_input = "";
                            current_input += c;
                            has_input = true;
                        } else if (current_input == "0") {
                            // Replace leading zero
                            current_input = "";
                            current_input += c;
                        } else {
                            // Count digits
                            int digit_count = 0;
                            for (char ch : current_input) {
                                if (ch >= '0' && ch <= '9') {
                                    digit_count++;
                                }
                            }
                            if (digit_count < 8) {
                                current_input += c;
                            }
                        }
                    }
                } else {
                    // Operator or equal sign
                    
                    if (has_input && !current_input.empty()) {
                        // Process the current input
                        double num = stod(current_input);
                        if (last_operator != 0) {
                            current_value = apply_op(last_operator, current_value, num, error);
                        } else {
                            current_value = num;
                        }
                        current_input = "";
                        has_input = false;
                    }
                    
                    if (c == '=') {
                        last_operator = 0;
                    } else {
                        // It's an operator (+, -, x, :)
                        last_operator = c;
                    }
                }
            }
        }

        if (error) {
            cout << pad_to_10("Error.") << endl;
        } else {
            string output = format_value(current_value);
            output = pad_to_10(output);
            cout << output << endl;
        }
    }
    return 0;
}
