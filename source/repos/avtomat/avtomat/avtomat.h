#pragma once
#include <string>
#include <vector>

enum class TokenType;
struct Token;
double computeExpression(const std::string& line);
std::vector<Token> lex(const std::string& line);
void validateTokens(const std::vector<Token>& tokens);
std::vector<Token> toRPN(const std::vector<Token>& tokens);
double evalRPN(const std::vector<Token>& rpn);
void initOperators();
