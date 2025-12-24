#include "avtomat.h"

using namespace std;



unordered_map<string, OperatorInfo> operators;

static double factorial(double a) {
    if (a < 0 || floor(a) != a) throw runtime_error("Factorial only for non-negative integers");
    double r = 1;
    for (int i = 1; i <= static_cast<int>(a); ++i) r *= i;
    return r;
}

void initOperators() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    operators["+"] = { "+", 1, Assoc::Left, 2, [](double a,double b) { return a + b; } };
    operators["-"] = { "-", 1, Assoc::Left, 2, [](double a,double b) { return a - b; } };
    operators["*"] = { "*", 2, Assoc::Left, 2, [](double a,double b) { return a * b; } };
    operators["/"] = { "/", 2, Assoc::Left, 2, [](double a,double b) { if (b == 0) throw runtime_error("Division by zero"); return a / b; } };
    operators["^"] = { "^", 4, Assoc::Right, 2, [](double a,double b) { return pow(a,b); } };
    operators["u-"] = { "u-", 5, Assoc::Right, 1, [](double a,double) { return -a; } };
    operators["!"] = { "!", 5, Assoc::Left, 1, [](double a,double) { return factorial(a); } };
    operators["u!"] = { "u!", 6, Assoc::Right, 1, [](double a,double) { return factorial(a); } };
}

static inline bool isSpace(char c) { return std::isspace(static_cast<unsigned char>(c)); }
static inline bool isDigit(char c) { return std::isdigit(static_cast<unsigned char>(c)); }

/* конечный автомат, распознающий числа */
bool parseNumberDFA(const string& s, size_t& i, string& outNumber) {
    enum class State { Start, IntPart, Dot, FracPart, Accept, Error };
    State st = State::Start;
    size_t j = i;
    bool sawDigit = false;
    while (j < s.size()) {
        char c = s[j];
        switch (st) {
        case State::Start:
            if (isDigit(c)) { st = State::IntPart; sawDigit = true; ++j; }
            else if (c == '.') { st = State::Dot; ++j; }
            else st = State::Error;
            break;
        case State::IntPart:
            if (isDigit(c)) { sawDigit = true; ++j; }
            else if (c == '.') { st = State::Dot; ++j; }
            else st = State::Accept;
            break;
        case State::Dot:
            if (isDigit(c)) { st = State::FracPart; sawDigit = true; ++j; }
            else st = State::Accept; 
            break;
        case State::FracPart:
            if (isDigit(c)) ++j;
            else st = State::Accept;
            break;
        default:
            st = State::Error; break;
        }
        if (st == State::Accept || st == State::Error) break;
    }
    if (st == State::IntPart || st == State::FracPart || st == State::Dot) st = State::Accept;
    if (st != State::Accept || !sawDigit) return false;
    outNumber = s.substr(i, j - i);
    i = j;
    return true;
}

// вспомогательные функции (упрощение лексического анализа)
inline bool isOperatorChar(char c) {
    return operators.count(string(1, c)) != 0;
}

inline string resolveOperator(char c, const vector<Token>& tokens, TokenType prevType) {
    string op(1, c);
    if (c == '-') {
        if (tokens.empty() || prevType == TokenType::Op || prevType == TokenType::LParen) op = "u-";
    }
    if (c == '!') {
        op = (prevType == TokenType::Number || prevType == TokenType::RParen) ? "!" : "u!";
    }
    return op;
}

// лексический анализ (преобразуем в последовательность токенов)
vector<Token> lex(const string& line) {
    if (operators.empty()) initOperators(); 
    vector<Token> tokens;
    size_t i = 0;
    TokenType prevType = TokenType::Op;
    const size_t n = line.size();

    while (i < n) {
        unsigned char uc = static_cast<unsigned char>(line[i]);
        if (isSpace(uc)) { ++i; continue; }

        if (isDigit(uc) || line[i] == '.') {
            string num;
            size_t start = i;
            if (!parseNumberDFA(line, i, num))
                throw runtime_error("Invalid number near: " + line.substr(start, min<size_t>(10, n - start)));
            tokens.emplace_back(TokenType::Number, num);
            prevType = TokenType::Number;
            continue;
        }

        if (line[i] == '(') { tokens.emplace_back(TokenType::LParen, "("); ++i; prevType = TokenType::LParen; continue; }
        if (line[i] == ')') { tokens.emplace_back(TokenType::RParen, ")"); ++i; prevType = TokenType::RParen; continue; }

        char c = line[i];
        if (isOperatorChar(c)) {
            string op = resolveOperator(c, tokens, prevType);
            tokens.emplace_back(TokenType::Op, op);
            prevType = (op == "!") ? TokenType::RParen : TokenType::Op;
            ++i;
            continue;
        }

        throw runtime_error("Unknown character at position " + to_string(i) + ": '" + string(1, c) + "'");
    }
    return tokens;
}

/* синтаксический анализ */
void validateTokens(const vector<Token>& tokens) {
    if (tokens.empty()) throw runtime_error("Empty expression");
    int balance = 0;
    TokenType prev = TokenType::Op;
    for (size_t idx = 0; idx < tokens.size(); ++idx) {
        const Token& t = tokens[idx];
        if (t.type == TokenType::Number) {
            if (prev == TokenType::Number || prev == TokenType::RParen) throw runtime_error("Unexpected number at position " + to_string(idx));
            prev = TokenType::Number;
        }
        else if (t.type == TokenType::LParen) {
            if (prev == TokenType::Number || prev == TokenType::RParen) throw runtime_error("Unexpected '(' at position " + to_string(idx));
            ++balance; prev = TokenType::LParen;
        }
        else if (t.type == TokenType::RParen) {
            if (prev == TokenType::Op || prev == TokenType::LParen) throw runtime_error("Unexpected ')' at position " + to_string(idx));
            --balance; if (balance < 0) throw runtime_error("Mismatched parentheses");
            prev = TokenType::RParen;
        }
        else {  
            const string& opk = t.text;
            bool isPref = (opk == "u-" || opk == "u!");
            bool isPost = (opk == "!");
            bool isBin = !isPref && !isPost;
            if (isBin) {
                if (prev == TokenType::Op || prev == TokenType::LParen) throw runtime_error("Unexpected binary operator '" + opk + "' at position " + to_string(idx));
                prev = TokenType::Op;
            }
            else if (isPref) {
                if (prev == TokenType::Number || prev == TokenType::RParen) throw runtime_error("Unexpected prefix unary '" + opk + "' at position " + to_string(idx));
                prev = TokenType::Op;
            }
            else {  
                if (prev != TokenType::Number && prev != TokenType::RParen) throw runtime_error("Unexpected postfix unary '" + opk + "' at position " + to_string(idx));
                prev = TokenType::RParen;
            }
        }
    }
    if (balance != 0) throw runtime_error("Mismatched parentheses: not balanced");
    const Token& last = tokens.back();
    if (last.type == TokenType::Op) {
        auto it = operators.find(last.text);
        if (it == operators.end()) throw runtime_error("Unknown operator at end: " + last.text);
        if (it->second.arity == 2) throw runtime_error("Expression cannot end with a binary operator");
        if (last.text == "u-" || last.text == "u!") throw runtime_error("Expression cannot end with a prefix unary operator");
    }
}

/* алгоритм Дейкстры (преобразование в ОПН) */
vector<Token> toRPN(const vector<Token>& tokens) {
    vector<Token> output;
    vector<Token> opstack;
    for (const Token& t : tokens) {
        if (t.type == TokenType::Number) output.push_back(t);
        else if (t.type == TokenType::Op) {
            auto it = operators.find(t.text); if (it == operators.end()) throw runtime_error("Unknown operator: " + t.text);
            const OperatorInfo& op1 = it->second;
            while (!opstack.empty()) {
                Token top = opstack.back();
                if (top.type == TokenType::Op) {
                    auto it2 = operators.find(top.text); if (it2 == operators.end()) throw runtime_error("Unknown operator on stack");
                    const OperatorInfo& op2 = it2->second;
                    bool higher = (op2.precedence > op1.precedence) || (op2.precedence == op1.precedence && op1.assoc == Assoc::Left);
                    if (higher) { output.push_back(top); opstack.pop_back(); continue; }
                }
                break;
            }
            opstack.push_back(t);
        }
        else if (t.type == TokenType::LParen) opstack.push_back(t);
        else { 
            bool found = false;
            while (!opstack.empty()) {
                Token top = opstack.back(); opstack.pop_back();
                if (top.type == TokenType::LParen) { found = true; break; }
                output.push_back(top);
            }
            if (!found) throw runtime_error("Mismatched parentheses during RPN conversion");
        }
    }
    while (!opstack.empty()) {
        Token top = opstack.back(); opstack.pop_back();
        if (top.type == TokenType::LParen || top.type == TokenType::RParen) throw runtime_error("Mismatched parentheses at end of RPN conversion");
        output.push_back(top);
    }
    return output;
}

/* Вычисление */
double evalRPN(const vector<Token>& rpn) {
    vector<double> st;
    for (const Token& t : rpn) {
        if (t.type == TokenType::Number) st.push_back(stod(t.text));
        else {
            auto it = operators.find(t.text); if (it == operators.end()) throw runtime_error("Unknown operator during evaluation");
            const OperatorInfo& op = it->second;
            if (op.arity == 2) {
                if (st.size() < 2) throw runtime_error("Not enough operands");
                double b = st.back(); st.pop_back();
                double a = st.back(); st.pop_back();
                st.push_back(op.func(a, b));
            }
            else {
                if (st.empty()) throw runtime_error("Not enough operands for unary operator");
                double a = st.back(); st.pop_back();
                st.push_back(op.func(a, 0.0));
            }
        }
    }
    if (st.size() != 1) throw runtime_error("Invalid expression: leftover values on stack");
    return st.back();
}

/*  Построчная обработка */
double computeExpression(const string& line) {
    auto tokens = lex(line);
    validateTokens(tokens);
    auto rpn = toRPN(tokens);
    return evalRPN(rpn);
}



