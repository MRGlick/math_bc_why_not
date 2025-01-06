#include <stdio.h>
#include <math.h>

typedef struct Fraction {
    int numerator, denominator;
} Fraction;

#define frac(d) fmod(d, 1.0)
#define min(a, b) ((a < b ? a : b))
#define max(a, b) ((a > b ? a : b))
#define abs(a) ((a < 0 ? -a : a))

double root(double x, int n);

Fraction reduce_fraction(Fraction f) {
    int min_ = min(f.denominator, f.numerator);

    for (int i = 1; i < min_; i++) {
        if (f.denominator % i == 0 && f.numerator % i == 0) {
            f.denominator /= i;
            f.numerator /= i;
        }
    }

    return f;
}

Fraction double_to_fraction(double d) {
    
    double multiplier = 1;
    while (frac(d * multiplier) > 0) {
        multiplier *= 10;
    }
    
    Fraction f = {
        .numerator = d * multiplier, 
        .denominator = multiplier
    };

    return reduce_fraction(f);
}

double fraction_to_double(Fraction f) {
    return (double)f.numerator / f.denominator;
}


double my_pow(double x, int n) {
    if (n == 0) return 1;
    if (n > 0) {
        double prod = 1;
        for (int i = 0; i < n; i++) {
            prod *= x;
        }
        return prod;
    }

    double prod = 1;
    for (int i = 0; i < -n; i++) {
        prod /= x;
    }

    return prod;
}

double f(double x, double k, int n) {
    return my_pow(x, n) - k;
}

double df(double x, int n) {
    return my_pow(x, n - 1) * x;
}

double root_iter(double guess, double num, int n) {
    double m = df(guess, n);
    double b = f(guess, num, n) - m * guess;

    return -b/m;
}

double root(double x, int n) {
    int root_iters = max(500, (int)(x * 1.2));

    double res = x;
    for (int i = 0; i < root_iters; i++) {
        res = root_iter(res, x, n);
    }

    return res;
}

double super_pow(double x, double n) {
    Fraction frac = double_to_fraction(n);

    printf("frac: %d/%d \n", frac.numerator, frac.denominator);


    double power = my_pow(x, frac.numerator);

    printf("%.2f ^ %d = %.2f", x, frac.numerator, power);

    return root(power, frac.denominator);
}

void root_test(int r) {

    int print_interval = 100;

    int print_counter = 0;
    double errors_sum = 0;

    for (long i = 0; i < 10000; i++) {
        double root_prediction = root(i, r);
        double prediction_number = my_pow(root_prediction, r);

        double error = abs(prediction_number - i);
        
        errors_sum += error;
        print_counter++;

        if (print_counter >= print_interval) {
            double avg_error = errors_sum / print_counter;
            printf("Average error over interval %ld-%ld: %.2f \n", i - print_counter + 1, i, avg_error);
            print_counter = 0;
            errors_sum = 0;
        }
    }
}


int main() {


    double x = 10;
    int n = 2;
    double nth_root = root(x, n);

    printf("%dth root of %.7f: %.2f \n", n, x, nth_root);
    printf("%.7f to the %dth power: %.7f \n", nth_root, n, my_pow(nth_root, n));


    int num = 55;
    int denom = 10;

    Fraction frac = {.numerator = num, .denominator = denom};
    Fraction reduced = reduce_fraction(frac);

    printf("Fraction: %d/%d, Reduced: %d/%d \n", frac.numerator, frac.denominator, reduced.numerator, reduced.denominator);

    Fraction from_d = double_to_fraction(5.125);


    printf("Fraction: %d/%d\n", from_d.numerator, from_d.denominator);


    double a = 5;
    double b = 2.5;

    printf("A: %.2f, B: %.2f, A^B: %.2f \n", a, b, super_pow(a, b));


    // printf("Root test: \n");
    // root_test(2);

    while (1) {};

}