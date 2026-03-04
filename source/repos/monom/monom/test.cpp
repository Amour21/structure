#include <gtest/gtest.h>
#include <sstream>
#include <string>
#include <algorithm>
#include <cmath>
#include "monom.h"

static std::string norm_str(const std::string& s) {
    std::string r;
    r.reserve(s.size());
    for (char c : s) if (!std::isspace((unsigned char)c)) r.push_back(c);
    return r;
}

static std::string poly_to_str(const Polynomial& p) {
    std::ostringstream oss;
    oss << p;
    return norm_str(oss.str());
}

static double approx_eq(double a, double b, double eps = 1e-9) {
    return std::fabs(a - b) <= eps;
}

TEST(PolynomialParseTests, ParseSingleMonomial) {
    std::string s = "3x^2";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "3x^2");
}

TEST(PolynomialParseTests, ParseMultipleMonomials) {
    std::string s = "2x + 3y";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "2x+3y");
}

TEST(PolynomialParseTests, ParseNoSpaces) {
    std::string s = "2y^2z+3xy^2";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "2y^2z+3xy^2");
}

TEST(PolynomialParseTests, ParseNegativeCoeff) {
    std::string s = "-x^2";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "-x^2");
}

TEST(PolynomialParseTests, ParseDecimalCoeff) {
    std::string s = "1.5x^2";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "1.5x^2");
}

TEST(PolynomialOperations, ImplicitOneCoeff) {
    auto p = Polynomial::parse("x");
    EXPECT_EQ(poly_to_str(p), "x");
}

TEST(PolynomialParseTests, RepeatedVariableExponentsAccumulate) {
    std::string s = "x^2x^3";
    auto p = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(p), "x^5");
}

TEST(PolynomialOperations, SimpleAddition) {
    auto a = Polynomial::parse("x");
    auto b = Polynomial::parse("x");
    auto r = a + b;
    EXPECT_EQ(poly_to_str(r), "2x");
}

TEST(PolynomialOperations, SimpleSubtraction) {
    auto a = Polynomial::parse("5x");
    auto b = Polynomial::parse("2x");
    auto r = a - b;
    EXPECT_EQ(poly_to_str(r), "3x");
}

TEST(PolynomialOperations, MultiplyMonomials) {
    auto a = Polynomial::parse("3xy^2");
    auto b = Polynomial::parse("2xz");
    auto r = a * b;
    EXPECT_EQ(poly_to_str(r), "6x^2y^2z");
}

TEST(PolynomialProperties, Distributivity) {
    auto a = Polynomial::parse("2x");
    auto b = Polynomial::parse("x");
    auto c = Polynomial::parse("23");
    auto left = a * (b + c);
    auto right = (a * b) + (a * c);
    EXPECT_EQ(poly_to_str(left), poly_to_str(right));
}

TEST(PolynomialOperations, MultiplyByZeroConstant) {
    auto p = Polynomial::parse("x");
    auto r = p * 0.0;
    EXPECT_EQ(poly_to_str(r), "0");
    EXPECT_TRUE(approx_eq(r.evaluate(1, 1, 1), 0.0));
}

TEST(PolynomialOperations, DivisionByConstantViaReciprocal) {
    auto p = Polynomial::parse("x");
    auto r = p * 0.5;  
    EXPECT_EQ(poly_to_str(r), "0.5x");
}

TEST(PolynomialExceptions, ExponentOverflowThrows) {
    auto a = Polynomial::parse("x^1023");
    auto b = Polynomial::parse("x");
    EXPECT_THROW({
        auto r = a * b;
        (void)r;
        }, std::out_of_range);
}

TEST(PolynomialOperations, AdditiveInverseYieldsZero) {
    auto p = Polynomial::parse("2x+3y");
    auto r = p + (p * -1.0);
    EXPECT_EQ(poly_to_str(r), "0");
}

TEST(PolynomialProperties, AdditionCommutativity) {
    auto a = Polynomial::parse("x+2");
    auto b = Polynomial::parse("3y");
    auto ab = a + b;
    auto ba = b + a;
    EXPECT_EQ(poly_to_str(ab), poly_to_str(ba));
}

TEST(PolynomialProperties, MultiplicationCommutativity) {
    auto a = Polynomial::parse("3x");
    auto b = Polynomial::parse("2y");
    auto ab = a * b;
    auto ba = b * a;
    EXPECT_EQ(poly_to_str(ab), poly_to_str(ba));
}

TEST(PolynomialProperties, AdditionAssociativity) {
    auto a = Polynomial::parse("x");
    auto b = Polynomial::parse("y");
    auto c = Polynomial::parse("z");
    auto r1 = (a + b) + c;
    auto r2 = a + (b + c);
    EXPECT_EQ(poly_to_str(r1), poly_to_str(r2));
}

TEST(PolynomialEvaluation, EvaluateAtPoint) {
    auto p = Polynomial::parse("2x + 3y + 4");
    double val = p.evaluate(1.0, 1.0, 1.0);
    EXPECT_TRUE(approx_eq(val, 9.0));
}

TEST(PolynomialOperations, MultiplyByConstant) {
    auto p = Polynomial::parse("2x+3");
    auto r = p * 2.0;
    EXPECT_EQ(poly_to_str(r), "4x+6");
}

TEST(PolynomialOperations, DivideByConstant) {
    auto p = Polynomial::parse("4x+6");
    auto r = p * 0.5;
    EXPECT_EQ(poly_to_str(r), "2x+3");
}

TEST(PolynomialParseAndCompute, ChainMultiplication) {
    std::string s = "2x*3y*4z";
    auto r = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(r), "24xyz");
}

TEST(PolynomialParseTests, SignsAndCompactInput) {
    std::string s = "-2x+3y";
    auto r = Polynomial::parse(s);
    EXPECT_EQ(poly_to_str(r), "-2x+3y");
}

TEST(PolynomialEdgeCases, ZeroPolynomialParse) {
    auto p = Polynomial::parse("0");
    EXPECT_EQ(poly_to_str(p), "0");
    EXPECT_TRUE(approx_eq(p.evaluate(3.2, 4.5, 1.1), 0.0));
}

TEST(PolynomialEdgeCases, CancellationLeavesZero) {
    auto p = Polynomial::parse("x - x");
    EXPECT_EQ(poly_to_str(p), "0");
}

TEST(PolynomialOperations, CompressSameDegrees) {
    auto p = Polynomial::parse("x + x + x");
    EXPECT_EQ(poly_to_str(p), "3x");
}

TEST(PolynomialParseTests, VariableOrderIrrelevant) {
    auto p1 = Polynomial::parse("yx");
    auto p2 = Polynomial::parse("xy");
    EXPECT_EQ(poly_to_str(p1), poly_to_str(p2));
}

TEST(PolynomialIntegration, ComplexMultiplyContainsBothTerms) {
    auto left = Polynomial::parse("2y^2z + 3xy^2");
    auto right = Polynomial::parse("2xz");
    auto r = left * right;
    auto rs = poly_to_str(r);
    EXPECT_NE(rs.find("6x^2y^2z"), std::string::npos);
    EXPECT_NE(rs.find("4xy^2z^2"), std::string::npos);
}

TEST(PolynomialOperations, DecimalCoefficientsMultiplication) {
    auto p = Polynomial::parse("1.5x");
    auto q = Polynomial::parse("2");
    auto r = p * q;
    EXPECT_EQ(poly_to_str(r), "3x");
}

TEST(PolynomialEdgeCases, ZeroMonomIsIgnored) {
    auto p = Polynomial::parse("0x^2");
    EXPECT_EQ(poly_to_str(p), "0");
}