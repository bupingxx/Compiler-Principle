  int main (void){   
    int a = 1, b = 0b11;
           
    a = a * b + a;
    a = a / (b - a);
    a = (a + b) % a;

    // test...
    char str[10] = "Testing...";
    for(int i = 0; i < 10; i++){
        printf("Hello...%s/n", str);
    }

    /* everything good */
    return 0;
}