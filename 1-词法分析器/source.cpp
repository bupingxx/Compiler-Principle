#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

class Lexical{
private:
    // keyword set
    vector< pair<string, int> > keywords;

    // separator set
    vector< pair<string, int> > separators;

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
    int dot_flag;

    // keyword table
    vector<string> KT;

    // separator+operator table
    vector<string> PT;

    // results:<token, code>
    vector< pair<string, int> > results;
public:
    Lexical();
    ~Lexical();
    void print_table();
    void init_set();
    bool is_letter(char ch);
    bool is_digital(char ch);
    int check_keyword(string s);
    int check_separator(string s);
    int check_operator(string s);
    void analyse(const char* file_name);
    void state_change(int& state, char ch);
    void state_to_code(int pre_state, string token);
};

Lexical::Lexical(){
    dot_flag = 0;
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
    for(auto item:separators){
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
    keywords.emplace_back( make_pair("int", 4));
    keywords.emplace_back( make_pair("long", 5));
    keywords.emplace_back( make_pair("char", 6));
    keywords.emplace_back( make_pair("float", 7));
    keywords.emplace_back( make_pair("double", 8));
    keywords.emplace_back( make_pair("signed", 9));
    keywords.emplace_back( make_pair("unsigned", 10));
    keywords.emplace_back( make_pair("short", 11));
    keywords.emplace_back( make_pair("auto", 12));
    keywords.emplace_back( make_pair("constant", 13));
    keywords.emplace_back( make_pair("main", 14));
    keywords.emplace_back( make_pair("if", 15));
    keywords.emplace_back( make_pair("else", 16));
    keywords.emplace_back( make_pair("for", 17));
    keywords.emplace_back( make_pair("continue", 18));
    keywords.emplace_back( make_pair("break", 19));
    keywords.emplace_back( make_pair("while", 20));
    keywords.emplace_back( make_pair("do", 21));
    keywords.emplace_back( make_pair("switch", 22));
    keywords.emplace_back( make_pair("case", 23));
    keywords.emplace_back( make_pair("struct", 24));
    keywords.emplace_back( make_pair("typedef", 25));

    separators.emplace_back( make_pair("\"", 26));
    separators.emplace_back( make_pair("\'", 27));
    separators.emplace_back( make_pair(".", 28));
    separators.emplace_back( make_pair(":", 29));
    separators.emplace_back( make_pair(";", 30));
    separators.emplace_back( make_pair(",", 31));
    separators.emplace_back( make_pair("{", 32));
    separators.emplace_back( make_pair("}", 33));
    separators.emplace_back( make_pair("[", 34));
    separators.emplace_back( make_pair("]", 35));
    separators.emplace_back( make_pair("(", 36));
    separators.emplace_back( make_pair(")", 37));
    
    opers.emplace_back( make_pair("+", 38));
    opers.emplace_back( make_pair("-", 39));
    opers.emplace_back( make_pair("*", 40));
    opers.emplace_back( make_pair("/", 41));
    opers.emplace_back( make_pair("=", 42));
    opers.emplace_back( make_pair(">", 43));
    opers.emplace_back( make_pair("<", 44));
    opers.emplace_back( make_pair("!", 45));
    opers.emplace_back( make_pair("%", 46));
    opers.emplace_back( make_pair("&", 47));
    opers.emplace_back( make_pair("|", 48));
    opers.emplace_back( make_pair("!", 49));
    opers.emplace_back( make_pair("~", 50));
    opers.emplace_back( make_pair("^", 51));
    opers.emplace_back( make_pair("?", 52));
    opers.emplace_back( make_pair("<<", 53));
    opers.emplace_back( make_pair(">>", 54));
    opers.emplace_back( make_pair("&&", 55));
    opers.emplace_back( make_pair("||", 56));
    opers.emplace_back( make_pair("++", 57));
    opers.emplace_back( make_pair("--", 58));
    opers.emplace_back( make_pair("==", 59));
    opers.emplace_back( make_pair(">=", 60));
    opers.emplace_back( make_pair("<=", 61));
    opers.emplace_back( make_pair("!=", 62));
    opers.emplace_back( make_pair("+=", 63));
    opers.emplace_back( make_pair("-=", 64));
    opers.emplace_back( make_pair("*=", 65));
    opers.emplace_back( make_pair("/=", 66));
    opers.emplace_back( make_pair("%=", 67));
    opers.emplace_back( make_pair("&=", 68));
    opers.emplace_back( make_pair("|=", 69));
    opers.emplace_back( make_pair(">>=", 70));
    opers.emplace_back( make_pair("<<=", 71));
    return;
}

// check letter
bool Lexical::is_letter(char ch){
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

// check digital
bool Lexical::is_digital(char ch){
    return (ch >= '0' && ch <= '9');
}

// check keyword, return 0 when false
int Lexical::check_keyword(string s){
    for(auto item:keywords){
        if(item.first == s)
            return item.second;
    }
    return 0;
}

// check separator, return 0 when false
int Lexical::check_separator(string s){
    for(auto item:separators){
        if(item.first == s)
            return item.second;
    }
    return 0;
}

// check operator, return 0 when false
int Lexical::check_operator(string s){
    for(auto item:opers){
        if(item.first == s)
            return item.second;
    }
    return 0;
}


// *kernel function
void Lexical::analyse(const char* file_name){
    ifstream infile;
    infile.open(file_name);
    string tmp;

    if(infile.fail()){
        cout << "Cannot open file:" << file_name << endl;
        exit(0);
    }
    
    // print input
    string str(27, '*');
    cout << str << " Input of lexical analyzer:" << str << endl;
    while(getline(infile, tmp)){
        cout << tmp << endl;
    }
    cout << endl;
    
    // return to begin
    infile.clear();
    infile.seekg(0);

    // current state
    int state = -1;
    // pre state
    int pre_state = -1;
    string token;

    while(getline(infile, tmp)){
        for(char ch:tmp){
            pre_state = state;
            state_change(state, ch);
            if(state != -1)
                token += ch;
            else{
                state_to_code(pre_state, token);
                token.clear();
                if(ch != ' '){
                    state_change(state, ch);
                    token += ch;
                }
            }
        }
    }

    // print output
    cout << str << "Output of lexical analyzer:" << str << endl;
    for(auto item:results){
        cout << "< " << item.first << " , " << item.second << " >" << endl; 
    }
    cout << endl;

    // close file
    infile.close();
}

// change state
void Lexical::state_change(int& state, char ch){
    if(ch == ' '){
        state = -1;
        return;
    }

    switch(state){
        case -1:
            if(is_letter((ch)))
                state = 0;
            else if(is_digital(ch))
                state = 1;
            else if(check_separator(string(1, ch)))
                state = 2;
            else if(check_operator(string(1, ch)))
                state = 3;
            else state = -2;
            break;
        case 0:
            if(is_letter((ch)) || is_digital(ch))
                state = 0;
            else
                state = -1;
            break;
        case 1:
            if(is_digital(ch))
                state = 1;
            else if(ch == '.'){
                if(dot_flag == 0){
                    dot_flag = 1;
                    state = 1;
                } else {
                    cout << "This digit has too many decimal points." << endl;
                    exit(0);
                }
            }
            else
                state = -1;
            break;
        case 2:
            state = -1;
            break;
        case 3:
            if(check_operator(string(1, ch)))
                state = 3;
            else
                state = -1;
            break;
        default:
            cout << "There is a wrong state." << endl;
            exit(0);
    }

    if(state <= -2){
        cout << "Error in state_change, error code:" << state << endl << "current char:" << ch << endl;
        exit(0);
    }
    if(state != 1)
        dot_flag = 0;
        
    return;
}

// transfer token with pre_state to code
void Lexical::state_to_code(int pre_state, string token){
    if(pre_state == -1){
        return;
    } else if(pre_state == 0){
        int code = check_keyword(token);
        if(code){
            KT.emplace_back(token);
            results.emplace_back(make_pair(token, code));
        } else {
            iT.emplace_back(token);
            results.emplace_back(make_pair(token, 0));
        }
        // change 此地方需要修改 
        // 暂时默认所有字符都为标识符，实际上还要根据单引号、双引号判断是否为字符或字符串
        // cT - 1  ST - 2
    } else if(pre_state == 1){
        CT.emplace_back(token);
        results.emplace_back(make_pair(token, 3));
        // 数字即为常量，暂时不用修改
    } else if(pre_state == 2){
        int code = check_separator(token);
        if(code){
            PT.emplace_back(token);
            results.emplace_back(make_pair(token, code));
        } else {
            cout << "There is a wrong separator:" << token << endl;
            exit(0);
        }
        // 分隔符，暂时不用修改
    } else if(pre_state == 3){
        int code = check_operator(token);
        if(code){
            PT.emplace_back(token);
            results.emplace_back(make_pair(token, code));
        } else {
            cout << "There is a wrong operator:" << token << endl;
            exit(0);
        }
        // 运算符，暂时不用修改
    } else {
        cout << "Cannot recognize token:" << token << endl;
        exit(0);
    }
}

int main() {
    Lexical myLexical;
    myLexical.analyse("demo.c");
    return 0;
}


