#pragma once

#include <iostream>
#include <vector>
#include <stdexcept>
#include <tuple>
#include <algorithm>
#include <functional>
#include "polynomial.h"
#include "modint.h"

using namespace std;


//////////////////////////////
// factor_ring.h
//////////////////////////////

// Template class representing an element of the factor ring R[x]/(mod_poly).
// Internally, the element is stored as a polynomial reduced modulo mod_poly.
template<typename T>
class FactorRingElement {
public:
    Polynomial<T> poly;     // The polynomial representing the element.
    Polynomial<T> mod_poly; // The modulus polynomial (the ideal).

    FactorRingElement(const Polynomial<T>& poly, const Polynomial<T>& mod_poly)
        : mod_poly(mod_poly)
    {
        this->poly = poly % mod_poly;
    }
    // Default constructor (unit element).
    FactorRingElement() : mod_poly(Polynomial<T>(T(1))) {}

    FactorRingElement operator+(const FactorRingElement& other) const {
        if(mod_poly.coeffs != other.mod_poly.coeffs)
            throw runtime_error("Different moduli in factor ring addition");
        return FactorRingElement(poly + other.poly, mod_poly);
    }
    
    FactorRingElement operator-(const FactorRingElement& other) const {
        if(mod_poly.coeffs != other.mod_poly.coeffs)
            throw runtime_error("Different moduli in factor ring subtraction");
        return FactorRingElement(poly - other.poly, mod_poly);
    }
    
    FactorRingElement operator*(const FactorRingElement& other) const {
        if(mod_poly.coeffs != other.mod_poly.coeffs)
            throw runtime_error("Different moduli in factor ring multiplication");
        return FactorRingElement(poly * other.poly, mod_poly);
    }
    
    // Extended Euclidean algorithm for polynomials:
    // finds x and y such that a*x + b*y == gcd(a, b)
    static tuple<Polynomial<T>, Polynomial<T>, Polynomial<T>> extended_gcd(
        const Polynomial<T>& a, const Polynomial<T>& b)
    {
        if(b.degree() < 0) {
            return {a, Polynomial<T>(T(1)), Polynomial<T>(T(0))};
        }
        auto [q, r] = a.divmod(b);
        auto [g, x, y] = extended_gcd(b, r);
        return {g, y, x - q * y};
    }
    
    // Compute the inverse in the factor ring if it exists.
    // The inverse exists if gcd(poly, mod_poly) is a nonzero constant polynomial.
    FactorRingElement inv() const {
        auto [g, x, y] = extended_gcd(poly, mod_poly);
        if(g.degree() != 0)
            throw runtime_error("Inverse does not exist in this factor ring");
        T inv_g = g.coeffs[0].inv(); // Assuming that T provides an inv() method.
        return FactorRingElement(x * Polynomial<T>(inv_g), mod_poly);
    }
    
    FactorRingElement operator/(const FactorRingElement& other) const {
        if(mod_poly.coeffs != other.mod_poly.coeffs)
            throw runtime_error("Different moduli in factor ring division");
        return *this * other.inv();
    }
    
    FactorRingElement pow(unsigned int exponent) const {
        FactorRingElement result(Polynomial<T>(T(1)), mod_poly);
        FactorRingElement base = *this;
        while(exponent) {
            if(exponent & 1)
                result = result * base;
            base = base * base;
            exponent >>= 1;
        }
        return result;
    }
    
    friend ostream& operator<<(ostream &os, const FactorRingElement& elem) {
        os << elem.poly;
        return os;
    }
};

//////////////////////////////
// Irreducibility Check
//////////////////////////////

// A simple brute-force irreducibility check over a finite field F (with coefficients of type T).
// This function assumes that T (for example, ModInt<...>) provides a static constant MOD_VALUE.
template<typename T>
bool is_irreducible(const Polynomial<T>& poly) {
    int deg = poly.degree();
    if(deg <= 0)
        return false;
    if(deg == 1)
        return true;
    int field_mod = T::MOD_VALUE;
    // Check for any nontrivial factor (of degree from 1 up to deg/2).
    for (int d = 1; d <= deg / 2; d++) {
        vector<T> coeffs(d, T(0));
        // Use a recursive lambda to generate all monic candidate divisors of degree d.
        function<bool(int)> gen = [&](int pos) -> bool {
            if (pos == d) {
                vector<T> candidate_coeffs = coeffs;
                candidate_coeffs.push_back(T(1)); // force monic coefficient
                Polynomial<T> candidate(candidate_coeffs);
                auto rem = poly.divmod(candidate).second;
                if(rem.degree() < 0)
                    return true; // candidate divides poly exactly -> poly is reducible.
                return false;
            }
            for (int i = 0; i < field_mod; i++) {
                coeffs[pos] = T(i);
                if (gen(pos + 1))
                    return true;
            }
            return false;
        };
        if(gen(0))
            return false; // found a divisor, so poly is reducible.
    }
    return true;
}

//////////////////////////////
// Helper function to read a polynomial from input.
//////////////////////////////

// The user is prompted first for the number of coefficients, and then each coefficient
// (starting with the constant term, i.e. coefficient for x^0).
template<typename T>
Polynomial<T> read_polynomial() {
    int n;
    cout << "Enter the number of coefficients: ";
    cin >> n;
    vector<T> coeff(n);
    cout << "Enter the coefficients (constant term first): ";
    for (int i = 0; i < n; i++) {
        int tmp;
        cin >> tmp;
        coeff[i] = T(tmp);
    }
    return Polynomial<T>(coeff);
}

//////////////////////////////
// Factor Ring Operations (Field Extension F[x]/(f(x)))
//////////////////////////////

// Sets up the factor ring F[x]/(f(x)) over a given prime field F = Z_p.
// The user is prompted for an irreducible polynomial f(x) over F and then for two elements.
// Operations like addition, subtraction, multiplication, inversion, division and exponentiation are performed.
template<int P>
void run_factor_ring() {
    typedef ModInt<P> Field;
    cout << "\nFactor ring operations over field Z" << P << ":\n";
    Polynomial<Field> f;
    bool irreducible_valid = false;
    while(!irreducible_valid) {
        cout << "Enter the polynomial f(x) (coefficients as constant term first):\n";
        f = read_polynomial<Field>();
        if(is_irreducible(f)) {
            irreducible_valid = true;
        } else {
            cout << "The polynomial f(x) is reducible over Z" << P
                 << ". Please enter an irreducible polynomial.\n";
        }
    }
    cout << "Using factor ring F[x]/(f(x)) where F = Z" << P << ":\n";
    cout << "Enter the first element (polynomial with coefficients in Z" << P << "):\n";
    Polynomial<Field> a = read_polynomial<Field>();
    cout << "Enter the second element:\n";
    Polynomial<Field> b = read_polynomial<Field>();
    FactorRingElement<Field> elem1(a, f);
    FactorRingElement<Field> elem2(b, f);
    
    cout << "Element A = " << elem1 << "\n";
    cout << "Element B = " << elem2 << "\n";
    
    cout << "\nA + B = " << (elem1 + elem2) << "\n";
    cout << "A - B = " << (elem1 - elem2) << "\n";
    cout << "A * B = " << (elem1 * elem2) << "\n";
    try {
        FactorRingElement<Field> invA = elem1.inv();
        cout << "Inverse of A = " << invA << "\n";
        cout << "A / B = " << (elem1 / elem2) << "\n";
    } catch(const runtime_error& e) {
        cout << "Error computing inverse: " << e.what() << "\n";
    }
    int exp;
    cout << "Enter an exponent for computing A^exp: ";
    cin >> exp;
    cout << "A^" << exp << " = " << elem1.pow(exp) << "\n";
}