#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <functional>
#include <cmath>
#include <stdexcept>
#include <iomanip>
#include <cctype>
#include "Vector.h"
#include "stack.h"
#include "queue.h"

using namespace std;

enum class TokenType { Number, Op, LParen, RParen };// типы токенов
enum class Assoc { Left, Right }; // ассоциативность операторов

struct OperatorInfo {
    string sym;
    int precedence;
    Assoc assoc;
    int arity; // 1 or 2
    function<double(double, double)> func;
};

struct Token {
    TokenType type;
    string text;
    Token(TokenType t = TokenType::Op, string s = "") : type(t), text(move(s)) {}
};

double computeExpression(const std::string& line);
std::vector<Token> lex(const std::string& line);
void validateTokens(const std::vector<Token>& tokens);
std::vector<Token> toRPN(const std::vector<Token>& tokens);
double evalRPN(const std::vector<Token>& rpn);
void initOperators();
