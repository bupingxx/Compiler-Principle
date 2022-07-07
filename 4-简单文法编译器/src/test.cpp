#include <iostream>
using namespace std;

extern "C" int compare(int arg1, int arg2, int res);

int main(){
    cout << "Test compare:" << endl;
    int a = 1, b = 3, c = 0;
    for (int i = 0; i < 5; i++)
    {
        int res = compare(a, b, c);
        cout << "When a = " << a << ", b = " << b << "  ==>>  ";
        if (res == 2)
        {
            cout << "a = b" << endl;
        }
        else if (res == 1)
        {
            cout << "a > b" << endl;
        }
        else if (res == 0)
        {
            cout << "a < b" << endl;
        }
        else
        {
            cout << "Test fail." << endl;
            return -1;
        }
        a = a + 1;
        cout << endl;
    }
    cout << "Test success." << endl;
    return 0;
}