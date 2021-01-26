/*
    This demonstrates the approximate code to find and print the
    factorals for a number.
*/

class factors() {

    int number

    constructor(int)
    public void calculate()
}

factors.constructor(num) {
    number = num
}

factors.calculate() {

    int fact

    for(int c = 1; c < number; c++) {
        fact = fact * c
    }

    print("the factor is {0}\n"((string)fact))
}

class enter {}

// The constructor does not need to be declared if there are
// no parameters
enter.constructor() {

    factors facts = factors(123)
    facts.calculate()
}