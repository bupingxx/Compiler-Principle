#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

class Lexical{
private:
    // current state
    int state;

    // pre state
    int pre_state;

    // keyword set
    vector< pair<string, int> > keywords;

    // operator set
    vector< pair<string, int> > opers;

    // identify table
    vector<string> iT;

    // char table
    vector<char> cT;

    // string table
    vector<string> sT;

    // constant table
    vector<string> CT;

    // keyword table
    vector<string> KT;

    // operater table
    vector<string> PT;

    // results
    vector< pair<string, int> > results;
public:
    Lexical();
    ~Lexical();
    void print_table();
    void init_set();
    bool is_letter(char ch);
    bool is_digital(char ch);
    void analyse(const char* file_name);
    void state_change(int& state, char ch);
    void state_to_code(int pre_state, string token);
    void print_results();
};

Lexical::Lexical(){
    state = 0;
    pre_state = 0;
    init_set();
    //print_table();
}

Lexical::~Lexical(){
    keywords.clear();
    opers.clear();
    iT.clear();
    cT.clear();
    sT.clear();
    CT.clear();
    KT.clear();
    PT.clear();
}

// print the table of symbol-code
void Lexical::print_table(){
    string str(27, '*');
    cout << str << " Table of symbol and code: " << str << endl;
    cout << setiosflags(ios::right);
    cout << setw(10) << "symbol" << setw(10) << "code" << setw(10) << "symbol" << setw(10) << "code";
    cout << setw(10) << "symbol" << setw(10) << "code" << setw(10) << "symbol" << setw(10) << "code";
    cout << endl;
    cout << setw(10) << "iT" << setw(10) << 0 << setw(10) << "cT" << setw(10) << 1;
    cout << setw(10) << "sT" << setw(10) << 2 << setw(10) << "CT" << setw(10) << 3;
    cout << endl;
    int cnt = 0;
    for(auto item:keywords){
        cout << setw(10) << item.first << setw(10) << item.second;
        cnt++;
        if(cnt % 4 == 0)
            cout << endl;
    }
    for(auto item:opers){
        cout << setw(10) << item.first << setw(10) << item.second;
        cnt++;
        if(cnt % 4 == 0)
            cout << endl;
    }
    cout << endl;
    return ;
}

// init keyword and operator set
void Lexical::init_set(){
    keywords.push_back( make_pair("int", 4));
    keywords.push_back( make_pair("long", 5));
    keywords.push_back( make_pair("char", 6));
    keywords.push_back( make_pair("float", 7));
    keywords.push_back( make_pair("double", 8));
    keywords.push_back( make_pair("signed", 9));
    keywords.push_back( make_pair("unsigned", 10));
    keywords.push_back( make_pair("short", 11));
    keywords.push_back( make_pair("auto", 12));
    keywords.push_back( make_pair("constant", 13));
    keywords.push_back( make_pair("main", 14));
    keywords.push_back( make_pair("if", 15));
    keywords.push_back( make_pair("else", 16));
    keywords.push_back( make_pair("for", 17));
    keywords.push_back( make_pair("continue", 18));
    keywords.push_back( make_pair("break", 19));
    keywords.push_back( make_pair("while", 20));
    keywords.push_back( make_pair("do", 21));
    keywords.push_back( make_pair("switch", 22));
    keywords.push_back( make_pair("case", 23));
    keywords.push_back( make_pair("struct", 24));
    keywords.push_back( make_pair("typedef", 25));
    opers.push_back( make_pair("\"", 26));
    opers.push_back( make_pair("\'", 27));
    opers.push_back( make_pair(".", 28));
    opers.push_back( make_pair(":", 29));
    opers.push_back( make_pair(";", 30));
    opers.push_back( make_pair("==", 31));
    opers.push_back( make_pair(">=", 32));
    opers.push_back( make_pair("<=", 33));
    opers.push_back( make_pair("!=", 34));
    opers.push_back( make_pair("=", 35));
    opers.push_back( make_pair(">", 36));
    opers.push_back( make_pair("<", 37));
    opers.push_back( make_pair("+", 38));
    opers.push_back( make_pair("-", 39));
    opers.push_back( make_pair("*", 40));
    opers.push_back( make_pair("/", 41));
    opers.push_back( make_pair("+=", 42));
    opers.push_back( make_pair("-=", 43));
    opers.push_back( make_pair("*=", 44));
    opers.push_back( make_pair("/=", 45));
    opers.push_back( make_pair("&", 46));
    opers.push_back( make_pair("|", 47));
    opers.push_back( make_pair("<<", 48));
    opers.push_back( make_pair(">>", 49));
    opers.push_back( make_pair("{", 50));
    opers.push_back( make_pair("}", 51));
    opers.push_back( make_pair("[", 52));
    opers.push_back( make_pair("]", 53));
    opers.push_back( make_pair("(", 54));
    opers.push_back( make_pair(")", 55));
    opers.push_back( make_pair("&&", 56));
    opers.push_back( make_pair("||", 57));
    opers.push_back( make_pair("!", 58));
    opers.push_back( make_pair("%", 59));
    return ;
}

// check letter
bool Lexical::is_letter(char ch){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

// check digital
bool Lexical::is_digital(char ch){
    return (ch >= '0' && ch <= '9');
}

// analyse
void Lexical::analyse(const char* file_name){
    string str(27, '*');
    cout << str << " Input of lexical analyzer:" << str << endl;

    string tmp;
    ifstream infile;
    infile.open(file_name);
    while(getline(infile, tmp)){
        cout << tmp << endl;
    }
    cout << endl;




    print_results();
}

// change state
void Lexical::state_change(int& state, char ch){
    
}

// transfer pre state to code
void Lexical::state_to_code(int pre_state, string token){

}

// output results
void Lexical::print_results(){
    string str(27, '*');
    cout << str << "Output of lexical analyzer:" << str << endl;
    for(auto item:results){
        cout << "< " << item.first << " , " << item.second << " >" << endl; 
    }
}

int main() {
    Lexical myLexical;
    myLexical.analyse("demo.c");
    return 0;
}


