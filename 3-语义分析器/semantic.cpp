#include "syntactic.hpp"
using namespace std;

#define SEMANTIC_PRINT 1

struct quater{
    string op;
    string exp1;
    string exp2;
    string res;
    quater(string o = "", string e1 = "", string e2 = "", string r = ""):op(o), exp1(e1), exp2(e2), res(r){ };
    void clear(){
        op = "";
        exp1 = "";
        exp2 = "";
        res = "";
    }
    string get_string(){
        return op + " " + exp1 + " " + exp2 + " " + res;
    }
};

class LL1_semantic:public syntactic{
protected:
    ofstream outfile;
    vector<string> non_terminal;
    vector<string> terminal;
public:
    LL1_semantic(){ };
    LL1_semantic(vector< pair<string, int> > input);
    ~LL1_semantic(){ };
    string get_symbol(string str);
    void init_table();
    vector<string> search_table(string non_ter, string ter);
    void analyse(queue<string> expr, queue<string> input);
    void parse();
    void print_error(int error_code, string error_info);
};

LL1_semantic::LL1_semantic(vector< pair<string, int> > input){
    outfile.open("LL1_explain_output.txt");
    this->results = input;
    init_table();
    parse();
}

// get the symbol in () 
string LL1_semantic::get_symbol(string str){
    int idx = 0;
    int len = str.size();
    for(; idx < len; idx++){
        if(str[idx] == '{'){
            break;
        }
    }
    return str.substr(idx+1, len-idx-2);
}

// init non_terminal, terminal and analysis table
void LL1_semantic::init_table(){
    non_terminal = {"S", "V", "E", "E'", "T", "T'", "F"};
    terminal = {"(", "p", "=", "+", "-", "*", "/", "%", ")", "#"};
    table = {
        { {"V", "=", "E", "GEQ{=}"}, {"V", "=", "E", "GEQ{=}"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"p", "PUSH"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL" }},
        { {"T", "E'"}, {"T", "E'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} , {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"NULL"}, {"+", "T", "GEQ{+}", "E'"}, {"-", "T", "GEQ{-}", "E'"}, {"NULL"}, {"NULL"}, {"NULL"},{"e"}, {"e"} },
        { {"F", "T'"}, {"F", "T'"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} },
        { {"NULL"}, {"NULL"}, {"NULL"}, {"e"}, {"e"}, {"*", "F", "GEQ{*}", "T'"}, {"/", "F", "GEQ{/}", "T'"}, {"%", "F", "GEQ{%}", "T'"}, {"e"}, {"e"} },
        { {"(", "E", ")"}, {"p", "PUSH"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"}, {"NULL"} }
    };
}

// search analysis table
vector<string> LL1_semantic::search_table(string non_ter, string ter){
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
void LL1_semantic::analyse(queue<string> expr, queue<string> input){
    string expr_info = get_queue(expr);
    outfile << "For expression: " << expr_info << endl;
    if(SEMANTIC_PRINT)
        cout << "For expression: " << expr_info << endl;
    expr.push("#");
    // index of t
    int t_index = 1;
    // syntactic analysis stack
    stack<string> syn_stk;
    // semantic analysis stack
    stack<string> sem_stk;
    // explain results
    vector<quater> explain_results;

    string cur_symbol = "";
    string oper_info = "Start";
    quater cur_quater;
    syn_stk.push("#");
    syn_stk.push("S");
    outfile << setiosflags(ios::right);
    outfile << setw(45) << "SYN analysis stack" << setw(19) << "Current symbol"         
         << setw(19) << "Remaining seq" << setw(30) << "Operation" << setw(22)
         << "SEM analysis stack" << setw(14) << "Quaternion" << endl;
    outfile << setw(45) << get_stack(syn_stk, 0) << setw(19) << cur_symbol 
         << setw(19) << get_queue(input) << setw(30) << oper_info << setw(22)
         << get_stack(sem_stk, 0) << setw(14) << cur_quater.get_string() << endl;
    if(SEMANTIC_PRINT){
            cout << setiosflags(ios::right);
            cout << setw(45) << "SYN analysis stack" << setw(19) << "Current symbol"         
                << setw(19) << "Remaining seq" << setw(30) << "Operation" << setw(22)
                << "SEM analysis stack" << setw(14) << "Quaternion" << endl;
            cout << setw(45) << get_stack(syn_stk, 0) << setw(19) << cur_symbol 
                << setw(19) << get_queue(input) << setw(30) << oper_info << setw(22)
                << get_stack(sem_stk, 0) << setw(14) << cur_quater.get_string() << endl;
    }

    while(!syn_stk.empty()){
        cur_quater.clear();
        string stk_symbol = syn_stk.top();
        if(cur_symbol.empty()){
            if(input.empty())   // error
                print_error(3, cur_symbol);
            cur_symbol = input.front(); // next input
            input.pop();
        }

        // template val
        string syn_stk_info = get_stack(syn_stk, 0); 
        string cur_info = cur_symbol;
        syn_stk.pop();

        if(stk_symbol.find("PUSH") != stk_symbol.npos){ 
            // sem_stk push
            string push_symbol = get_symbol(stk_symbol);
            sem_stk.push(push_symbol);
            oper_info = "push to sem: " + push_symbol;

        } else if(stk_symbol.find("GEQ") != stk_symbol.npos){
            // generate quaternion
            string op = get_symbol(stk_symbol);

            if(op == "="){
                // treat = especially 
                string exp2 = "_";
                string exp1 = sem_stk.top();
                sem_stk.pop();
                string res = sem_stk.top();
                sem_stk.pop();
                cur_quater = quater(op, exp1, exp2, res);
            } else {
                string exp2 = sem_stk.top();
                sem_stk.pop();
                string exp1 = sem_stk.top();
                sem_stk.pop();
                string res = "t" + to_string(t_index);
                cur_quater = quater(op, exp1, exp2, res);
                sem_stk.push(res);
            }

            explain_results.emplace_back(cur_quater);
            t_index++;
            oper_info = "generate quaternion";
        } else if(stk_symbol == cur_symbol){
            // stack top match cur_symbol
            expr.pop();
            oper_info = "match: " + stk_symbol + "=" + cur_symbol;
            cur_symbol.clear();
        } else {
            vector<string> new_symbols = search_table(stk_symbol, cur_symbol);
            // NULL
            if(new_symbols[0] == "NULL")
                print_error(1, expr_info);

            string info = "";
            for(string str:new_symbols)
                info += str;
            oper_info = "select: " + stk_symbol + " -> " + info;

            for(int i = new_symbols.size()-1; i >= 0; i--){     // reverse push
                if(new_symbols[i] == "PUSH"){
                    // get expression
                    syn_stk.push(new_symbols[i] + "{" + expr.front() + "}");

                } else if(new_symbols[i] != "e"){
                    // non_empty symbol
                    syn_stk.push(new_symbols[i]);
                }   
            }
        }

        outfile << setw(45) << syn_stk_info << setw(19) << cur_info
            << setw(19) << get_queue(input) << setw(30) << oper_info << setw(22)
            << get_stack(sem_stk, 1) << setw(14) << cur_quater.get_string() << endl;
        if(SEMANTIC_PRINT){
            cout << setw(45) << syn_stk_info << setw(19) << cur_info
                << setw(19) << get_queue(input) << setw(30) << oper_info << setw(22)
                << get_stack(sem_stk, 1) << setw(14) << cur_quater.get_string() << endl;
        }
    }
    outfile << "Everything fine. The quaternions are:" << endl; 
    for(int i = 0; i < explain_results.size(); i++){
        if(explain_results[i].op == "="){
            outfile << "(" << i+1 << ") " << explain_results[i].res << " = "
                    << explain_results[i].exp1 << endl;
        } else {
            outfile << "(" << i+1 << ") " << explain_results[i].res << " = " << explain_results[i].exp1 << " "
                    << explain_results[i].op << " " << explain_results[i].exp2 << endl;
        }
    }
    outfile << endl;
    if(SEMANTIC_PRINT){
        cout << "Everything fine. The quaternions are:" << endl;
        for(int i = 0; i < explain_results.size(); i++){
            if(explain_results[i].op == "="){
                cout << "(" << i+1 << ") " << explain_results[i].res << " = "
                        << explain_results[i].exp1 << endl;
            } else {
                cout << "(" << i+1 << ") " << explain_results[i].res << " = " << explain_results[i].exp1 << " "
                        << explain_results[i].op << " " << explain_results[i].exp2 << endl;
            }
        }
        cout << endl;
    }
}

// get input
void LL1_semantic::parse(){
    // print output
    string str(58, '*');
    outfile << str << " Output of LL1 semantic analyzer:" << str << endl;
    if(SEMANTIC_PRINT){
        cout << str << " Output of LL1 semantic analyzer:" << str << endl;
    }

    // input string
    queue<string> expr;
    queue<string> input;

    for(int i = 0; i < results.size(); i++){
        string word = results[i].first;
        int token = results[i].second;

        if(!recognize_token(token)){
            while(!expr.empty())
                expr.pop();
            while(!input.empty())   // clear queue
                input.pop();   
        } else {
            string cur = token_convert(token);
            input.push(cur);

            if(cur == "#"){
                if(input.size() > 3)
                    analyse(expr, input);
                while(!expr.empty())
                    expr.pop();
                while(!input.empty())   // clear queue
                    input.pop();   
            } else {
                expr.push(word);
            }
        }
    }
    outfile << str << "   End of LL1 semantic analyzer: " << str << endl << endl;
    if(SEMANTIC_PRINT){
        cout << str << "   End of LL1 semantic analyzer: " << str << endl << endl;
    }    
}

// print error
void LL1_semantic::print_error(int error_code, string error_info){
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

int main(){
    Lexical lex("demo.c");
    LL1_semantic LL(lex.get_results());

    return 0;
}
