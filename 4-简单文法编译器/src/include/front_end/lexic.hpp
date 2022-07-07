//**************************************************************************
/* File:     lexic.hpp
 * 
 * Purpose:  词法分析器,将源程序转换为Token序列
 * 
 * Input:    源程序 demo.c
 * Output:   Token 序列
 */
 //**************************************************************************

#ifndef LEXIC_HPP
#define LEXIC_HPP
#include "../utils/info.hpp"
#include "symbol.hpp"

// 词法分析器类
class Lexical{
protected:
    // 行指标
    int line_index;
    // char类型指标
    int char_flag;
    // 参数个数
    int fn;
    // 参数入口指标
    int program_flag;
    string program_name;
    // 声明指标
    int var_flag;
    // 变量个数
    vector<string> varn;

    // 符号表系统
    Symbol_System symbol_sys;

    // 关键字字典
    vector< pair<string, int> > keywords;

    // 分隔符字典
    vector< pair<string, int> > separators;

    // 运算符字典
    vector< pair<string, int> > opers;

    // 输出结果: Token序列
    vector< pair<string, int> > results;

public:
    Lexical(){ };
    ~Lexical(){ };
    void lexical_analyse(const char* file_name);
    void print_table();
    void init_table();
    int check_keyword(string s);
    int check_separator(string s);
    int check_operator(string s);
    void analyse(const char* file_name);
    void state_change(int& state, char ch);
    void state_to_code(int pre_state, string word);
    void print_error(int error_code, string error_info);
    vector< pair<string, int> > get_results();
    Symbol_System get_symbol_system();
};

// 进行词法分析
void Lexical::lexical_analyse(const char* file_name){
    line_index = 1;
    char_flag = 0;
    fn = 0;
    program_flag = 0;
    program_name = "";   
    var_flag = 0;

    init_table();
    analyse(file_name);
}

// 打印符号与Token的对应关系
void Lexical::print_table(){
    string str(27, '*');
    cout << str << " Table of Symbol -- Token: " << str << endl;
    cout << setiosflags(ios::right);
    cout << setw(10) << "symbol" << setw(10) << "token" << setw(10) << "symbol" << setw(10) << "token";
    cout << setw(10) << "symbol" << setw(10) << "token" << setw(10) << "symbol" << setw(10) << "token";
    cout << endl;
    cout << setw(10) << "IT" << setw(10) << 0 << setw(10) << "CT" << setw(10) << 1;
    cout << setw(10) << "KT" << setw(10) << 2 << setw(10) << "PT" << setw(10) << 3;
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

// 初始化字典, 用于查询Token
void Lexical::init_table(){
    keywords.emplace_back( make_pair("program", 4));
    keywords.emplace_back( make_pair("var", 5));
    keywords.emplace_back( make_pair("integer", 6));
    keywords.emplace_back( make_pair("real", 7));
    keywords.emplace_back( make_pair("char", 8));
    keywords.emplace_back( make_pair("bool", 9));
    keywords.emplace_back( make_pair("true", 10));
    keywords.emplace_back( make_pair("false", 11));
    keywords.emplace_back( make_pair("if", 12));
    keywords.emplace_back( make_pair("elif", 13));
    keywords.emplace_back( make_pair("else", 14));
    keywords.emplace_back( make_pair("while", 15));
    keywords.emplace_back( make_pair("begin", 16));
    keywords.emplace_back( make_pair("end", 17));
    keywords.emplace_back( make_pair("and", 18));
    keywords.emplace_back( make_pair("or", 19));
    keywords.emplace_back( make_pair("not", 20));

    separators.emplace_back( make_pair(":", 21));
    separators.emplace_back( make_pair(";", 22));
    separators.emplace_back( make_pair(",", 23));
    separators.emplace_back( make_pair("'", 24));
    separators.emplace_back( make_pair("(", 25));
    separators.emplace_back( make_pair(")", 26));
    separators.emplace_back( make_pair("[", 27));
    separators.emplace_back( make_pair("]", 28));
    separators.emplace_back( make_pair("{", 29));
    separators.emplace_back( make_pair("}", 30));

    opers.emplace_back( make_pair("+", 31));
    opers.emplace_back( make_pair("-", 32));
    opers.emplace_back( make_pair("*", 33));
    opers.emplace_back( make_pair("/", 34));
    opers.emplace_back( make_pair(":=", 35));
    opers.emplace_back( make_pair(">", 36));
    opers.emplace_back( make_pair("<", 37));
    opers.emplace_back( make_pair("=", 38));

    return;
}

// 检查是否为关键字, 否时返回0
int Lexical::check_keyword(string s){
    for(auto item:keywords){
        if(item.first == s)
            return item.second;
    }
    return 0;
}

// 检查是否为分隔符, 否时返回0
int Lexical::check_separator(string s){
    for(auto item:separators){
        if(item.first == s)
            return item.second;
    }
    return 0;
}

// 检查是否为运算符, 否时返回0
int Lexical::check_operator(string s){
    for(auto item:opers){
        if(item.first == s)
            return item.second;
    }
    return 0;
}

// 核心函数: 进行词法分析
void Lexical::analyse(const char* file_name){
    ifstream infile;
    infile.open(file_name);
    string tmp;

    if(infile.fail()){
        print_error(7, file_name);
    }
    
    // 打印源程序代码
    string str(27, '*');
    cout << str << " Input Demo -- Source Code:" << str << endl;
    while(getline(infile, tmp)){
        cout << tmp << endl;
    }
    cout << endl;
    
    // 重定向至源程序开头
    infile.clear();
    infile.seekg(0);
    
    // 打印符号与Token的对应关系
    print_table();

    // 初始状态(当前状态)
    int state = 0;
    // 上一个状态
    int pre_state = 0;
    string word;

    while(getline(infile, tmp)){
        for(char ch:tmp){
            pre_state = state;
            state_change(state, ch);    // 转换到下一个状态

            if(state != 0){             // 非0状态, 记录word
                word += ch;
            } else {                    // 0状态,将word转化为Token
                state_to_code(pre_state, word);
                word.clear();
                if(ch != ' '){
                    state_change(state, ch);
                    word += ch;
                }
            }
        }
        state_to_code(state, word);     // 该行的最后一个Token
        word.clear();
        state = 0;

        if(char_flag)
            print_error(6, "");         // 单引号不匹配,报错
        line_index++;
    }

    // 打印词法分析器的输出
    ofstream outfile;
    outfile.open("./output/Lexical_output.log");
    outfile << str << "Output of Lexical Analyzer:" << str << endl << endl;

    outfile << str << "       Tokens Results:     " << str << endl;
    for(auto item:results){
        outfile << "< " << item.first << " , " << item.second << " >" << endl; 
    }
    outfile << endl;
    outfile << str << " End of Lexical analyzer: " << str << endl;

    // 控制台输出
    cout << str << "Output of Lexical Analyzer:" << str << endl;
    for(auto item:results){
        cout << "< " << item.first << " , " << item.second << " >" << endl; 
    }
    cout << endl;
    
    // 打印符号表
    symbol_sys.print_system();
    
    // 关闭文件
    infile.close();
    outfile.close();
}

// 状态转换函数,基于DFA
void Lexical::state_change(int& state, char ch){
    switch(state){
        // 0状态
        case 0:
            if(ch == '\''){
                char_flag = char_flag == 0 ? 1 : 0;
                state = 5;
            } else if (char_flag || is_letter(ch))
                state = 1;
            else if (ch == ' ')
                state = 0;
            else if (is_digital(ch))
                state = 2;
            else if (check_separator(string(1, ch)))
                state = 5;
            else if (check_operator(string(1, ch)))
                state = 6;
            else print_error(0, to_string(state) + " - " + string(1, ch));  // 非法状态,报错
            break;
        // 标识符或关键字
        case 1:
            if(is_letter((ch)) || is_digital(ch))
                state = 1;
            else
                state = 0;
            break;
        // 整数或实数
        case 2:
            if(is_digital(ch))
                state = 2;
            else if (ch == '.')
                state = 3;
            else if(is_letter(ch))
                print_error(2, string(1, ch));      // 非法状态,报错
            else
                state = 0;
            break;
        // 小数点
        case 3:
            if(is_digital(ch))
                state = 4;
            else if(ch == '.' || is_letter(ch))
                print_error(2, string(1, ch));      // 非法状态,报错
            else
                state = 0;
            break;
        // 实数
        case 4:
            if(is_digital(ch))
                state = 4;
            else if(ch == '.' || is_letter(ch))
                print_error(2, string(1, ch));      // 非法状态,报错
            else
                state = 0;
        // 分割符
        case 5:
            if(ch == '=')
                state = 6;
            else
                state = 0;
            break;
        // 运算符
        case 6:
            if(check_operator(string(1, ch)))
                state = 4;
            else
                state = 0;
            break;
        // 不属于上述任何一个状态,说明非法状态,报错
        default:
            print_error(3, to_string(state));
    }
}

// 根据上一个状态,将当前的word转换为Token,并记录表中
void Lexical::state_to_code(int pre_state, string word){
    if(pre_state == 0){
        return;
    } else if (pre_state == 1){
        if(char_flag){
            results.emplace_back(make_pair(word, 1));
            // 常量不能重复
            if(!vector_find(symbol_sys.CONSL, word)){
                // 字符类型的常量, 转换为ASCII码, 存入常量表
                symbol_sys.CONSL.emplace_back(to_string(word[0]));
                symbol_sys.create_SYNBL_item(word, -1, 'c', false);
            }
        } else {
            int token = check_keyword(word);
            if(token){
                // 关键字
                results.emplace_back(make_pair(word, token));
                if(word == "program"){
                    program_flag = 1;
                } else if(word == "var"){
                    // 准备声明变量
                    var_flag = 1;
                    varn.clear();
                } else if(word == "begin" && program_name.size() != 0){
                    symbol_sys.create_PFINFL_item(program_name, fn, PROGRAM_ENTRY);
                    program_name.clear();
                } else if((word == "integer" || word == "real" || word == "char" || word == "bool") && var_flag == 1){
                    var_flag = 0;
                    for(auto name:varn){
                            symbol_sys.create_SYNBL_item(name, word[0], 'v', true);
                    }
                }
            } else {
                // 标识符
                results.emplace_back(make_pair(word, 0));
                if(program_flag == 1){
                    // 记录过程名字
                    program_name = word;
                    program_flag = 0;
                    // 记录过程
                    symbol_sys.create_SYNBL_item(program_name, -1, 'p', true);
                } else if(program_name.size() != 0){
                    // 记录参数个数
                    fn++;
                }
                if(var_flag == 1){
                    // 记录声明的变量
                    varn.emplace_back(word);
                }
            }
        }
    } else if (pre_state == 2 || pre_state == 3 || pre_state == 4){
        // 数字类型的常量, 存入常量表
        results.emplace_back(make_pair(word, 1));
        // 常量不能重复
        if(!vector_find(symbol_sys.CONSL, word)){
            symbol_sys.CONSL.emplace_back(word);
            symbol_sys.create_SYNBL_item(word, -1, 'c', false);
        }
    } else if (pre_state == 5){
        // 分隔符
        int token = check_separator(word);
        if(token){
            results.emplace_back(make_pair(word, token));
        } else {
            print_error(4, word);      // error
        }
    } else if (pre_state == 6){
        // 运算符
        int token = check_operator(word);
        if(token){
            results.emplace_back(make_pair(word, token));
        } else {
            print_error(4, word);      // error
        }
    } else {
        print_error(5, word);          // error
    }
}

// 报错函数
void Lexical::print_error(int error_code, string error_info){
    if(error_code != 6 && error_code != 9)
        cout << "In line " << line_index << ": " << endl;
    
    switch(error_code){
        case 0:
            cout << "error in state_change, current state - current char: " << error_info << endl;
            break;
        case 1:
            cout << "error: too many decimal points in number." << endl;
            break;
        case 2:
            cout << "error: unable to find numeric literal operator: " << error_info << endl;
            break;
        case 3:
            cout << "error in state_change, there is a wrong state: " << error_info << endl;
            break;
        case 4:
            cout << "stray \"" << error_info << "\" in program." << endl;
            break;
        case 5:
            cout << "error in state_to_code: " << error_info << endl;
            break;
        case 6:
            cout << "missing terminating ' character" << endl;
            break;
        case 7:
            cout << "Cannot open file:" << error_info << endl;
            break;
        default:
            cout << "Unknown error token:" << error_code << endl;
            break;
    }
    exit(error_code);
}

// 返回词法分析的结果-Token序列
vector< pair<string, int> > Lexical::get_results(){
    return results;
}

// 返回符号表系统
Symbol_System Lexical::get_symbol_system(){
    return symbol_sys;
}

#endif