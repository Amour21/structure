#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include <cmath>
#include <stdexcept>
#include "list.h"

struct Monom {
    double coeff;
    uint32_t deg;

    Monom(double c = 0.0, uint32_t d = 0) : coeff(c), deg(d) {}

    static uint32_t pack(uint32_t i, uint32_t j, uint32_t k) {
        if (i >= 1024 || j >= 1024 || k >= 1024)
            throw std::out_of_range("overflow");
        return (i << 20) | (j << 10) | k;
    }

    static void unpack(uint32_t deg, uint32_t& i, uint32_t& j, uint32_t& k) {
        uint32_t mask = (1 << 10) - 1;
        k = deg & mask;
        j = (deg >> 10) & mask;
        i = (deg >> 20) & mask;
    }

    bool operator==(const Monom& other) const {
        return deg == other.deg;
    }

    bool operator<(const Monom& other) const {
        uint32_t ix1, iy1, iz1;
        uint32_t ix2, iy2, iz2;
        unpack(deg, ix1, iy1, iz1);
        unpack(other.deg, ix2, iy2, iz2);
        if (ix1 != ix2) return ix1 > ix2;
        if (iy1 != iy2) return iy1 > iy2;
        if (iz1 != iz2) return iz1 > iz2;
        return false;
    }
};

class Polynomial {
private:
    ForwardList<Monom> data;

    void add_sorted(const Monom& m) {
        if (std::abs(m.coeff) < 1e-9) return;

        if (data.empty()) {
            data.push_front(m);
            return;
        }

        auto prev = data.end();
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->deg == m.deg) {
                it->coeff += m.coeff;
                if (std::abs(it->coeff) < 1e-9)
                    data.erase_after(prev);
                return;
            }
            if (it->deg > m.deg) {
                if (prev == data.end())
                    data.push_front(m);
                else
                    data.insert_after(prev, m);
                return;
            }
            prev = it;
        }

        data.insert_after(prev, m);
    }

    void add_back(const Monom& m) {
        if (std::abs(m.coeff) < 1e-9) return;
        auto prev = data.end();
        for (auto it = data.begin(); it != data.end(); ++it) {
            if (it->deg == m.deg) {
                it->coeff += m.coeff;
                if (std::abs(it->coeff) < 1e-9) {
                    data.erase_after(prev);  
                }
                return;
            }
            prev = it;
        }
        if (prev == data.end())
            data.push_front(m);
        else
            data.insert_after(prev, m);
    }

public:
    Polynomial() = default;

    friend Polynomial operator+(const Polynomial& a, const Polynomial& b) {
        Polynomial res;
        auto it1 = a.data.begin();
        auto it2 = b.data.begin();
        auto prev = res.data.end();

        while (it1 != a.data.end() && it2 != b.data.end()) {
            if (it1->deg == it2->deg) {
                double c = it1->coeff + it2->coeff;
                if (std::abs(c) > 1e-9) {
                    if (prev == res.data.end())
                        prev = res.data.push_front({ c, it1->deg });
                    else
                        prev = res.data.insert_after(prev, { c, it1->deg });
                }
                ++it1; ++it2;
            }
            else if (it1->deg < it2->deg) {
                if (prev == res.data.end())
                    prev = res.data.push_front(*it1);
                else
                    prev = res.data.insert_after(prev, *it1);
                ++it1;
            }
            else {
                if (prev == res.data.end())
                    prev = res.data.push_front(*it2);
                else
                    prev = res.data.insert_after(prev, *it2);
                ++it2;
            }
        }

        while (it1 != a.data.end()) {
            if (prev == res.data.end())
                prev = res.data.push_front(*it1);
            else
                prev = res.data.insert_after(prev, *it1);
            ++it1;
        }

        while (it2 != b.data.end()) {
            if (prev == res.data.end())
                prev = res.data.push_front(*it2);
            else
                prev = res.data.insert_after(prev, *it2);
            ++it2;
        }

        return res;
    }

    friend Polynomial operator-(const Polynomial& a, const Polynomial& b) {
        return a + (b * -1.0);
    }

    friend Polynomial operator*(const Polynomial& a, const Polynomial& b) {
        Polynomial res;

        for (auto& m1 : a.data) {
            for (auto& m2 : b.data) {

                uint32_t i1, j1, k1;
                uint32_t i2, j2, k2;

                Monom::unpack(m1.deg, i1, j1, k1);
                Monom::unpack(m2.deg, i2, j2, k2);

                uint32_t ni = i1 + i2;
                uint32_t nj = j1 + j2;
                uint32_t nk = k1 + k2;

                Monom r(
                    m1.coeff * m2.coeff,
                    Monom::pack(ni, nj, nk)
                );

                res.add_sorted(r);
            }
        }
        return res;
    }

    friend Polynomial operator*(const Polynomial& p, double c) {
        Polynomial res;
        for (auto& m : p.data)
            res.add_back({ m.coeff * c, m.deg });
        return res;
    }

    friend Polynomial operator*(double c, const Polynomial& p) {
        return p * c;
    }

    double evaluate(double x, double y, double z) const {
        double sum = 0;
        for (auto& m : data) {
            uint32_t i, j, k;
            Monom::unpack(m.deg, i, j, k);
            sum += m.coeff *
                std::pow(x, i) *
                std::pow(y, j) *
                std::pow(z, k);
        }
        return sum;
    }

    friend std::ostream& operator<<(std::ostream& os, const Polynomial& p) {
        if (p.data.empty()) {
            os << "0";
            return os;
        }

        bool first = true;
        for (auto& m : p.data) {

            if (!first && m.coeff > 0) os << "+";
            if (m.coeff < 0) os << "-";

            double c = std::abs(m.coeff);

            uint32_t i, j, k;
            Monom::unpack(m.deg, i, j, k);

            if (c != 1 || m.deg == 0)
                os << c;

            if (i) os << "x" << (i > 1 ? "^" + std::to_string(i) : "");
            if (j) os << "y" << (j > 1 ? "^" + std::to_string(j) : "");
            if (k) os << "z" << (k > 1 ? "^" + std::to_string(k) : "");

            first = false;
        }
        return os;
    }

    static Polynomial parse(const std::string& input) {
        Polynomial result;
        std::string s;
        for (char c : input)
            if (c != ' ') s += c;

        size_t pos_mul = s.find('*');
        if (pos_mul != std::string::npos) {
            return parse(s.substr(0, pos_mul)) *
                parse(s.substr(pos_mul + 1));
        }

        if (s.empty()) return result;

        if (s[0] != '-') s = "+" + s;

        for (size_t i = 0; i < s.size();) {
            int sign = (s[i] == '+') ? 1 : -1;
            ++i;

            double coeff = 0;
            bool has_coeff = false;

            if (i < s.size() && (std::isdigit((unsigned char)s[i]) || s[i] == '.')) {
                double int_part = 0.0;
                double frac_part = 0.0;
                double frac_div = 1.0;
                bool seen_dot = false;
                while (i < s.size() && (std::isdigit((unsigned char)s[i]) || s[i] == '.')) {
                    if (s[i] == '.') {
                        if (seen_dot) break;
                        seen_dot = true;
                    }
                    else {
                        int digit = s[i] - '0';
                        if (!seen_dot) {
                            int_part = int_part * 10.0 + digit;
                        }
                        else {
                            frac_part = frac_part * 10.0 + digit;
                            frac_div *= 10.0;
                        }
                    }
                    ++i;
                }
                coeff = int_part + (frac_div > 1.0 ? frac_part / frac_div : 0.0);
                has_coeff = true;
            }

            if (!has_coeff) coeff = 1;
            coeff *= sign;

            uint32_t ix = 0, iy = 0, iz = 0;

            while (i < s.size() && s[i] != '+' && s[i] != '-') {
                char var = s[i++];
                uint32_t deg = 1;

                if (i < s.size() && s[i] == '^') {
                    ++i;
                    deg = 0;
                    while (i < s.size() && std::isdigit((unsigned char)s[i])) {
                        deg = deg * 10 + (s[i] - '0');
                        ++i;
                    }
                }

                if (var == 'x') ix += deg;
                if (var == 'y') iy += deg;
                if (var == 'z') iz += deg;
            }

            if (ix >= 1024 || iy >= 1024 || iz >= 1024)
                throw std::out_of_range("exponent overflow");

            result.add_back({ coeff, Monom::pack(ix, iy, iz) });
        }

        return result;
    }

};