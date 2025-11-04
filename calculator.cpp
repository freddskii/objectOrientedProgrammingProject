#include <cstdio>
#include <cmath>
#include <cstring>

inline double apply_op(char op, double a, double b, bool &error) {
    double result;
    switch (op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case 'x': result = a * b; break;
        case ':': 
            if (b == 0.0) {
                error = true;
                return 0.0;
            }
            result = a / b;
            break;
        default: return a;
    }
    if (fabs(result) >= 1e100) {
        error = true;
        return 0.0;
    }
    return result;
}

void format_output(double x, char *output) {
    if (fabs(x) <= 1e-100) {
        sprintf(output, "%10s", "0.");
        return;
    }
    if (fabs(x) >= 1e100) {
        sprintf(output, "%10s", "Error.");
        return;
    }
    
    double abs_x = fabs(x);
    bool negative = (x < 0);
    
    if (round(abs_x) >= 1e8 || (abs_x < 5e-8 && abs_x > 1e-100)) {
        int exponent = (int)floor(log10(abs_x));
        double mantissa = abs_x / pow(10.0, exponent);
        mantissa = round(mantissa * 10000.0) / 10000.0;
        
        if (mantissa >= 10.0) {
            mantissa /= 10.0;
            exponent++;
        }
        
        char temp[20];
        sprintf(temp, "%.4f", mantissa);
        
        int len = strlen(temp);
        while (len > 0 && temp[len-1] == '0') {
            len--;
        }
        temp[len] = '\0';
        
        char exp_sign = (exponent >= 0) ? ' ' : '-';
        int exp_val = (exponent >= 0) ? exponent : -exponent;
        
        char result[20];
        sprintf(result, "%s%s%c%02d", 
                negative ? "-" : "",
                temp,
                exp_sign,
                exp_val);
        
        sprintf(output, "%10s", result);
    } else {
        int integer_digits = (abs_x < 1.0) ? 1 : (int)floor(log10(abs_x)) + 1;
        int decimal_places = 8 - integer_digits;
        if (decimal_places < 0) decimal_places = 0;
        
        char temp[20];
        sprintf(temp, "%.*f", decimal_places, abs_x);
        
        int len = strlen(temp);
        while (len > 0 && temp[len-1] == '0') {
            len--;
        }
        temp[len] = '\0';
        
        if (strchr(temp, '.') == NULL) {
            strcat(temp, ".");
        }
        
        char result[20];
        sprintf(result, "%s%s", 
                negative ? "-" : "",
                temp);
        
        sprintf(output, "%10s", result);
    }
}

int main() {
    int t;
    scanf("%d\n", &t);
    
    char line[4096];
    
    while (t--) {
        fgets(line, sizeof(line), stdin);
        
        double current_value = 0.0;
        char current_input[12];
        int input_len = 0;
        int digit_count = 0;
        bool has_dot = false;
        
        char last_operator = 0;
        bool error = false;
        bool has_input = false;
        
        int i = 0;
        while (line[i]) {
            if (line[i] == '[') {
                i++;
                if (!line[i]) break;
                
                char c = line[i];
                
                if (error && c != 'C') {
                    while (line[i] && line[i] != ']') i++;
                    if (line[i]) i++;
                    continue;
                }
                
                if (c == 'C') {
                    current_value = 0.0;
                    input_len = 0;
                    digit_count = 0;
                    has_dot = false;
                    last_operator = 0;
                    error = false;
                    has_input = false;
                } else if (c == '.' || (c >= '0' && c <= '9')) {
                    if (c == '.') {
                        if (!has_input) {
                            current_input[0] = '0';
                            current_input[1] = '.';
                            input_len = 2;
                            digit_count = 1;
                            has_dot = true;
                            has_input = true;
                        } else if (!has_dot) {
                            current_input[input_len++] = '.';
                            has_dot = true;
                        }
                    } else if (c == '0') {
                        if (!has_input) {
                            current_input[0] = '0';
                            input_len = 1;
                            digit_count = 1;
                            has_input = true;
                        } else if (input_len == 1 && current_input[0] == '0' && !has_dot) {
                        } else if (digit_count < 8) {
                            current_input[input_len++] = c;
                            digit_count++;
                        }
                    } else {
                        if (!has_input) {
                            current_input[0] = c;
                            input_len = 1;
                            digit_count = 1;
                            has_input = true;
                        } else if (input_len == 1 && current_input[0] == '0' && !has_dot) {
                            current_input[0] = c;
                        } else if (digit_count < 8) {
                            current_input[input_len++] = c;
                            digit_count++;
                        }
                    }
                } else {
                    if (has_input && input_len > 0) {
                        current_input[input_len] = '\0';
                        double num = atof(current_input);
                        
                        if (last_operator != 0) {
                            current_value = apply_op(last_operator, current_value, num, error);
                        } else {
                            current_value = num;
                        }
                        
                        input_len = 0;
                        digit_count = 0;
                        has_dot = false;
                        has_input = false;
                    }
                    
                    last_operator = (c == '=') ? 0 : c;
                }
                
                while (line[i] && line[i] != ']') i++;
            }
            if (line[i]) i++;
        }
        
        char output[16];
        if (error) {
            sprintf(output, "%10s", "Error.");
        } else {
            format_output(current_value, output);
        }
        printf("%s\n", output);
    }
    
    return 0;
}
