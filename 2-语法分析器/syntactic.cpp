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
    } else if(token == 38 || token == 39){
        return "w0";
    } else if(token == 40 || token == 41 || token == 46){
        return "w1";
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

class LL1_synt:private syntactic{
protected:
    ofstream outfile;
    vector<string> non_terminal;
    vector<string> terminal;
public:
    LL1_synt(){ };
    LL1_synt(vector< pair<string, int> > input);
    ~LL1_synt(){ };
    void init_table();
    vector<string> search_table(string non_ter, string ter);
    void analyse(string expr, queue<string> input);
    void parse();
};

LL1_synt::LL1_synt(vector< pair<string, int> > input){
    outfile.open("LL1_output.txt");
    this->results = input;
    init_table();
    parse();
}

// init non_terminal, terminal and analysis table
void LL1_synt::init_table(){
    non_terminal = {"S", "V", "E", "E'", "T", "T'", "F"};
    terminal = {"(", "p", "=", "w0", "w1", ")", "#"};
    table = {
        { {"V", "=", "E"}, {"V", "=", "E"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"p"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},
        { {"T", "E'"}, {"T", "E'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"NULL"}, {"w0", "T", "E'"}, {"NULL"}, {"e"}, {"e"} },
        { {"F", "T'"}, {"F", "T'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"NULL"}, {"e"}, {"w1", "F", "T'"}, {"e"}, {"e"} },
        { {"(", "E", ")"}, {"p"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} }
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
    if(x == -1 || y == -1 )  // error 
        print_error(2, non_ter + " -- " + ter);
    
    return table[x][y];
}

// kernel function
void LL1_synt::analyse(string expr, queue<string> input){
    outfile << "For expression: " << expr << endl;
    if(syntactic_print)
        cout << "For expression: " << expr << endl;
    // analysis stack
    stack<string> stk;
    string cur_symbol = "";
    string oper_info = "Start";
    stk.push("#");
    stk.push("S");
    outfile << setiosflags(ios::right);
    outfile << setw(19) << "Analysis stack" << setw(19) << "Current symbol"         
         << setw(19) << "Remaining seq" << setw(30) << "Operation" << endl;
    outfile << setw(19) << get_stack(stk, 0) << setw(19) << cur_symbol 
         << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
    if(syntactic_print){
            cout << setiosflags(ios::right);
            cout << setw(19) << "Analysis stack" << setw(19) << "Current symbol"         
                << setw(19) << "Remaining seq" << setw(30) << "Operation" << endl;
            cout << setw(19) << get_stack(stk, 0) << setw(19) << cur_symbol 
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
        string stk_info = get_stack(stk, 0); 
        string cur_info = cur_symbol;
        stk.pop();

        if(stk_symbol == cur_symbol){
            // stack top match cur_symbol
            oper_info = "match: " + stk_symbol + "=" + cur_symbol;
            cur_symbol.clear();
        } else {
            vector<string> new_symbols = search_table(stk_symbol, cur_symbol);
            // NULL
            if(new_symbols[0] == "NULL")
                print_error(1, expr);

            string info = "";
            for(string str:new_symbols)
                info += str;
            oper_info = "select: " + stk_symbol + " -> " + info;

            for(int i = new_symbols.size()-1; i >= 0; i--){     // reverse push
                if(new_symbols[i] != "e")   // non_empty symbol
                    stk.push(new_symbols[i]);
            }
        }

        outfile << setw(19) << stk_info << setw(19) << cur_info 
             << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
        if(syntactic_print){
            cout << setw(19) << stk_info << setw(19) << cur_info 
                    << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
        }
    }
    outfile << "Everything fine." << endl << endl; 
    if(syntactic_print){
        cout << "Everything fine." << endl << endl; 
    }
}

// get input
void LL1_synt::parse(){
    // print output
    string str(27, '*');
    outfile << str << " Output of LL1 synatactic analyzer:" << str << endl;
    if(syntactic_print){
        cout << str << " Output of LL1 synatactic analyzer:" << str << endl;
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
    outfile << str << "   End of LL1 synatactic analyzer: " << str << endl << endl;
    if(syntactic_print){
        cout << str << "   End of LL1 synatactic analyzer: " << str << endl << endl;
    }    
}

class LR1_synt:private syntactic{
protected:
    ofstream outfile;
    vector<string> left_products;
    vector<int> right_products; 
    vector<string> symbols;
public:
    LR1_synt(){ };
    LR1_synt(vector< pair<string, int> > input);
    ~LR1_synt(){ };
    void init_table();

    vector<string> search_table(int state, string cur_symbol);
    void switch_state(stack<string> &stk, int &state, int op, string search_res);
    void analyse(string expr, queue<string> input);
    void parse();
};

LR1_synt::LR1_synt(vector< pair<string, int> > input){
    outfile.open("LR1_output.txt");
    this->results = input;
    init_table();
    parse();
}

// init state, symbol and analysis table
void LR1_synt::init_table(){
    left_products = {
        "G0",
        "S1",
        "S1",
        "E4/E5",
        "E4/E5",
        "T7/T8",
        "T7/T8",
        "F10/F11",
        "F10/F11",
        "V2"
    };

    right_products = { 0, 3, 1, 3, 1, 3, 1, 1, 3, 1 };  // pop times

    symbols = {"(", "p", "=", "w0", "w1", ")", "#", "S", "V", "E", "T", "F"};
    table = {
        { {"(13"}, {"p12", "p16"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"S1"}, {"V2"}, {"E5"}, {"NULL"}, {"NULL"} },   // line 0
        { {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"ok"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"=3"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"(13"}, {"p12"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"E4"}, {"NULL"}, {"NULL"} },
        { {"r(1)"}, {"r(1)"}, {"r(1)"}, {"w06"}, {"r(1)"}, {")15"}, {"r(1)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"T8"}, {"NULL"} },    // line 4
        { {"r(2)"}, {"r(2)"}, {"r(2)"}, {"w06"}, {"r(2)"}, {")15"}, {"r(2)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"T8"}, {"NULL"} },
        { {"(13"}, {"p12"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"T7"}, {"NULL"} },
        { {"r(3)"}, {"r(3)"}, {"r(3)"}, {"r(3)"}, {"w19"}, {"r(3)"}, {"r(3)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"F11"}},
        { {"r(4)"}, {"r(4)"}, {"r(4)"}, {"r(4)"}, {"w19"}, {"r(4)"}, {"r(4)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"F11"}},    // line 8
        { {"(13"}, {"p12"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"F10"} },
        { {"(13"}, {"p12"}, {"r(5)"}, {"r(5)"}, {"r(5)"}, {"r(5)"}, {"r(5)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},
        { {"(13"}, {"p12"}, {"r(6)"}, {"r(6)"}, {"r(6)"}, {"r(6)"}, {"r(6)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},    // line 11
        { {"r(7)"}, {"r(7)"}, {"r(7)"}, {"r(7)"}, {"r(7)"}, {"r(7)"}, {"r(7)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},
        { {"(13"}, {"p12"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"E14"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {")15"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"r(8)"}, {"r(8)"}, {"r(8)"}, {"r(8)"}, {"r(8)"}, {"r(8)"}, {"r(8)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},  // line 15
        { {"r(9)"}, {"r(9)"}, {"r(9)"}, {"r(9)"}, {"r(9)"}, {"r(9)"}, {"r(9)"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}},
    };
}

// search analysis table
vector<string> LR1_synt::search_table(int state, string cur_symbol){
    int idx = -1;
    for(int i = 0; i < symbols.size(); i++){
        if(cur_symbol == symbols[i]){
            idx = i;
            break;
        }
    }
    if(idx == -1)
        print_error(2, cur_symbol);
    return table[state][idx];
}

// switch state op=0->reduce  op=1->move
void LR1_synt::switch_state(stack<string> &stk, int &state, int op, string search_res){
    if(op == 0){
        // reduce
        int idx_products = search_res[2] - '0';
        string reduce_str = left_products[idx_products];
        int pop_times = right_products[idx_products];
        while(pop_times--){
            stk.pop();
        }

        if(reduce_str.size() > 4){
            if(reduce_str == "E4/E5"){
                if(stk.top() == "=3"){
                    stk.push("E4");
                    state = 4;
                } else {
                    stk.push("E5");
                    state = 5;
                }
            } else if(reduce_str == "T7/T8"){
                if(stk.top() == "w06"){
                    stk.push("T7");
                    state = 7;
                } else {
                    stk.push("T8");
                    state = 8;
                }
            } else if(reduce_str == "F10/F11"){
                if(stk.top() == "w19"){
                    stk.push("F10");
                    state = 10;
                } else {
                    stk.push("F11");
                    state = 11;
                }
            }
        } else {
            stk.push(reduce_str);
            state = reduce_str[1] - '0';
        }
    } else if(op == 1){
        // move
        stk.push(search_res);
        if(search_res[0] == 'w'){       // w06 w19
            state = search_res[2] - '0';
        } else if(search_res.size()>2){
            state = stoi(search_res.substr(1, 2)); // p12 (13 E14 )15 ..
        } else {
            state = search_res[1] - '0';    // S1 V2 =3 .. 
        }
    } else print_error(3, "Wrong op.");
}

// kernel function
void LR1_synt::analyse(string expr, queue<string> input){
    outfile << "For expression: " << expr << endl;
    if(syntactic_print){
        cout << "For expression: " << expr << endl;
    }
    // analysis stack
    int state = 0;
    bool ok = false;
    stack<string> stk;
    string cur_symbol = "";
    string oper_info = "Start";
    stk.push("#0");
    outfile << setiosflags(ios::right);
    outfile << setw(35) << "Analysis stack" << setw(16) << "Current symbol"
         << setw(19) << "Remaining seq" << setw(19) << "Operation" << endl;
    outfile << setw(35) << get_stack(stk, 1) << setw(16) << cur_symbol
         << setw(19) << get_queue(input) << setw(19) << oper_info << endl;
    if(syntactic_print){
            cout << setiosflags(ios::right);
            cout << setw(19) << "Analysis stack" << setw(19) << "Current symbol"
                << setw(19) << "Remaining seq" << setw(30) << "Operation" << endl;
            cout << setw(19) << get_stack(stk, 1) << setw(19) << cur_symbol
                << setw(19) << get_queue(input) << setw(30) << oper_info << endl;
    }

    while(!ok){
        // template val
        string stk_info = get_stack(stk, 1);
        string cur_info = cur_symbol;
        string que_info = get_queue(input);

        if(cur_symbol.empty()){
            if(input.empty()){  // error
                print_error(3, cur_symbol);
            }   
            // next(word)    
            cur_symbol = input.front();
            input.pop();
            cur_info = cur_symbol;
            que_info = get_queue(input);
            oper_info = "next(" + cur_symbol + ")";
        } else {
            // search table first
            // decide to reduce or move
            vector<string> search_res = search_table(state, cur_symbol);
            if(state == 1 && search_res[0] == "ok"){
                // ok and quit
                ok = true;
                oper_info = "ok";
            } else if(search_res[0][0] == 'r'){
                // reduce
                switch_state(stk, state, 0, search_res[0]);
                oper_info = "reduce: " + search_res[0];
            } else if(search_res[0] != "NULL"){
                // move
                if(input.empty()){  // error
                    print_error(3, cur_symbol);
                }
                cur_symbol = input.front();
                input.pop();
                // To avoid reduce conflict LR(1) 
                // need to check next word
                if(search_res.size() > 1 && cur_symbol == "="){     
                    switch_state(stk, state, 1, search_res[1]);
                    oper_info = "push: " + search_res[1];
                } else {
                    switch_state(stk, state, 1, search_res[0]);
                    oper_info = "push: " + search_res[0];
                }
            } else {    // NULL
                print_error(1, expr);
            }
        }

        
        outfile << setw(35) << stk_info << setw(16) << cur_info
             << setw(19) << que_info << setw(19) << oper_info << endl;
        if(syntactic_print){
            cout << setw(35) << stk_info << setw(16) << cur_info
                    << setw(19) << que_info << setw(19) << oper_info << endl;
        }
    }
    outfile << "Everything fine." << endl << endl; 
    if(syntactic_print){
        cout << "Everything fine." << endl << endl; 
    }
}

// get input
void LR1_synt::parse(){
    // print output
    string str(27, '*');
    outfile << str << " Output of LR1 synatactic analyzer:" << str << endl;
    if(syntactic_print){
        cout << str << " Output of LR1 synatactic analyzer:" << str << endl;
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
    outfile << str << "   End of LR1 synatactic analyzer: " << str << endl << endl;
    if(syntactic_print){
        cout << str << "   End of LR1 synatactic analyzer: " << str << endl << endl;
    }    
}

int main(){
    Lexical lex("demo.c");
    LL1_synt LL(lex.get_results());
    LR1_synt LR(lex.get_results());
    return 0;
}
