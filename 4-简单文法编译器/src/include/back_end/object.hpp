//**************************************************************************
/* File:     object.hpp
 * 
 * Purpose:  根据优化后的中间代码生成目标代码
 * 
 * Input:    优化后的中间代码
 * Output:   目标代码
 * 
 */
 //**************************************************************************

#ifndef _OBJECT_HPP
#define _OBJECT_HPP
#include "../front_end/symbol.hpp"
#include "optimize.hpp"

// 目标代码生成器类
class Object{
protected:
    Optimizer opt;
    Symbol_System global_symbols;
    // 跳转指令
    string jmp;
    // 局部变量的偏移
    int local_v_off;
    // 局部变量表
    unordered_map<string, int> local_variable;
public:
    Object(){
        jmp = "jle";
        local_v_off = 4;
    }
    ~Object(){ }
    string get_addr(string target);
    string convert_operator(string op);
    void generate_object(vector<quater> input, Symbol_System ss);
};

// 返回target的内存地址
string Object::get_addr(string target){
    
    if(is_constant(target)){
        // 先判断是否是常量
        return target;
    } else if(target[0] == 't'){
        // 再判断是否是临时变量
        if(local_variable.count(target) == 0){
            local_variable[target] = local_v_off;
            local_v_off += 4;
        }
        return "[ebp-" + to_string(local_variable[target]) + "]";
    } else {
        // 否则是非临时变量, 到符号表系统中的结构表中找内存地址(即参数的地址)
        int off = global_symbols.get_var_addr(target);
        // 栈结构中参数地址从ebp+8开始
        return "[ebp+" + to_string(off+8) + "]";
    }
    return "";
}

// 返回算符对应的操作码
string Object::convert_operator(string op){
    if(op == "+"){
        return "add";
    } else if(op == "-"){
        return "sub";
    } else if(op == "*"){
        return "imul";
    } else if(op == "/"){
        return "idiv";
    } else if(op == "and"){
        jmp = "jle";
        return "and";
    } else if(op == "or"){
        jmp = "jle";
        return "or";
    } else if(op == ">"){
        jmp = "jle";
        return "sub";
    } else if(op == "="){
        jmp = "jne";
        return "sub";
    } else if(op == "<"){
        jmp = "jge";
        return "sub";
    }
    return "ERROR";
}

// 生成目标代码
void Object::generate_object(vector<quater> input, Symbol_System ss){
     // 中间代码优化
    opt.optimize(input);

    // 输出生成目标代码的过程
    ofstream of;
    of.open("./output/Object_output.log");
    string str(26, '*');
    of << str << " Output of Object Generater: " << str << endl << endl;
    cout << str << " Output of Object Generater: " << str << endl;

    // 获得词法分析器的符号表
    global_symbols = ss;

    // 以基本块为单位
    for(int i = 0; i < opt.bbs.size(); i++){
        basic_block& bb = opt.bbs[i];
        // 经过优化后的基本块可能是空的, 跳过
        if(bb.empty()){
            continue;
        }

        // 根据词法分析的符号表初始化非临时变量的活跃信息
        unordered_map<string, bool> SYMBL;
        SYMBL["_"] = false;
        int param = global_symbols.PFINFL[0].PARAM;
        int fn = global_symbols.PFINFL[0].FN;
        for(int j = 0; j < fn; j++){
            string name = global_symbols.RINFL[param + j].ID;
            SYMBL[name] = true;
        } 
        // 逆序生成四元式的活跃信息
        for(int j = bb.block_quaters.size()-1; j > -1; j--){
            quater& quad = bb.block_quaters[j];
            
            // 初始化临时变量的活跃信息, 并计入到局部变量表中
            if( !is_constant(quad.exp1) && SYMBL.count(quad.exp1) == 0){
                SYMBL[quad.exp1] = false;
            }
            if( !is_constant(quad.exp2) && SYMBL.count(quad.exp2) == 0){
                SYMBL[quad.exp2] = false;
            }
            if( !is_constant(quad.res)  && SYMBL.count(quad.res) == 0){
                SYMBL[quad.res] = false;
            }

            // 第一步, QT[res.L] = SYMBL(res.L)
            quad.act3 = SYMBL[quad.res];
            // 第二步, SYMBL[res.L] = false
            SYMBL[quad.res] = false;
            // 第三步, QT[exp1.L, exp2.L] = SYMBL(exp1.L, exp2.L)
            quad.act1 = SYMBL[quad.exp1];
            quad.act2 = SYMBL[quad.exp2];
            // 第四步, SYMBL(exp1.L, exp2.L) = true
            SYMBL[quad.exp1] = true;
            SYMBL[quad.exp2] = true;
        }
    }
    for(int i = 0; i < opt.bbs.size(); i++){
        basic_block& bb = opt.bbs[i];
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater& quad = bb.block_quaters[j];
            of << "(" << i+1 << ") " << quad.get_active_string() << endl;
        }
    }

    /*
        选用x86指令集, 生成目标代码
    */
    string RDL = "";            // 寄存器描述表
    string eax = "eax";         // 仅使用eax寄存器
    vector<string> OBJ;         // 目标代码区
    string TAB = "    ";        // 换行符
    ofstream outfile;      
    outfile.open("object.asm"); // 汇编程序
    
    // 以基本块为单位
    for(int i = 0; i < opt.bbs.size(); i++){
        basic_block& bb = opt.bbs[i];
        // 经过优化后的基本块可能是空的, 跳过
        if(bb.empty()){
            continue;
        }
        // 初始化块内的活跃信息表
        unordered_map<string, bool> active_table;

        // 基本块入口
        RDL = "";
        active_table.clear();
        bool is_label = false;

        // 以四元式为单位
        for(int j = 0; j < opt.bbs[i].block_quaters.size(); j++){
            quater& quad = opt.bbs[i].block_quaters[j];
            string LABEL = TAB;
            if(j == 0 || is_label == true){
                LABEL = "B" + to_string(i) + ": ";
                is_label = false;
            }

            if(quad.op== ":="){
                // 赋值四元式
                if(RDL.size() == 0){
                    OBJ.emplace_back(LABEL + "mov " + eax + ", " + get_addr(quad.exp1));
                } else if(RDL == quad.exp1){
                    if(quad.act1 == true){
                        OBJ.emplace_back(LABEL + "mov " + get_addr(quad.exp1) + ", " + eax);
                    }
                } else if(RDL != quad.res){
                    if(active_table[RDL] == true){
                        OBJ.emplace_back(LABEL + "mov " + get_addr(RDL) + ", " + eax);
                        OBJ.emplace_back(TAB + "mov " + eax + ", " + get_addr(quad.exp1));
                    } else {
                        OBJ.emplace_back(LABEL + "mov " + eax + ", " + get_addr(quad.exp1));
                    }
                }
                RDL = quad.res;
            } else if(quad.op == "wh"){
                // 循环入口四元式
                // do nothing
            } else if(quad.op == "do" || quad.op == "if" || quad.op == "elif"){
                // 循环/条件四元式
                if(RDL.size() == 0){
                    OBJ.emplace_back(LABEL + "mov " + eax + ", " + get_addr(quad.exp1));
                    OBJ.emplace_back(TAB + "cmp " + eax + ", 0");
                } else if(RDL == quad.exp1){
                    if(quad.act1 == true){
                        OBJ.emplace_back(LABEL + "mov " + get_addr(quad.exp1) + ", " + eax);
                        OBJ.emplace_back(TAB + "cmp " + eax + ", 0");
                    } else {
                        OBJ.emplace_back(LABEL + "cmp " + eax + ", 0");
                    }
                } else {
                    if(active_table[RDL] == true){
                        OBJ.emplace_back(LABEL + "mov " + get_addr(RDL) + ", " + eax);
                        OBJ.emplace_back(TAB + "mov " + eax + ", " + get_addr(quad.exp1));
                        OBJ.emplace_back(TAB + "cmp " + eax + ", 0");
                    } else {
                        OBJ.emplace_back(TAB + "mov " + eax + ", " + get_addr(quad.exp1));
                        OBJ.emplace_back(LABEL + "cmp " + eax + ", 0");
                    }
                }

                OBJ.emplace_back(TAB + jmp + " B" + to_string(bb._goto_false));
                RDL.clear();

            } else if(quad.op == "we" || quad.op == "ife" || quad.op == "elife"){
                // 循环/条件结束四元式
                if(RDL.size() != 0 && active_table[RDL] == true){
                    OBJ.emplace_back(LABEL + "mov " + get_addr(RDL) + ", " + eax);
                    LABEL = TAB;
                }
                OBJ.emplace_back(LABEL + "jmp B" + to_string(bb._goto_true));
                RDL.clear(); 
            } else if(quad.op == "el"){
                is_label = true;
            } else if(quad.op == "ie"){
                // do nothing
            } else {
                // 运算四元式
                if(RDL.size() == 0){
                    OBJ.emplace_back(LABEL + "mov " + eax + ", " + get_addr(quad.exp1));
                    OBJ.emplace_back(TAB + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp2));
                } else if(RDL == quad.exp1){
                    if(quad.act1 == true){
                        // 若活跃
                        OBJ.emplace_back(LABEL + "mov " + get_addr(quad.exp1) + ", " + eax);
                        OBJ.emplace_back(TAB + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp2));
                    } else {
                        OBJ.emplace_back(LABEL + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp2));
                    }
                } else if(RDL == quad.exp2 && (quad.op == "+" || quad.op == "*" || quad.op == "and" || quad.op == "or" || quad.op == "=")){
                    if(quad.act2 == true){
                        // 若活跃
                        OBJ.emplace_back(LABEL + "mov " + get_addr(quad.exp2) + ", " + eax);
                        OBJ.emplace_back(TAB + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp1));
                    } else {
                        OBJ.emplace_back(LABEL + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp1));
                    }
                } else {
                    if(RDL != quad.res && active_table[RDL] == true){
                        OBJ.emplace_back(LABEL + "mov " + get_addr(RDL) + ", " + eax);
                        OBJ.emplace_back(TAB + "mov " + eax + ", " + get_addr(quad.exp1));
                        OBJ.emplace_back(TAB + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp2));
                    } else {
                        OBJ.emplace_back(LABEL + "mov " + eax + ", " + get_addr(quad.exp1));
                        OBJ.emplace_back(TAB + convert_operator(quad.op) + " " + eax + ", " + get_addr(quad.exp2));
                    }
                }
                // 登记寄存器描述表
                RDL = quad.res;
            }
            // 记录活跃信息
            active_table[quad.exp1] = quad.act1;
            active_table[quad.exp2] = quad.act2;
            active_table[quad.res] = quad.act3;
        }
        // 基本块出口, 清除寄存器描述表
        if(RDL.size() != 0 && active_table[RDL] == true){
            OBJ.emplace_back(TAB + "mov " + get_addr(RDL) + ", " + eax);
        }
        OBJ.emplace_back("");
    }

    // 声明程序
    outfile << "global " << global_symbols.SYNBL[0].NAME << endl << endl 
            << global_symbols.SYNBL[0].NAME << ":" << endl;

    // 保存栈指针
    outfile << TAB << "push ebp" << endl;
    // 程序入口地址
    outfile << TAB << "mov ebp, esp" << endl;
    // 为局部临时变量声明内存空间
    if(local_v_off-4 != 0){
        outfile << TAB << "sub esp, " << local_v_off-4 << endl;
    }
    /*
        仅使用单寄存器eax, 无需保存其他寄存器的值
    */
    outfile << endl;

    // 将目标代码输出到文件中
    for(int i = 0; i < OBJ.size(); i++){
        outfile << OBJ[i] << endl;
    }

    // 释放临时变量
    outfile << "END:" << "mov esp, ebp" << endl;
    // 还原栈指针
    outfile << TAB << "pop ebp" << endl;
    // 返回
    outfile << TAB << "ret" << endl;
    outfile.close();

    // 控制台输出
    ifstream infile;
    infile.open("object.asm");
    string tmp;
    while(getline(infile, tmp)){
        cout << tmp << endl;
    }
    cout << endl;
    infile.close();
}

#endif