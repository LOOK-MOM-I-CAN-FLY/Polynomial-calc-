#include "polynomial.h"
#include "factor_ring.h"
#include "modint.h"



//////////////////////////////
// main.cpp
//////////////////////////////

int main() {
    cout << "Polynomial Calculator\n";
    cout << "Select an operation:\n";
    cout << "1. Addition of two polynomials\n";
    cout << "2. Subtraction of two polynomials\n";
    cout << "3. Multiplication of two polynomials\n";
    cout << "4. Division of two polynomials (quotient and remainder)\n";
    cout << "5. Exponentiation of a polynomial\n";
    cout << "6. Evaluate a polynomial at a given point\n";
    cout << "7. Factor ring operations (Field extension F[x]/(f(x)))\n";

    int op;
    cout << "Your choice: ";
    cin >> op;

    if(op >= 1 && op <= 6) {
        // For operations 1-6 we use type double for coefficients.
        Polynomial<double> p, q;
        double x;
        int exp;
        switch(op) {
            case 1:
                cout << "Polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Polynomial B:\n";
                q = read_polynomial<double>();
                cout << "A + B = " << p + q << "\n";
                break;
            case 2:
                cout << "Polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Polynomial B:\n";
                q = read_polynomial<double>();
                cout << "A - B = " << p - q << "\n";
                break;
            case 3:
                cout << "Polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Polynomial B:\n";
                q = read_polynomial<double>();
                cout << "A * B = " << p * q << "\n";
                break;
            case 4:
                cout << "Dividend polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Divisor polynomial B:\n";
                q = read_polynomial<double>();
                try {
                    auto [quotient, remainder] = p.divmod(q);
                    cout << "Quotient: " << quotient << "\n";
                    cout << "Remainder: " << remainder << "\n";
                } catch (const runtime_error &e) {
                    cout << "Error: " << e.what() << "\n";
                }
                break;
            case 5:
                cout << "Polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Enter a non-negative exponent: ";
                cin >> exp;
                cout << "A^" << exp << " = " << p.pow(exp) << "\n";
                break;
            case 6:
                cout << "Polynomial A:\n";
                p = read_polynomial<double>();
                cout << "Enter the value of x: ";
                cin >> x;
                cout << "A(" << x << ") = " << p.evaluate(x) << "\n";
                break;
        }
    } 
    else if(op == 7) {
        int prime;
        cout << "\nEnter a prime number for the field F = Z_p: ";
        cin >> prime;
        // Verify that the number is prime.
        bool isPrime = true;
        if(prime < 2)
            isPrime = false;
        for (int i = 2; i * i <= prime; i++) {
            if(prime % i == 0) {
                isPrime = false;
                break;
            }
        }
        if(!isPrime) {
            cout << prime << " is not a prime number.\n";
        } else {
            // For demonstration we support a limited set of primes.
            if(prime == 2) run_factor_ring<2>();
            else if(prime == 3) run_factor_ring<3>();
            else if(prime == 5) run_factor_ring<5>();
            else if(prime == 7) run_factor_ring<7>();
            else if(prime == 11) run_factor_ring<11>();
            else {
                cout << "Prime " << prime << " is not supported in this demo.\n";
            }
        }
    } else {
        cout << "Unknown operation!\n";
    }
    
    return 0;
}