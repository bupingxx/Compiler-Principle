int main (void){   
    int a = 1, b = 0b11;
    int c = 0011, d = 0xFF;
    if(a<=d){       
        d -= a;
    }
    c=a+b;
    // test...
    char str[10] = "Testing...";
    char ch = '\t';
    for(int i = 0; i < 10; i++){
        printf("Hello...%s/n", str);
    }

    /* everything good */
    return 0;
}