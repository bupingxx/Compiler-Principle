#include "lexical.hpp"
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_map>
using namespace std;

#define syntactic_print 1

class syntactic{
protected:
    // output of lexical analyzer
    vector< pair<string, int> > results;

    // analysis table
    vector<string> non_terminal;
    vector<string> terminal;
    vector<vector<vector<string>>> table;
    
public:
    syntactic(){ };
    ~syntactic(){ };
    bool recognize_token(int token);
    void print_error(int error_code, string error_info);
    string get_queue(queue<string> que);
    string get_stack(stack<string> stk);
    virtual void init_table() = 0;
    virtual vector<string> search_table(string non_ter, string ter) = 0;
    virtual string token_convert(int token) = 0;
    virtual void analyse(string expr, queue<string> input) = 0;
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
            cout << "error: expected primary-expression before: " << error_info << endl;
            break;
        case 2:
            cout << "error: search table failed:" << error_info << endl;
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
string syntactic::get_stack(stack<string> stk){
    vector<string> tmp;
    while(!stk.empty()){
        tmp.emplace_back(stk.top());
        stk.pop();
    }

    string res = "";
    for(int i = tmp.size()-1; i >= 0; i--)
        res += tmp[i];
    return res;
}

class LL1_synt:private syntactic{
protected:
    ofstream outfile;
public:
    LL1_synt(){ };
    LL1_synt(vector< pair<string, int> > input);
    ~LL1_synt();
    void init_table();
    vector<string> search_table(string non_ter, string ter);
    string token_convert(int token);
    void analyse(string expr, queue<string> input);
    void parse();
};

LL1_synt::LL1_synt(vector< pair<string, int> > input){
    if(syntactic_print){
        outfile.open("LL1_output.txt");
    }
    this->results = input;
    init_table();
    parse();
}

LL1_synt::~LL1_synt(){
    this->results.clear();
    this->table.clear();
}

// init non_terminal, terminal and analysis table
void LL1_synt::init_table(){
    non_terminal = {"S", "S'", "E", "E'", "T", "T'", "F"};
    terminal = {"(", "p", "=", "w0", "w1", ")", "#"};
    table = {
        { {"E", "S'"}, {"E", "S'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"=", "E", "S'"}, {"NULL"}, {"NULL"}, {"e"}, {"e"}},
        { {"T", "E'"}, {"T", "E'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"e"}, {"w0", "T", "E'"}, {"NULL"}, {"e"}, {"e"} },
        { {"F", "T'"}, {"F", "T'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"e"}, {"e"}, {"w1", "F", "T'"}, {"e"}, {"e"} },
        { {"(", "S", ")"}, {"p"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} }
    };
}

// search analysis table
vector<string> LL1_synt::search_table(string non_ter, string ter){
    int x = -1, y = -1;
    for(int i = 0; i < non_terminal.size(); i++){
        if(non_terminal[i] == non_ter){
            x = i;
            break;
        }
    }
    for(int i = 0; i < terminal.size(); i++){
        if(terminal[i] == ter){
            y = i;
            break;
        }
    }
    if(x == -1 || y == -1 || table[x][y][0] == "NULL")  // error 
        print_error(2, non_ter + " -- " + ter);
    
    return table[x][y];
}

// convert token to symbol
string LL1_synt::token_convert(int token){
    if(token >= 0 && token <= 3){
        return "p";
    } else if(token == 36){
        return "(";
    } else if(token == 37){
        return ")";
    } else if(token == 38 || token == 39){
        return "w0";
    } else if(token == 40 || token == 41 || token == 46){
        return "w1";
    } else if(token == 42){
        return "=";
    } else return "#";
}

// kernel function
void LL1_synt::analyse(string expr, queue<string> input){
    cout << "For expression: " << expr << endl;
    // analysis stack
    stack<string> stk;
    string cur_symbol = "";
    string oper_info = "Start";
    stk.push("#");
    stk.push("S");
    cout << setiosflags(ios::right);
    cout << setw(19) << "Analysis stack" << setw(19) << "Current symbol"         
         << setw(19) << "Remaining seq" << setw(30) << "Operation" << endl;
    cout << setw(19) << get_stack(stk) << setw(19) << cur_symbol 
         << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
    if(syntactic_print){
            outfile << setiosflags(ios::right);
            outfile << setw(19) << "Analysis stack" << setw(19) << "Current symbol"         
                << setw(19) << "Remaining seq" << setw(30) << "Operation" << endl;
            outfile << setw(19) << get_stack(stk) << setw(19) << cur_symbol 
                << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
    }

    while(!stk.empty()){
        string stk_symbol = stk.top();
        if(cur_symbol.empty()){
            if(input.empty())   // error
                print_error(3, cur_symbol);
            cur_symbol = input.front();
            input.pop();
        }

        // template val
        string stk_info = get_stack(stk); 
        string cur_info = cur_symbol;
        stk.pop();

        if(stk_symbol == cur_symbol){
            // stack top match cur_symbol
            oper_info = "match: " + stk_symbol + "=" + cur_symbol;
            cur_symbol.clear();
        } else {
            vector<string> new_symbols = search_table(stk_symbol, cur_symbol);

            string info = "";
            for(string str:new_symbols)
                info += str;
            oper_info = "select: " + stk_symbol + " -> " + info;

            for(int i = new_symbols.size()-1; i >= 0; i--){     // reverse push
                if(new_symbols[i] != "e")   // non_empty symbol
                    stk.push(new_symbols[i]);
            }
        }

        cout << setw(19) << stk_info << setw(19) << cur_info 
             << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
        if(syntactic_print){
            outfile << setw(19) << stk_info << setw(19) << cur_info 
                    << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
        }
    }
    cout << "Everything fine." << endl << endl; 
    if(syntactic_print){
        outfile << "Everything fine." << endl << endl; 
    }
}

// get input
void LL1_synt::parse(){
    // print output
    string str(27, '*');
    cout << str << " Output of LL1 synatactic analyzer:" << str << endl;
    if(syntactic_print){
        outfile << str << " Output of LL1 synatactic analyzer:" << str << endl;
    }

    // input string
    string expr = "";
    queue<string> input;

    for(int i = 0; i < results.size(); i++){
        string word = results[i].first;
        int token = results[i].second;

        if(!recognize_token(token)){
            expr = "";
            while(!input.empty())   // clear queue
                input.pop();   
        } else {
            string cur = token_convert(token);
            input.push(cur);

            if(cur == "#"){
                if(input.size() > 3)
                    analyse(expr, input);
                expr = "";
                while(!input.empty())   // clear queue
                    input.pop();   
            } else {
                expr += word;
            }
        }
    }    
}



class LR0_synt:private syntactic{

};



int main(){
    Lexical lex("demo.c");
    LL1_synt LL(lex.get_results());
    
    return 0;
}
