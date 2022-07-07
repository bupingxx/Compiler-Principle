//**************************************************************************
/* File:     semantic.hpp
 * 
 * Purpose:  语义分析器, 生成中间代码(四元式)
 * 
 * Input:    词法分析器的输出-Token序列
 * Output:   中间代码(四元式)
 * 
 * Note:     由于在翻译文法中增加了语义动作, Semantical类继承于Syntactic,
 *           通过同时维护语法栈和语义栈的方式来生成中间代码
 */
 //**************************************************************************

#ifndef _SEMANTIC_HPP
#define _SEMANTIC_HPP
#include "syntax.hpp"
#include "../utils/quater.hpp"

// 语义分析器类, 继承于语法分析器
class Semantical:public Syntactic{
protected:
    ofstream outfile;
    // 非终结符字典
    vector<string> non_terminal;

    // 终结符字典
    vector<string> terminal;

    // 输出的四元式序列
    vector<quater> quater_results;
public:
    Semantical(){ };
    ~Semantical(){ };
    void semantical_analyse(vector< pair<string, int> > input);
    string get_symbol(string str);
    void init_table();
    vector<string> search_table(string non_ter, string ter);
    void syntactic_analyse(queue<string> expr, queue<string> input);
    void parse();
    void print_error(int error_code, string error_info);
    vector<quater> get_result();
};

// 进行语法分析和语义分析
void Semantical::semantical_analyse(vector< pair<string, int> > input){
    outfile.open("./output/Syntactic_output.log");
    this->tokens = input;
    init_table();
    parse();
    outfile.close();
}

// 从语义动作中获得符号
// 例如: 输入GEQ{+}, 输出+
string Semantical::get_symbol(string str){
    int idx = 0;
    int len = str.size();
    for(; idx < len; idx++){
        if(str[idx] == '{'){
            break;
        }
    }
    return str.substr(idx+1, len-idx-2);
}

// 初始化非终结符, 终结符以及分析表
void Semantical::init_table(){
    non_terminal = {"P", "SP", "VD", "I", "I'", "TYPE", "CS", "SS", "AS", "E", "E'",
                    "T", "T'", "F", "LE", "LE'", "L", "L'", "IS", "ELIFS", "ELSES", "WS"};    // 22个

    terminal = {"program", "var", ":", ";", "id", ",", "integer", "real", "char", "bool", "begin", "end", ":=", 
                "+", "-", "*", "/", "cons", "(", ")", "and", "or", ">", "<", "=", "true", "false", "if", "elif",
                "else", "while", "#"};              // 32个

   table = {
        { {"program", "id", "SP"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {"VD", "CS"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {"var", "I", ":", "TYPE", ";", "VD"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {""}, {""}, {""}, {"id", "I'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {"e"}, {""}, {""}, {",", "id", "I'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {"integer"}, {"real"}, {"char"}, {"bool"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"begin", "SS", "end"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {"AS", ";", "SS"}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"IS", "SS"}, {""}, {""}, {"WS", "SS"}, {"e"} },
        { {""}, {""}, {""}, {""}, {"id", "PUSH{id}", ":=", "E", "GEQ{:=}"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {"T", "E'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"T", "E'"}, {"T", "E'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"+", "T", "GEQ{+}", "E'"}, {"-", "T", "GEQ{-}", "E'"}, {""}, {""}, {""}, {""}, {"e"}, {"e"}, {"e"}, {"e"}, {"e"}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {""}, {""}, {""}, {"F", "T'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"F", "T'"}, {"F", "T'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {"e"}, {"*", "F", "GEQ{*}", "T'"}, {"/", "F", "GEQ{/}", "T'"}, {""}, {""}, {"e"}, {"e"}, {"e"}, {"e"}, {"e"}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {""}, {""}, {""}, {"id", "PUSH{id}"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"cons", "PUSH{cons}"}, {"(", "E", ")"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {"L", "LE'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"L", "LE'"}, {"L", "LE'"}, {""}, {""}, {""}, {""}, {""}, {""}, {"L", "LE'"}, {"L", "LE'"}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {"and", "L", "GEQ{and}", "LE'"}, {"or", "L", "GEQ{or}", "LE'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"} },
        { {""}, {""}, {""}, {""}, {"E", "L'"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"E", "L'"}, {"E", "L'"}, {""}, {""}, {""}, {""}, {""}, {""}, {"true", "PUSH{true}"}, {"false", "PUSH{false}"}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {"e"}, {"e"}, {">", "L", "GEQ{>}"}, {"<", "L", "GEQ{<}"}, {"=", "L", "GEQ{=}"}, {""}, {""}, {""}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"if", "(", "LE", ")", "IF{if}", "CS", "IFE{ife}","ELIFS", "ELSES"}, {""}, {""}, {""}, {""} },
        { {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {"elif", "(", "LE", ")", "EIF{elif}", "CS", "ELIFE{elife}", "ELIFS"}, {"e"}, {"e"}, {"e"} },
        { {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {"else", "ELSE{el}", "CS", "IE{ie}"}, {"e"}, {"e"} },
        { {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"e"}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {""}, {"while", "WH{wh}", "(", "LE", ")", "DO{do}", "CS", "WE{we}"}, {""} }
    };
}

// 查找分析表
vector<string> Semantical::search_table(string non_ter, string ter){
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
    if(x == -1 || y == -1 )  // 查询的是错误的符号, 报错
        print_error(2, non_ter + " -- " + ter);

    return table[x][y];
}

// 核心函数: 语法分析+语义分析
void Semantical::syntactic_analyse(queue<string> expr, queue<string> input){
    // 在输入的末尾加上#
    string expr_info = get_queue(expr);
    expr.push("#");
    input.push("#");

    // 四元式中t的序号
    int t_index = 1;

    // 语法分析栈
    stack<string> syn_stk;

    // 语义分析栈
    stack<string> sem_stk;
    
    // 当前符号
    string cur_symbol = "";

    // 当前四元式
    quater cur_quater;

    // 打印初始的分析栈信息
    string oper_info = "Start";
    syn_stk.push("#");
    syn_stk.push("P");
    outfile << setiosflags(ios::right);
    outfile << setw(70) << "SYN analysis stack" << setw(20) << "Current symbol"         
         << setw(150) << "Remaining seq" << setw(55) << "Operation" << setw(31)
         << "SEM analysis stack" << setw(20) << "Quaternion" << endl;
    outfile << setw(70) << get_stack(syn_stk, 0) << setw(20) << cur_symbol 
         << setw(150) << get_queue(input) << setw(55) << oper_info << setw(31)
         << get_stack(sem_stk, 0) << setw(20) << cur_quater.get_string() << endl;
    
    // 进行分析,直至语法分析栈为空
    while(!syn_stk.empty()){
        cur_quater.clear();
        string stk_symbol = syn_stk.top();
        if(cur_symbol.empty()){
            if(input.empty())               // 输入串为空, 报错
                print_error(3, cur_symbol);
            cur_symbol = input.front();     // 获得输入串中的下一个符号
            input.pop();
        }

        // 临时变量, 用于打印信息
        string syn_stk_info = get_stack(syn_stk, 0); 
        string cur_info = cur_symbol;

        // 获得语法分析栈的栈顶元素
        syn_stk.pop();

        if(stk_symbol.find("PUSH") != stk_symbol.npos){ 
            // 语义栈 PUSH
            string push_symbol = get_symbol(stk_symbol);
            sem_stk.push(push_symbol);
            oper_info = "push to sem: " + push_symbol;

        } else if(stk_symbol.find("GEQ") != stk_symbol.npos){
            // 生成四元式
            string op = get_symbol(stk_symbol);

            if(op == ":="){
                // 对赋值符号:= 特殊处理
                string exp1 = sem_stk.top();
                sem_stk.pop();
                string res = sem_stk.top();
                sem_stk.pop();
                cur_quater = quater(op, exp1, "_", res);
            } else {
                string exp2 = sem_stk.top();
                sem_stk.pop();
                string exp1 = sem_stk.top();
                sem_stk.pop();
                string res = "t" + to_string(t_index);
                cur_quater = quater(op, exp1, exp2, res);
                sem_stk.push(res);
                t_index++;
            }
            // 记录生成的四元式
            quater_results.emplace_back(cur_quater);
            oper_info = "generate quaternion";
        } else if(stk_symbol.size() > 4 && stk_symbol.substr(0, 3) != "IFE" && (stk_symbol.substr(0, 2) == "IF" 
            || stk_symbol.substr(0, 2) == "DO" || stk_symbol.substr(0, 3) == "EIF")){
            // 获得语义动作中的符号并生成四元式
            string op = get_symbol(stk_symbol);
            string exp1 = sem_stk.top();
            sem_stk.pop();
            cur_quater = quater(op, exp1, "_", "_");

            // 记录生成的四元式
            quater_results.emplace_back(cur_quater);
            oper_info = "if/elif/do quaternion";
        } else if(stk_symbol.size() > 5 && stk_symbol.substr(0, 5) != "ELSES" && (stk_symbol.substr(0, 2) == "IE" 
            || stk_symbol.substr(0, 2) == "WH" || stk_symbol.substr(0, 2) == "WE" || stk_symbol.substr(0, 5) == "ELIFE" 
            || stk_symbol.substr(0, 4) == "ELSE" || stk_symbol.substr(0, 3) == "IFE")){
            // 获得语义动作中的符号并生成四元式
            string op = get_symbol(stk_symbol);
            cur_quater = quater(op, "_", "_", "_");

            // 记录生成的四元式
            quater_results.emplace_back(cur_quater);
            oper_info = "ife/elife/el/ie/wh/we quaternion";
        } else if(stk_symbol == cur_symbol){
            // 语法栈的栈顶与当前符号匹配
            expr.pop();
            oper_info = "match: " + stk_symbol + "=" + cur_symbol;
            cur_symbol.clear();
        } else {
            // 不符合上述情况, 查找分析表
            vector<string> new_symbols = search_table(stk_symbol, cur_symbol);
            
            // 查表结果为空, 报错
            if(new_symbols[0].size() == 0)
                print_error(1, expr_info);

            string info = "";
            for(string str:new_symbols)
                info += str;
            oper_info = "select: " + stk_symbol + " -> " + info;

            for(int i = new_symbols.size()-1; i >= 0; i--){      // 将选择的产生式反向入栈
                if(new_symbols[i].find("PUSH") != new_symbols[i].npos){
                    // 得到输入串中的标识符或常数
                    syn_stk.push("PUSH{" + expr.front() + "}");

                } else if(new_symbols[i] != "e"){
                    // 非空串
                    syn_stk.push(new_symbols[i]);
                }   // 空串, 不进行处理
            }
        }
        // 打印分析栈
        outfile << setw(70) << syn_stk_info << setw(20) << cur_info
            << setw(150) << get_queue(input) << setw(55) << oper_info << setw(31)
            << get_stack(sem_stk, 1) << setw(20) << cur_quater.get_string() << endl;
    }

    // 语法分析器控制台输出
    string str(25, '*');
    cout << str << " Output of Syntactic Analyzer: " << str << endl;
    cout << "Everything fine." << endl << endl;

    // 语义分析器文件输出
    ofstream of;
    of.open("./output/Semantic_output.log");
    of << str << " Output of Semantical Analyzer:" << str << endl << endl;
    for(int i = 0; i < quater_results.size(); i++){
        of << "(" << i+1 << ") " << quater_results[i].op << " " 
                << quater_results[i].exp1 << " " << quater_results[i].exp2
                << " " << quater_results[i].res << endl;
    }
    of << endl;
    of << str << "End of Semantical Analyzer:" << str << endl;

    // 语义分析器控制台输出
    cout << str << " Output of Semantical Analyzer:" << str << endl;
    for(int i = 0; i < quater_results.size(); i++){
        cout << "(" << i+1 << ") " << quater_results[i].op << " " 
                << quater_results[i].exp1 << " " << quater_results[i].exp2
                << " " << quater_results[i].res << endl;
    }
    cout << endl;
}

// 扫描词法分析器的Token序列
void Semantical::parse(){
    // demo1 stk setw(4_0) seq setw(8_2) str 10_4
    // demo2, demo3 stk setw(8_0) seq setw(15_0) str 
    // 语法分析器文件输出
    string str(156, '*');
    outfile << str << " Output of LL1 Syntactic Analyzer:" << str << endl << endl;
    
    // 输入串
    queue<string> expr;
    queue<string> input;

    for(int i = 0; i < tokens.size(); i++){
        if(tokens[i].first == "'"){
            continue;
        }
        string word = tokens[i].first;
        int code = tokens[i].second;
        
        if(word[0] == '\''){
            continue;
        } else if(code == 0){
            // 将标识符转换为id表示
            input.push("id");
        } else if(code == 1){
            // 将常数转换为cons表示
            input.push("cons");
        } else input.push(word);

        expr.push(word);
    }
    // 进行分析
    syntactic_analyse(expr, input);
    outfile << "Everything fine." << endl;
    outfile << str << "   End of LL1 Syntactic analyzer: " << str << endl;
}

// 报错函数
void Semantical::print_error(int error_code, string error_info){
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

// 返回语义分析的结果-四元式序列
vector<quater> Semantical::get_result(){
    return quater_results;
}

#endif