#include <cstdio>
#include <cmath>
#include <cstring>

class Calculator {
private:
    double currentValue;
    char currentInput[12];
    int inputLength;
    int digitCount;
    bool hasDot;
    bool hasInput;
    bool errorState;
    char lastOperator;
    
    double operation(char op, double a, double b) {
        double result;
        switch (op) {
            case '+': result = a + b; break;
            case '-': result = a - b; break;
            case 'x': result = a * b; break;
            case ':':
                if (b == 0.0) {
                    errorState = true;
                    return 0.0;
                }
                result = a / b;
                break;
            default: return a;
        }
        if (fabs(result) >= 1e100) {
            errorState = true;
            return 0.0;
        }
        return result;
    }
    
    void formatOutput(double x, char *output) {
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
            int exp = (int)floor(log10(abs_x));
            double rounding = abs_x / pow(10.0, exp);
            rounding = round(rounding * 10000.0) / 10000.0;
            
            if (rounding >= 10.0) {
                rounding /= 10.0;
                exp++;
            }
            
            char temp[20];
            sprintf(temp, "%.4f", rounding);
            
            size_t len = strlen(temp);
            size_t dot = 0;
            for (size_t j = 0; j < len; j++) {
                if (temp[j] == '.') {
                    dot = j;
                    break;
                }
            }
            
            if (dot > 0) {
                size_t lastNum = len - 1;
                while (lastNum > dot && temp[lastNum] == '0') {
                    lastNum--;
                }
                temp[lastNum + 1] = '\0';
            }
            
            char expSign = (exp >= 0) ? ' ' : '-';
            int expVal = (exp >= 0) ? exp : -exp;
            
            char result[20];
            if (negative)
                sprintf(result, "-%s%c%02d", temp, expSign, expVal);
            else
                sprintf(result, "%s%c%02d", temp, expSign, expVal);
            
            sprintf(output, "%10s", result);
        } else {
            int numInit = (abs_x < 1.0) ? 1 : (int)floor(log10(abs_x)) + 1;
            int numDecimal = 8 - numInit;
            if (numDecimal < 0) numDecimal = 0;
            
            char temp[20];
            sprintf(temp, "%.*f", numDecimal, abs_x);
            
            size_t len = strlen(temp);
            size_t dot = 0;
            for (size_t j = 0; j < len; j++) {
                if (temp[j] == '.') {
                    dot = j;
                    break;
                }
            }
            
            if (dot > 0) {
                size_t lastNum = len - 1;
                while (lastNum > dot && temp[lastNum] == '0')
                    lastNum--;
                temp[lastNum + 1] = '\0';
            } else {
                strcat(temp, ".");
            }
            
            char result[20];
            if (negative)
                sprintf(result, "-%s", temp);
            else
                strcpy(result, temp);
            
            sprintf(output, "%10s", result);
        }
    }
    
    void digitorDot(char c) {
        if (c == '.') {
            if (!hasInput) {
                currentInput[0] = '0';
                currentInput[1] = '.';
                inputLength = 2;
                digitCount = 1;
                hasDot = hasInput = true;
            } else if (!hasDot) {
                currentInput[inputLength++] = '.';
                hasDot = true;
            }
        } else if (c == '0') {
            if (!hasInput) {
                currentInput[0] = '0';
                inputLength = digitCount = 1;
                hasInput = true;
            } else if (!(inputLength == 1 && currentInput[0] == '0' && !hasDot) && digitCount < 8) {
                currentInput[inputLength++] = c;
                digitCount++;
            }
        } else {
            if (!hasInput) {
                currentInput[0] = c;
                inputLength = digitCount = 1;
                hasInput = true;
            } else if (inputLength == 1 && currentInput[0] == '0' && !hasDot) {
                currentInput[0] = c;
            } else if (digitCount < 8) {
                currentInput[inputLength++] = c;
                digitCount++;
            }
        }
    }
    
    void operators(char op) {
        if (hasInput && inputLength > 0) {
            currentInput[inputLength] = '\0';
            double num = atof(currentInput);
            
            if (lastOperator != 0)
                currentValue = operation(lastOperator, currentValue, num);
            else
                currentValue = num;
            
            inputLength = digitCount = 0;
            hasDot = hasInput = false;
        }
        lastOperator = (op == '=') ? 0 : op;
    }
    
public:
    Calculator() {
        reset();
    }
    
    void reset() {
        currentValue = 0.0;
        inputLength = digitCount = 0;
        hasDot = hasInput = errorState = false;
        lastOperator = 0;
    }
    
    void processButton(char button) {
        if (errorState && button != 'C') {
            return;
        }
        
        if (button == 'C') {
            reset();
        } else if (button == '.' || (button >= '0' && button <= '9')) {
            digitorDot(button);
        } else {
            operators(button);
        }
    }
    
    void getResult(char *output) {
        if (errorState)
            sprintf(output, "%10s", "Error.");
        else
            formatOutput(currentValue, output);
    }
};

int main() {
    int t;
    scanf("%d\n", &t);
    
    char input[2048];
    
    while (t--) {
        fgets(input, sizeof(input), stdin);
        
        Calculator calc;
        
        int len = strlen(input);
        for (int i = 0; i < len; i++) {
            if (input[i] != '[') continue;
            
            i++;
            if (i >= len) break;
            char c = input[i];
            
            calc.processButton(c);
            
            while (i < len && input[i] != ']') i++;
        }
        
        char output[16];
        calc.getResult(output);
        
        printf("%s\n", output);
    }
    
    return 0;
} 
