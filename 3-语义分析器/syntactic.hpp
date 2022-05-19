#ifndef SYNTACTIC_HPP
#define SYNTACTIC_HPP

#include "lexical.hpp"
#include <algorithm>
#include <queue>
#include <stack>
using namespace std;

#define SYNTACTIC_PRINT 0

class syntactic{
protected:
    // output of lexical analyzer
    vector< pair<string, int> > results;

    // analysis table
    vector<vector<vector<string>>> table;   
public:
    syntactic(){ };
    ~syntactic(){ };
    bool recognize_token(int token);
    void print_error(int error_code, string error_info);
    string get_queue(queue<string> que);
    string get_stack(stack<string> stk, int type);
    string token_convert(int token);
    pair<string, string> string_convert(string str);
    virtual void init_table() = 0;
    virtual void analyse(queue<string> expr, queue<string> input) = 0;
};

// check if token is an operator
bool syntactic::recognize_token(int token){
    // iT cT sT CT ; , ] ( ) + - * / % 
    int check[] = {0, 1, 2, 3, 30, 31, 35, 36, 37, 38, 39, 40, 41, 42, 46};

    for(int i = 0; i < 15; i++){
        if(token == check[i]){
            return true;
        }
    }
    return false;
}

// print error
void syntactic::print_error(int error_code, string error_info){
    switch(error_code){
        case 1:
            cout << "error: expected primary-expression in: " << error_info << endl;
            break;
        case 2:
            cout << "error: search table failed:" << error_info << endl;
            break;
        case 3:
            cout << "error: input is invalid." << endl;
            break;
        default:
            cout << "Unknown error token:" << error_code << endl;
            break;
    }
    exit(error_code);
}

// get queue string
string syntactic::get_queue(queue<string> que){
    string res = "";
    while(!que.empty()){
        res += que.front();
        que.pop();
    }
    return res;
}

// get stack string
string syntactic::get_stack(stack<string> stk, int type){
    vector<string> tmp;
    while(!stk.empty()){
        tmp.emplace_back(stk.top());
        stk.pop();
    }

    string res = "";
    for(int i = tmp.size()-1; i >= 0; i--){
        res += tmp[i];
        if(type == 1)
            res += " ";
    }
        
    return res;
}

// convert token to symbol
string syntactic::token_convert(int token){
    if(token >= 0 && token <= 3){
        return "p";
    } else if(token == 36){
        return "(";
    } else if(token == 37){
        return ")";
    } else if(token == 38){
        return "+";
    } else if(token == 39){
        return "-";
    } else if(token == 40){
        return "*";
    } else if(token == 41){
        return "/";
    } else if(token == 46){
        return "%";
    } else if(token == 42){
        return "=";
    } else return "#";
}

// convert string to string-string
pair<string, string> syntactic::string_convert(string str){
    int idx = -1;
    for(int i = 0; i < str.size(); i++){
        if(str[i] == '/'){
            idx = i;
            break;
        }
    }
    string a = str.substr(0, idx);
    string b = str.substr(idx + 1, str.size()-idx-1);
    return make_pair(a, b);
}

#endif