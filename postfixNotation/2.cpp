#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

using namespace std;

// 연산자 우선순위 반환
int getPriority(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// 문자가 연산자인지 확인
bool isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || 
           (unsigned char)c == 0xe2; // UTF-8 en dash 첫 바이트
}

// 문자가 숫자인지 확인
bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

// 수식 유효성 검사
bool validateExpression(char* expr, int len) {
    int parenCount = 0;
    bool lastWasOperator = true; // 처음에는 연산자가 올 수 없음
    bool lastWasNumber = false;
    
    for (int i = 0; i < len; i++) {
        char c = expr[i];
        
        // 공백은 무시
        if (c == ' ') continue;
        
        // UTF-8 en dash 처리 (3바이트)
        if (i + 2 < len && (unsigned char)c == 0xe2 && 
            (unsigned char)expr[i+1] == 0x80 && (unsigned char)expr[i+2] == 0x93) {
            if (lastWasOperator) {
                return false;
            }
            lastWasOperator = true;
            lastWasNumber = false;
            i += 2; // 3바이트 문자이므로 2칸 더 건너뜀
            continue;
        }
        
        // 유효한 문자인지 확인
        if (!isDigit(c) && !isOperator(c) && c != '(' && c != ')') {
            return false;
        }
        
        // 숫자 처리
        if (isDigit(c)) {
            if (lastWasNumber) { // 연속된 숫자 (두 자리 이상)
                return false;
            }
            lastWasOperator = false;
            lastWasNumber = true;
        }
        // 연산자 처리
        else if (isOperator(c)) {
            if (lastWasOperator) { // 연속된 연산자
                return false;
            }
            lastWasOperator = true;
            lastWasNumber = false;
        }
        // 왼쪽 괄호
        else if (c == '(') {
            parenCount++;
            lastWasOperator = true; // 괄호 다음엔 숫자나 괄호가 와야 함
            lastWasNumber = false;
        }
        // 오른쪽 괄호
        else if (c == ')') {
            parenCount--;
            if (parenCount < 0 || lastWasOperator) { // 괄호 불일치 또는 빈 괄호
                return false;
            }
            lastWasOperator = false;
            lastWasNumber = false;
        }
    }
    
    // 괄호가 짝이 맞지 않거나 연산자로 끝나는 경우
    if (parenCount != 0 || lastWasOperator) {
        return false;
    }
    
    return true;
}

// 중위 표기법을 후위 표기법으로 변환
int infixToPostfix(char* infix, char* postfix, int len) {
    char stack[200];
    int top = -1;
    int j = 0;
    
    for (int i = 0; i < len; i++) {
        char c = infix[i];
        
        // 공백은 무시
        if (c == ' ') continue;
        
        // UTF-8 en dash 처리
        if (i + 2 < len && (unsigned char)c == 0xe2 && 
            (unsigned char)infix[i+1] == 0x80 && (unsigned char)infix[i+2] == 0x93) {
            while (top >= 0 && stack[top] != '(' && 
                   getPriority(stack[top]) >= getPriority('-')) {
                postfix[j++] = stack[top--];
                postfix[j++] = ' ';
            }
            stack[++top] = '-'; // 일반 마이너스로 변환
            i += 2;
            continue;
        }
        
        // 피연산자는 바로 출력
        if (isDigit(c)) {
            postfix[j++] = c;
            postfix[j++] = ' ';
        }
        // 왼쪽 괄호는 스택에 push
        else if (c == '(') {
            stack[++top] = c;
        }
        // 오른쪽 괄호를 만나면 왼쪽 괄호까지 pop
        else if (c == ')') {
            while (top >= 0 && stack[top] != '(') {
                postfix[j++] = stack[top--];
                postfix[j++] = ' ';
            }
            top--; // 왼쪽 괄호 제거
        }
        // 연산자 처리
        else if (isOperator(c)) {
            while (top >= 0 && stack[top] != '(' && 
                   getPriority(stack[top]) >= getPriority(c)) {
                postfix[j++] = stack[top--];
                postfix[j++] = ' ';
            }
            stack[++top] = c;
        }
    }
    
    // 스택에 남은 연산자 모두 pop
    while (top >= 0) {
        postfix[j++] = stack[top--];
        postfix[j++] = ' ';
    }
    
    postfix[j] = '\0';
    return j;
}

// 후위 표기법 계산
double evaluatePostfix(char* postfix, int len, bool& divByZero) {
    double stack[200];
    int top = -1;
    
    for (int i = 0; i < len; i++) {
        char c = postfix[i];
        
        if (c == ' ') continue;
        
        if (isDigit(c)) {
            stack[++top] = c - '0';
        }
        else if (isOperator(c)) {
            double b = stack[top--];
            double a = stack[top--];
            
            switch(c) {
                case '+': stack[++top] = a + b; break;
                case '-': stack[++top] = a - b; break;
                case '*': stack[++top] = a * b; break;
                case '/': 
                    if (b == 0) {
                        divByZero = true;
                        return 0;
                    }
                    stack[++top] = a / b; 
                    break;
            }
        }
    }
    
    return stack[top];
}

// 문자열 길이 계산
int getStringLength(char* str) {
    int len = 0;
    while (str[len] != '\0') len++;
    return len;
}

// 테스트 케이스 하나 처리
void processTestCase(char* buffer) {
    char postfix[200];
    
    // 길이 계산
    int len = getStringLength(buffer);
    
    cout << "************" << endl;
    cout << "입력 값 : " << buffer << endl;
    
    // 길이 검사
    if (len > 100) {
        cout << "후위 표기 : " << endl;
        cout << "결과 값 : input.txt의 수식이 올바르지 않습니다." << endl;
        cout << "************" << endl;
        return;
    }
    
    // 수식 유효성 검사
    if (!validateExpression(buffer, len)) {
        cout << "후위 표기 : " << endl;
        cout << "결과 값 : input.txt의 수식이 올바르지 않습니다." << endl;
        cout << "************" << endl;
        return;
    }
    
    // 중위 표기법을 후위 표기법으로 변환
    int postfixLen = infixToPostfix(buffer, postfix, len);
    cout << "후위 표기 : " << postfix << endl;
    
    // 후위 표기법 계산
    bool divByZero = false;
    double result = evaluatePostfix(postfix, postfixLen, divByZero);
    
    if (divByZero) {
        cout << "결과 값 : 0으로는 나눌 수 없습니다." << endl;
    } else {
        cout << "결과 값 : " << fixed << setprecision(1) << result << endl;
    }
    cout << "************" << endl;
}

int main() {
    char buffer[200];
    
    // 파일 읽기
    ifstream file("input.txt");
    if (!file.is_open()) {
        cout << "input.txt 파일을 열 수 없습니다." << endl;
        return 1;
    }
    
    // 각 줄을 읽어서 처리
    while (file.getline(buffer, 200)) {
        // 빈 줄은 건너뛰기
        if (getStringLength(buffer) == 0) {
            continue;
        }
        
        processTestCase(buffer);
    }
    
    file.close();
    return 0;
}
