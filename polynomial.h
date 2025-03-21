#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <algorithm>


using namespace std;

//////////////////////////////
// polynomial.h
//////////////////////////////

// Template class for representing a polynomial with coefficients of type T.
// The polynomial is stored as a vector of coefficients where coeffs[i] corresponds to x^i.
template<typename T>
class Polynomial {
public:
    vector<T> coeffs;
    
    Polynomial() {}
    Polynomial(const vector<T>& c): coeffs(c) {
        normalize();
    }
    // Constructor for a constant polynomial.
    Polynomial(T constant) : coeffs(1, constant) {
        normalize();
    }
    
    // Remove trailing zero coefficients.
    void normalize(){
        while(!coeffs.empty() && coeffs.back() == T(0))
            coeffs.pop_back();
    }
    
    // Degree of the polynomial (the zero polynomial has degree -1).
    int degree() const {
        return coeffs.empty() ? -1 : coeffs.size()-1;
    }
    
    // Get the coefficient for x^i. Returns 0 if i is out of range.
    T operator[](int idx) const {
        return (idx < 0 || idx >= (int)coeffs.size()) ? T(0) : coeffs[idx];
    }
    
    // Addition.
    Polynomial operator+(const Polynomial& other) const {
        vector<T> result(max(coeffs.size(), other.coeffs.size()), T(0));
        for (size_t i = 0; i < result.size(); i++) {
            if (i < coeffs.size())
                result[i] = result[i] + coeffs[i];
            if (i < other.coeffs.size())
                result[i] = result[i] + other.coeffs[i];
        }
        return Polynomial(result);
    }
    Polynomial& operator+=(const Polynomial& other) {
        *this = *this + other;
        return *this;
    }
    
    // Subtraction.
    Polynomial operator-(const Polynomial& other) const {
        vector<T> result(max(coeffs.size(), other.coeffs.size()), T(0));
        for (size_t i = 0; i < result.size(); i++) {
            if (i < coeffs.size())
                result[i] = result[i] + coeffs[i];
            if (i < other.coeffs.size())
                result[i] = result[i] - other.coeffs[i];
        }
        return Polynomial(result);
    }
    Polynomial& operator-=(const Polynomial& other) {
        *this = *this - other;
        return *this;
    }
    
    // Multiplication.
    Polynomial operator*(const Polynomial& other) const {
        if(coeffs.empty() || other.coeffs.empty())
            return Polynomial();
        vector<T> result(coeffs.size() + other.coeffs.size() - 1, T(0));
        for (size_t i = 0; i < coeffs.size(); i++) {
            for (size_t j = 0; j < other.coeffs.size(); j++) {
                result[i+j] = result[i+j] + coeffs[i] * other.coeffs[j];
            }
        }
        return Polynomial(result);
    }
    Polynomial& operator*=(const Polynomial& other) {
        *this = *this * other;
        return *this;
    }
    
    // Division: returns a pair (quotient, remainder) such that A = divisor * quotient + remainder.
    pair<Polynomial, Polynomial> divmod(const Polynomial& divisor) const {
        if(divisor.coeffs.empty())
            throw runtime_error("Division by zero polynomial");
        Polynomial dividend = *this;
        Polynomial quotient;
        quotient.coeffs.resize(max(0, dividend.degree() - divisor.degree() + 1), T(0));
        while(dividend.degree() >= divisor.degree() && dividend.degree() >= 0) {
            int diff = dividend.degree() - divisor.degree();
            T factor = dividend.coeffs.back() / divisor.coeffs.back();
            vector<T> term(diff+1, T(0));
            term[diff] = factor;
            Polynomial<T> monomial(term);
            quotient.coeffs[diff] = factor;
            dividend = dividend - (divisor * monomial);
            dividend.normalize();
        }
        quotient.normalize();
        dividend.normalize();
        return {quotient, dividend};
    }
    
    // Returns the quotient of the division.
    Polynomial operator/(const Polynomial& divisor) const {
        return divmod(divisor).first;
    }
    
    // Returns the remainder after division.
    Polynomial operator%(const Polynomial& divisor) const {
        return divmod(divisor).second;
    }
    
    // Exponentiation to a nonnegative integer power.
    Polynomial pow(unsigned int exponent) const {
        Polynomial result(T(1)); // The unit (constant) polynomial.
        Polynomial base = *this;
        while(exponent) {
            if(exponent & 1)
                result = result * base;
            base = base * base;
            exponent >>= 1;
        }
        return result;
    }
    
    // Evaluate the polynomial at a given value x using Horner's method.
    T evaluate(const T& x) const {
        T result = T(0);
        for (int i = degree(); i >= 0; i--) {
            result = result * x + coeffs[i];
        }
        return result;
    }
    
    // Overload the output operator for pretty printing.
    friend ostream& operator<<(ostream &os, const Polynomial& poly) {
        if(poly.coeffs.empty()){
            os << "0";
            return os;
        }
        bool first = true;
        for(int i = poly.degree(); i >= 0; i--) {
            if(poly.coeffs[i] == T(0))
                continue;
            if(!first)
                os << " + ";
            first = false;
            if(i == 0) {
                os << poly.coeffs[i];
            } else {
                if(poly.coeffs[i] != T(1))
                    os << poly.coeffs[i] << "*";
                os << "x";
                if(i > 1)
                    os << "^" << i;
            }
        }
        if(first)
            os << "0";
        return os;
    }
};