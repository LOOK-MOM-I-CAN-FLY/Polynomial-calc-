#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <algorithm>

using namespace std;



//////////////////////////////
// modint.h
//////////////////////////////

// Template for performing arithmetic modulo MOD (for instance ℤₚ).
template<int MOD>
struct ModInt {
    int value;
    static constexpr int MOD_VALUE = MOD;
    
    ModInt(int v = 0) {
        value = v % MOD;
        if (value < 0)
            value += MOD;
    }
    ModInt(const ModInt &other) : value(other.value) {}

    ModInt& operator+=(const ModInt &other) {
        value += other.value;
        if(value >= MOD)
            value -= MOD;
        return *this;
    }
    ModInt& operator-=(const ModInt &other) {
        value -= other.value;
        if(value < 0)
            value += MOD;
        return *this;
    }
    ModInt& operator*=(const ModInt &other) {
        value = (int)((long long)value * other.value % MOD);
        return *this;
    }
    // Fast exponentiation (binary exponentiation) modulo MOD.
    ModInt pow(long long exp) const {
        ModInt base = *this;
        ModInt result(1);
        while(exp > 0) {
            if(exp & 1)
                result *= base;
            base *= base;
            exp /= 2;
        }
        return result;
    }
    // Assuming MOD is prime.
    ModInt inv() const {
        return pow(MOD - 2);
    }
    ModInt& operator/=(const ModInt &other) {
        *this *= other.inv();
        return *this;
    }
    friend ModInt operator+(ModInt a, const ModInt &b) { return a += b; }
    friend ModInt operator-(ModInt a, const ModInt &b) { return a -= b; }
    friend ModInt operator*(ModInt a, const ModInt &b) { return a *= b; }
    friend ModInt operator/(ModInt a, const ModInt &b) { return a /= b; }
    
    friend ostream& operator<<(ostream &os, const ModInt &m) {
        os << m.value;
        return os;
    }
    friend bool operator==(const ModInt &a, const ModInt &b) {
        return a.value == b.value;
    }
    friend bool operator!=(const ModInt &a, const ModInt &b) {
        return a.value != b.value;
    }
};