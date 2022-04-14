#ifndef LEXICAL_HPP
#define LEXICAL_HPP

#include<iostream>
#include<iomanip>
#include<cstdlib>
#include<fstream>
#include<string>
#include<vector>
using namespace std;

#define lexical_print 0

class Lexical{
protected:
    // some flags
    int line_index;
    int note_flag;     // notes like /* */
    int char_flag;
    int string_flag;
    int dot_flag;

    // keyword set
    vector< pair<string, int> > keywords;

    // separator set
    vector< pair<string, int> > separators;

    // operator set
    vector< pair<string, int> > opers;

    // identify table
    vector<string> iT;

    // char table
    vector<string> cT;

    // string table
    vector<string> sT;

    // constant table
    vector<string> CT;

    // keyword table
    vector<string> KT;

    // separator+operator table
    vector<string> PT;

    // results:<word, token>
    vector< pair<string, int> > results;

public:
    Lexical();
    Lexical(const char* file_name);
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
    void state_to_code(int pre_state, string word);
    void print_error(int error_code, string error_info);
    vector< pair<string, int> > get_results();
};

Lexical::Lexical(){
    line_index = 1;
    char_flag = 0;
    string_flag = 0;
    dot_flag = 0;
    init_set();
}

Lexical::Lexical(const char* file_name){
    line_index = 1;
    char_flag = 0;
    string_flag = 0;
    dot_flag = 0;
    init_set();
    if(lexical_print) print_table();
    analyse(file_name);
}

Lexical::~Lexical(){
    keywords.clear();
    separators.clear();
    opers.clear();
    iT.clear();
    cT.clear();
    sT.clear();
    CT.clear();
    KT.clear();
    PT.clear();
    results.clear();
}

// print the table of symbol-token
void Lexical::print_table(){
    string str(27, '*');
    cout << str << " Table of symbol and token: " << str << endl;
    cout << setiosflags(ios::right);
    cout << setw(10) << "symbol" << setw(10) << "token" << setw(10) << "symbol" << setw(10) << "token";
    cout << setw(10) << "symbol" << setw(10) << "token" << setw(10) << "symbol" << setw(10) << "token";
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
    cout << endl << endl;
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
    keywords.emplace_back( make_pair("const", 13));
    keywords.emplace_back( make_pair("main", 14));
    keywords.emplace_back( make_pair("if", 15));
    keywords.emplace_back( make_pair("else", 16));
    keywords.emplace_back( make_pair("for", 17));
    keywords.emplace_back( make_pair("continue", 18));
    keywords.emplace_back( make_pair("break", 19));
    keywords.emplace_back( make_pair("return", 20));
    keywords.emplace_back( make_pair("void", 21));
    keywords.emplace_back( make_pair("while", 22));
    keywords.emplace_back( make_pair("do", 23));
    keywords.emplace_back( make_pair("struct", 24));
    keywords.emplace_back( make_pair("typedef", 25));

    separators.emplace_back( make_pair("\"", 26));
    separators.emplace_back( make_pair("\'", 27));
    separators.emplace_back( make_pair("\\", 28));
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
    opers.emplace_back( make_pair(".", 71));
    return;
}

// check letter
bool Lexical::is_letter(char ch){
    return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
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
        print_error(7, file_name);
    }
    
    // print input
    string str(27, '*');
    if(lexical_print){
        cout << str << " Input of lexical analyzer:" << str << endl;
        while(getline(infile, tmp)){
            cout << tmp << endl;
        }
        cout << endl << endl;
    
        // return to begin
        infile.clear();
        infile.seekg(0);
    }

    // current state
    int state = 0;
    // pre state
    int pre_state = 0;
    string word;

    while(getline(infile, tmp)){
        for(char ch:tmp){
            // notes like /*...*/
            if(note_flag){
                word += ch;
                if(word.size() > 1 && word.substr(word.size()-2, 2) == "*/"){
                    note_flag = 0;
                    word.clear();
                }
                continue;
            }

            pre_state = state;
            state_change(state, ch);
            if(state != 0){
                word += ch;
            } else {
                state_to_code(pre_state, word);
                word.clear();
                if(ch != ' '){
                    state_change(state, ch);
                    word += ch;
                }
            }
            // notes like // or /*
            if(word.size() > 1){
                if(word.substr(word.size()-2, 2) == "//"){
                    state = 0;
                    word.clear();
                    break;
                } else if (word.substr(word.size()-2, 2) == "/*"){
                    note_flag = 1;
                    word.clear();
                    state = 0;
                }
            }
        }
        line_index++;
    }
    state_to_code(pre_state, word);    // last char

    if(char_flag || string_flag)
        print_error(6, "");         // error
    if(note_flag)
        print_error(9, "");         // error
    
    if(lexical_print){
            // print output
        ofstream outfile;
        outfile.open("tokens.txt");
        cout << str << "Output of lexical analyzer:" << str << endl;
        for(auto item:results){
            cout << "< " << item.first << " , " << item.second << " >" << endl; 
        outfile << "< " << item.first << " , " << item.second << " >" << endl; 
        }
        cout << endl;
        outfile.close();
    }

    // close file
    infile.close();
}

// change state
void Lexical::state_change(int& state, char ch){
    switch(state){
        // start
        case 0:
            if(ch == '\''){
                char_flag = char_flag == 0 ? 1 : 0;
                state = 3;
            } else if (ch == '\"'){
                string_flag = string_flag == 0 ? 1 : 0;
                state = 3;
            } else if (char_flag || string_flag || is_letter(ch))
                state = 1;
            else if (ch == ' ')
                state = 0;
            else if (ch == '0')
                state = 6;
            else if (is_digital(ch))
                state = 2;
            else if (check_separator(string(1, ch)))
                state = 3;
            else if (check_operator(string(1, ch)))
                state = 4;
            else print_error(0, to_string(state) + " - " + string(1, ch));  // error
            break;
        // identify
        case 1:
            if(ch == '\'' || ch == '\"'){
                state = 0;
            } else if (char_flag || string_flag || is_letter((ch)) || is_digital(ch))
                state = 1;
            else
                state = 0;
            break;
        // digit or float(decimal) 
        case 2:
            if(is_digital(ch)){
                state = 2;
            } else if (ch == '.'){
                if(dot_flag == 0){
                    dot_flag = 1;
                    state = 2;
                } else {
                    print_error(1, "");         // error
                }
            } else if (ch == 'f'){
                state = 5;
            } else {
                state = 0;
            }
            break;
        // separator
        case 3:
            state = 0;
            break;
        // operator
        case 4:
            if(check_operator(string(1, ch)))
                state = 4;
            else
                state = 0;
            break;
        // float(decimal) 
        case 5:
            state = 0;
            break;
        // binary, ocatal or hexadecimal
        case 6:
            if(ch == 'b' || ch == 'B')
                state = 7;
            else if(ch >= '0' && ch <= '7')
                state = 8;
            else if(ch == 'x' || ch == 'X')
                state = 9;
            else if(ch == '8' || ch == '9')
                print_error(10, string(1, ch));
            else
                state = 0;
            break;
        // binary
        case 7:
            if(ch == '0' || ch == '1')
                state = 7;
            else if(is_digital(ch))
                print_error(11, string(1, ch));
            else
                state = 0;
            break;
        // ocatal
        case 8:
            if(ch >= '0' && ch <= '7')
                state = 8;
            else if(ch == '8' || ch == '9')
                print_error(10, string(1, ch));
            else
                state = 0;
            break;
        // hexadecimal
        case 9:
            if(is_digital(ch) || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
                state = 9;
            else if(is_letter(ch))
                print_error(12, string(1, ch));
            else
                state = 0;
            break;
        default:
            print_error(2, to_string(state));         // error
    }

    if(state != 2)
        dot_flag = 0;
        
    return;
}

// transfer word with pre_state to token
void Lexical::state_to_code(int pre_state, string word){
    if(pre_state == 0){
        return;
    } else if (pre_state == 1){
        // cT
        if(char_flag){
            int char_check = word[0]=='\\'?2:1;
            if(word.size() > char_check)
                print_error(8, word);    // error
            cT.emplace_back(word);
            results.emplace_back(make_pair(word, 1));
            return;
        }
        // sT
        if(string_flag){
            sT.emplace_back(word);
            results.emplace_back(make_pair(word, 2));
            return;
        }
        // KT or iT
        int token = check_keyword(word);
        if(token){
            KT.emplace_back(word);
            results.emplace_back(make_pair(word, token));
        } else {
            iT.emplace_back(word);
            results.emplace_back(make_pair(word, 0));
        }

    } else if (pre_state == 2 || (pre_state >= 5 && pre_state <= 9)){
        // CT state-2, 5, 6, 7, 8, 9
        CT.emplace_back(word);
        results.emplace_back(make_pair(word, 3));

    } else if (pre_state == 3){
        int token = check_separator(word);
        // PT state-3
        if(token){
            PT.emplace_back(word);
            results.emplace_back(make_pair(word, token));
        } else {
            print_error(3, word);      // error
        }
    } else if (pre_state == 4){
        int token = check_operator(word);
        // PT state-4
        if(token){
            PT.emplace_back(word);
            results.emplace_back(make_pair(word, token));
        } else {
            print_error(4, word);      // error
        }
    } else {
        print_error(5, word);          // error
    }
}

// print error_info when error
void Lexical::print_error(int error_code, string error_info){
    if(error_code != 6 && error_code != 9)
        cout << "In line " << line_index << ":" << endl;
    
    switch(error_code){
        case 0:
            cout << "Error in state_change, current state - char:" << error_info << endl;
            break;
        case 1:
            cout << "There is a digit which has too many decimal points." << endl;
            break;
        case 2:
            cout << "There is a wrong state:" << error_info << endl;
            break;
        case 3:
            cout << "There is a wrong separator:" << error_info << endl;
            break;
        case 4:
            cout << "There is a wrong operator:" << error_info << endl;
            break;
        case 5:
            cout << "Cannot recognize word:" << error_info << endl;
            break;
        case 6:
            cout << "Quotations do not match, you need check the token." << endl;
            break;
        case 7:
            cout << "Cannot open file:" << error_info << endl;
            break;
        case 8:
            cout << "Multi-character character constant:" << error_info << endl;
            break;
        case 9:
            cout << "Notes like /* and */ do not match, you need check the token." << error_info << endl;
            break;
        case 10:
            cout << "invalid digit \"" << error_info <<"\" in octal constant"<< endl;
            break;
        case 11:
            cout << "invalid digit \"" << error_info <<"\" in binary constant"<< endl;
            break;
        case 12:
            cout << "invalid digit \"" << error_info <<"\" in hexadecimal constant"<< endl;
            break;
        default:
            cout << "Unknown error token:" << error_code << endl;
            break;
    }
    exit(error_code);
}

// get results
vector< pair<string, int> > Lexical::get_results(){
    return results;
}

#endif