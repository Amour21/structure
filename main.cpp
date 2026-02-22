#include <iostream>
#include <iomanip>
#include "avtomat.h"

int main() {
    initOperators();
    std::cout << std::fixed << std::setprecision(10);

    std::string line;
    while (std::getline(std::cin, line)) {
        try {
            std::cout << computeExpression(line) << '\n';
        }
        catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}