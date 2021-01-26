/*
    These classes demonatrate approximate code to calculate
    the fibonacci series iteratively and recursively using
    inheritance.
*/

class fibonacci() {

    protected int terms
    constructor(int)
}

fibonacci.constructor(t) {
    terms = t
}

class fibo_iter(fibonacci) {

    public void calculate()
    constructor(int)
}

class fibo_recr(fibonacci) {

    public void calculate()
    private int do_calc(int)
    constructor(int)
}

fibo_iter.constructor(t) {

    terms = t
}

fibo_iter.calculate() {

    int first = 0
    int second = 1
    int next

    for(int c = 0; c < terms; c ++) {
        if ( c <= 1) {
            next = c
        }
        else {
            next = first + second
            first = second
            second = next
            print("{0}, "(next))
        }
    }
    print("\n")
}

fibo_recr.constructor(t) {

    terms = t
}

fibo_recr.calculate() {

    int val = 0

    for(int c = 1; c < terms; c++) {
        print("{0}, "(do_calc(val)))
        val ++
    }
}

fibo_recr.do_calc(val) {

    if(val equ 0 or val equ 1) {
        return val
    }
    else {
        return (do_calc(val-1) + do_calc(val - 2))
    }
}

// end