#include <gtest/gtest.h>
#include "avtomat.h"

class CalcTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        initOperators();
    }
};

// -------------------- Arithmetic --------------------
TEST_F(CalcTest, Arithmetic_Add) {
    EXPECT_DOUBLE_EQ(computeExpression("2+2"), 4);
}

TEST_F(CalcTest, Arithmetic_Subtract) {
    EXPECT_DOUBLE_EQ(computeExpression("10-3"), 7);
}

TEST_F(CalcTest, Arithmetic_Multiply) {
    EXPECT_DOUBLE_EQ(computeExpression("6*7"), 42);
}

TEST_F(CalcTest, Arithmetic_Divide) {
    EXPECT_NEAR(computeExpression("7/2"), 3.5, 1e-12);
}

TEST_F(CalcTest, Arithmetic_Power) {
    EXPECT_DOUBLE_EQ(computeExpression("2^3"), 8);
}

TEST_F(CalcTest, Arithmetic_Power_RightAssociative) {
    EXPECT_DOUBLE_EQ(computeExpression("2^3^2"), 512);
}

TEST_F(CalcTest, Arithmetic_MixedPrecedence_AddMul) {
    EXPECT_DOUBLE_EQ(computeExpression("2+3*4"), 14);
}

TEST_F(CalcTest, Arithmetic_MixedPrecedence_MulPower) {
    EXPECT_DOUBLE_EQ(computeExpression("2*3^2"), 18);
}

//  Factorial 
TEST_F(CalcTest, Factorial_Basic_3) {
    EXPECT_DOUBLE_EQ(computeExpression("3!"), 6);
}

TEST_F(CalcTest, Factorial_Zero) {
    EXPECT_DOUBLE_EQ(computeExpression("0!"), 1);
}

TEST_F(CalcTest, Factorial_Double_3Double) {
    EXPECT_DOUBLE_EQ(computeExpression("3!!"), 720); 
}

TEST_F(CalcTest, Factorial_AfterParentheses) {
    EXPECT_DOUBLE_EQ(computeExpression("(3)!"), 6);
}

TEST_F(CalcTest, Factorial_Negative_Throws) {
    EXPECT_THROW(computeExpression("(-3)!"), std::runtime_error);
}

TEST_F(CalcTest, Factorial_NonInteger_Throws) {
    EXPECT_THROW(computeExpression("3.5!"), std::runtime_error);
}

// Unary minus
TEST_F(CalcTest, Unary_Negation_Simple) {
    EXPECT_DOUBLE_EQ(computeExpression("-5"), -5);
}

TEST_F(CalcTest, Unary_Negation_WithBinary) {
    EXPECT_DOUBLE_EQ(computeExpression("5*-3"), -15);
}

TEST_F(CalcTest, Unary_DoubleNegation) {
    EXPECT_DOUBLE_EQ(computeExpression("--3"), 3);
}

TEST_F(CalcTest, Unary_Negation_WithParentheses) {
    EXPECT_DOUBLE_EQ(computeExpression("-(3+2)"), -5);
}

TEST_F(CalcTest, Unary_Negation_WithPower) {
    EXPECT_NEAR(computeExpression("-3^2"), 9.0, 1e-12);
}

TEST_F(CalcTest, Unary_Negation_Grouping) {
    EXPECT_DOUBLE_EQ(computeExpression("-(3^2)"), -9);
}

// Operators mix 
TEST_F(CalcTest, Operators_PlusMinus_Mix) {
    EXPECT_DOUBLE_EQ(computeExpression("10-2+3"), 11);
}

TEST_F(CalcTest, Operators_LeftAssociative_Minus) {
    EXPECT_DOUBLE_EQ(computeExpression("10-3-2"), 5);
}

TEST_F(CalcTest, Operators_DivideByZero_Throws) {
    EXPECT_THROW(computeExpression("5/0"), std::runtime_error);
}

TEST_F(CalcTest, Operators_ComplexCombination) {
    EXPECT_NEAR(computeExpression("5+2*3-4/2"), 9.0, 1e-12);
}

// Fractions and decimals 
TEST_F(CalcTest, Fractions_DecimalLiteral) {
    EXPECT_NEAR(computeExpression("0.5+0.25"), 0.75, 1e-12);
}

TEST_F(CalcTest, Fractions_LeadingDot) {
    EXPECT_NEAR(computeExpression(".5 + .25"), 0.75, 1e-12);
}

TEST_F(CalcTest, Formatting_TrailingZeros) {
    EXPECT_NEAR(computeExpression("1.2000 + 0.0300"), 1.23, 1e-12);
}

TEST_F(CalcTest, Fractions_SimpleDivision) {
    EXPECT_NEAR(computeExpression("1/4"), 0.25, 1e-12);
}

TEST_F(CalcTest, Fractions_NegativeDecimal) {
    EXPECT_NEAR(computeExpression("-0.5"), -0.5, 1e-12);
}

TEST_F(CalcTest, Fractions_PowerWithDecimal) {
    EXPECT_NEAR(computeExpression("2.5^2"), 6.25, 1e-12);
}

//  Parentheses
TEST_F(CalcTest, Parentheses_Simple) {
    EXPECT_DOUBLE_EQ(computeExpression("(2+3)"), 5);
}

TEST_F(CalcTest, Parentheses) {
    EXPECT_DOUBLE_EQ(computeExpression("((1+2)*3)"), 9);
}

TEST_F(CalcTest, Parentheses_Precedence) {
    EXPECT_DOUBLE_EQ(computeExpression("(2+3)*4"), 20);
}

TEST_F(CalcTest, Parentheses_WithUnary) {
    EXPECT_DOUBLE_EQ(computeExpression("- ( 2 + 3 )"), -5);
}

TEST_F(CalcTest, Parentheses_SpacesHandling) {
    EXPECT_DOUBLE_EQ(computeExpression("   2   *  ( 3 + 1 )  "), 8);
}

TEST_F(CalcTest, Parentheses_Empty_Throws) {
    EXPECT_THROW(computeExpression("()"), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_MissingClosing_Throws) {
    EXPECT_THROW(computeExpression("(2+3"), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_ExtraClosing_Throws) {
    EXPECT_THROW(computeExpression("2+3)"), std::runtime_error);
}

//  Error cases
TEST_F(CalcTest, Errors_UnknownCharacter) {
    EXPECT_THROW(computeExpression("2 & 3"), std::runtime_error);
}

TEST_F(CalcTest, Errors_DoubleOperator) {
    EXPECT_THROW(computeExpression("5**2"), std::runtime_error);
}

TEST_F(CalcTest, Errors_TrailingOperator) {
    EXPECT_THROW(computeExpression("2+3-"), std::runtime_error);
}

TEST_F(CalcTest, Errors_LeadingBinaryOperator) {
    EXPECT_THROW(computeExpression("+3"), std::runtime_error);
}

TEST_F(CalcTest, Errors_InvalidNumberFormat) {
    EXPECT_THROW(computeExpression("1..2"), std::runtime_error);
}

TEST_F(CalcTest, Errors_OnlySpaces) {
    EXPECT_THROW(computeExpression("   "), std::runtime_error);
}

// Combined 
TEST_F(CalcTest, Combined_Multiply_WithNegativePower) {
    EXPECT_NEAR(computeExpression("5*2^-3"), 0.625, 1e-12);
}

TEST_F(CalcTest, Combined_FactorialDivision) {
    EXPECT_NEAR(computeExpression("4! / (3! * 2)"), 2.0, 1e-12); 
}

TEST_F(CalcTest, Combined_PowerAndMultiply) {
    EXPECT_NEAR(computeExpression("2^(1+2) + 3*2"), 14.0, 1e-12);
}

TEST_F(CalcTest, Combined_MultiplyDivide) {
    EXPECT_NEAR(computeExpression("((2+3)*2) / 10"), 1, 1e-9);
}

//  Edge cases 
TEST_F(CalcTest, Edge_LargeExponent) {
    EXPECT_NEAR(computeExpression("10^2"), 100.0, 1e-12);
}

TEST_F(CalcTest, Edge_MultipleUnaryAndBinary) {
    EXPECT_DOUBLE_EQ(computeExpression("--(5) + -(-2)"), 7); 
}

TEST_F(CalcTest, Edge_UnaryMinusBeforeFactorial) {
    EXPECT_DOUBLE_EQ(computeExpression("-(3!)"), -6);
}

TEST_F(CalcTest, Edge_FactorialThenMultiply) {
    EXPECT_DOUBLE_EQ(computeExpression("3! * 2"), 12);
}

TEST_F(CalcTest, Edge_SequentialOperators) {
    EXPECT_DOUBLE_EQ(computeExpression("2+3*4-5/5"), 13);
}


TEST_F(CalcTest, Errors_UnexpectedPrefixBinary) {
    EXPECT_THROW(computeExpression("*2"), std::runtime_error);
}

TEST_F(CalcTest, Errors_TooManyNumbers) {
    EXPECT_THROW(computeExpression("2 3"), std::runtime_error);
}

TEST_F(CalcTest, Errors_MissingOperand) {
    EXPECT_THROW(computeExpression("2 + "), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_UnaryAndBinary) {
    EXPECT_DOUBLE_EQ(computeExpression("-(2+3)"), -5);
}

TEST_F(CalcTest, Parentheses_DeepNesting) {
    EXPECT_DOUBLE_EQ(computeExpression("((((1+2)*3)+4)/5)"), 2.6);
}

TEST_F(CalcTest, Parentheses_PrematureClosing_Throws) {
    EXPECT_THROW(computeExpression("(1+2))"), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_OperatorInEmpty_Throws) {
    EXPECT_THROW(computeExpression("(*)"), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_ComplexCorrect) {
    EXPECT_DOUBLE_EQ(computeExpression("((2+3)*(4+5)-6)/(1+2)"), 13);
}

TEST_F(CalcTest, Parentheses_NestedUnary) {
    EXPECT_DOUBLE_EQ(computeExpression("-((3+2)*(1+1))"), -10);
}

TEST_F(CalcTest, Parentheses_MultipleEmpty_Throws) {
    EXPECT_THROW(computeExpression("()()"), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_ClosingWithoutOpening_Throws) {
    EXPECT_THROW(computeExpression(")1+2("), std::runtime_error);
}

TEST_F(CalcTest, Parentheses_Many) {
    EXPECT_DOUBLE_EQ(computeExpression("((((3))))"), 3);
}

TEST_F(CalcTest, Parentheses_ComplexNesting) {
    EXPECT_NEAR(computeExpression("(2+(3*(4+5)))/5"), (2 + (3 * (9))) / 5.0, 1e-12);
}

TEST_F(CalcTest, Factorial_OnExpressionResult) {
    EXPECT_DOUBLE_EQ(computeExpression("(2+1)!"), 6);
}

TEST_F(CalcTest, Factorial_PowerPrecedence) {
    EXPECT_DOUBLE_EQ(computeExpression("3!^1"), 6);
}

//  Formatting
TEST_F(CalcTest, Formatting_LeadingTrailingSpaces) {
    EXPECT_DOUBLE_EQ(computeExpression("   42   "), 42);
}

TEST_F(CalcTest, Formatting_TabsNewlinesIgnored) {
    EXPECT_DOUBLE_EQ(computeExpression(" \t 6 \n * \t 7 "), 42);
}

TEST_F(CalcTest, Errors_MismatchedOperators) {
    EXPECT_THROW(computeExpression("(+2)"), std::runtime_error);
}

TEST_F(CalcTest, Errors_NonDigitInNumber) {
    EXPECT_THROW(computeExpression("12a3"), std::runtime_error);
}

// Precedence checks 
TEST_F(CalcTest, Precedence_Check1) {
    EXPECT_DOUBLE_EQ(computeExpression("1+2*3^2"), 19);
}

TEST_F(CalcTest, Precedence_Check2) {
    EXPECT_DOUBLE_EQ(computeExpression("-12+2*(3^2)"), 6);
}

// Input  
TEST_F(CalcTest, Errors_EmptyInput) {
    EXPECT_THROW(computeExpression(""), std::runtime_error);
}
