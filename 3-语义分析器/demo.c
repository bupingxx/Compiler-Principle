  int main (void){   
    int a = 1, b = 0b11;
    // test...
    a = a * b + a;
    a = a / (b - a);
    a = (a + b) % (a + a * b);

    /* everything good */
    return 0;
}