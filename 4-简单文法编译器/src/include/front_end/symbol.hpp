//**************************************************************************
/* File:     symbol.hpp
 * 
 * Purpose:  符号表系统, 从词法分析时开始维护
 * 
 * Input:    源程序
 * Output:   带有活跃信息的符号表系统
 * 
 */
 //**************************************************************************

#ifndef _SYMBOL_HPP
#define _SYMBOL_HPP
#include "../utils/info.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
using namespace std;

// 程序入口
#define PROGRAM_ENTRY 0x00001000 

// 总表项目
struct SYNBL_item{
public:
    string NAME;    // 标识符源码
    int TYP;        // 指针，指向TYPEL表
    char CAT;       // 种类, p-程序， c-常量， t-类型， v-程序变量
    int ADDR;       // 指针，根据CAT指向不同的表 
                    // p指向PFINFL, c指向CONSL， t指向RINFL，v指向LENL
    bool L;         // 活跃信息, true代表yes, false代表no
    SYNBL_item(){ };
    SYNBL_item(string name, int typ, char cat, int addr, bool l):NAME(name), TYP(typ), CAT(cat), ADDR(addr), L(l){ };
};

// 类型表项目
struct TYPEL_item{
public:
    char TVAL;    // 类型代码, i整型, r实型, c字符型, b布尔型, d结构型
    TYPEL_item(char tval):TVAL(tval){ };
};

// 结构表
struct RINFL_item{
public:
    string ID;   // 域名
    int OFF;     // 第i个ID首地址相对结构头的距离
    int TP;      // 域成分类型指针
    RINFL_item(string id, int off, int tp):ID(id),OFF(off),TP(tp){ };
};

// 过程表
struct PFINFL_item{
    int FN;      // 参数个数
    int PARAM;   // 指针，指向形参表
    int ENTRY;   // 目标代码运行时，程序首地址
    PFINFL_item(int fn, int param, int entry):FN(fn), PARAM(param), ENTRY(entry){ };
};

// 符号表系统类
class Symbol_System{
protected:
    // 长度表, 存放相应数据类型所占字节长度
    vector<int> LENL;

    // 类型表, 存放变量的类型
    vector<TYPEL_item> TYPEL;

    // 结构表, 存放结构的内容
    vector<RINFL_item> RINFL;

    // 过程表, 存放过程(程序)的信息
    vector<PFINFL_item> PFINFL;
public:
    // 符号表总表
    vector<SYNBL_item> SYNBL;

    // 常数表, 存放常量的初值
    vector<string> CONSL;
    
    Symbol_System(){ }
    ~Symbol_System(){ }
    int find_identify_name(string target);
    int find_const_name(string target);
    void create_PFINFL_item(string program, int fn, int program_entry);
    void create_SYNBL_item(string name, char typ, char cat, bool active);
    int get_var_addr(string name);
    string get_addr(int idx);
    void clear();
    void print_system();
    friend class Object;
};

// 找到标识符target在符号表总表的下标
int Symbol_System::find_identify_name(string target){
    for(int i = 0; i < SYNBL.size(); i++){
        if(target == SYNBL[i].NAME){
            return i;
        }
    }
    return -1;
}

// 找到常量target在符号表总表的下标
int Symbol_System::find_const_name(string target){
    for(int i = 0; i < CONSL.size(); i++){
        if(target == CONSL[i]){
            return i;
        }
    }
    return -1;
}

// 创建一个过程表项目, fn为该过程的参数个数
void Symbol_System::create_PFINFL_item(string program, int fn, int entry){
    // 找到过程在符号表总表的下标
    int program_idx = find_identify_name(program);

    // 地址指向过程表的下一个位置
    SYNBL[program_idx].ADDR = PFINFL.size();

    // 参数指针指向结构表的下一个位置(形参表)
    int param = RINFL.size();

    // 构建形参表
    int off = 0;
    for(int i = 1; i <= fn; i++){
        // 参数在符号表总表的下标
        int var_idx = program_idx + i;
        string id = SYNBL[var_idx].NAME;
        int tp = SYNBL[var_idx].TYP;
        RINFL_item new_RINFL_item = RINFL_item(id, off, tp);
        RINFL.push_back(new_RINFL_item);
        // 增加偏移
        int len_idx = SYNBL[var_idx].ADDR;
        off += LENL[len_idx];
    }

    // 构建过程表项目并压入过程表
    PFINFL_item new_PFINFL_item = PFINFL_item(fn, param, entry);
    PFINFL.push_back(new_PFINFL_item);
}

// 创建一个总表项目
void Symbol_System::create_SYNBL_item(string name, char tval, char cat, bool active){
    SYNBL_item new_SYNBL_item;
    if(cat == 'p'){
        // 程序, 指向PFINFL, 由词法分析器进一步构造
        new_SYNBL_item = SYNBL_item(name, -1, cat, -1, active);
    } else if(cat == 'c'){
        // 常量, 指向CONSL
        int addr = CONSL.size()-1;
        new_SYNBL_item = SYNBL_item(name, -1, cat, addr, active);
    } else if(cat == 't'){
        // 类型, 指向RINFL
        int addr = RINFL.size();
        new_SYNBL_item = SYNBL_item(name, -1, cat, addr, active);
    } else if(cat == 'v'){
        // 变量，指向LENL
        int addr = LENL.size();
        LENL.emplace_back(_size_of_(tval));
        // 创建类型表新项目
        int typ = TYPEL.size();
        TYPEL_item new_TYPEL_item = TYPEL_item(tval);
        TYPEL.emplace_back(new_TYPEL_item);
        // 创建总表项目
        new_SYNBL_item = SYNBL_item(name, typ, cat, addr, active);
    }
    SYNBL.emplace_back(new_SYNBL_item);
}

// 获得参数变量的内存偏移地址
int Symbol_System::get_var_addr(string name){
    for(int i = 0; i < RINFL.size(); i++){
        if(RINFL[i].ID == name){
            return RINFL[i].OFF;
        }
    }
    return -1;
}

// 获得总表中的ADDR指向地址
string Symbol_System::get_addr(int idx){
    char cat = SYNBL[idx].CAT;
    string addr = to_string(SYNBL[idx].ADDR);
    if(cat == 'p'){
        return "PFINFL[" + addr + "]";
    } else if(cat == 'c'){
        return "CONSL[" + addr + "]";
    } else if(cat == 't'){
        return "RINFL[" + addr + "]";
    } else if(cat == 'v'){
        return "LENL[" + addr + "]";
    }
    return "";
}

// 清空符号表
void Symbol_System::clear(){
    CONSL.clear();
    LENL.clear();
    SYNBL.clear();
    TYPEL.clear();
    RINFL.clear();
    PFINFL.clear();
}

// 打印符号表
void Symbol_System::print_system(){
    // 文件输出
    ofstream outfile;
    outfile.open("./output/Symbols_output.log");
    string str(27, '*');
    outfile << str << "  Output of Symbol System: " << str << endl << endl;
    outfile << setiosflags(ios::right);
    outfile << str << "---------SYNBL----------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "NAME" << setw(10) << "TYP"
            << setw(10) << "CAT" << setw(17) << "ADDR" << setw(14) << "L" << endl;
    for(int i = 0; i < SYNBL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << SYNBL[i].NAME << setw(10)
            << convert_typ(SYNBL[i].TYP) << setw(10) << SYNBL[i].CAT << setw(17) << get_addr(i) 
            << setw(14) << bool_to_string(SYNBL[i].L) << endl;
    }
    outfile << endl;
    outfile << str << "---------TYPEL----------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "TVAL" << endl;
    for(int i = 0; i < TYPEL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << TYPEL[i].TVAL << endl;
    }
    outfile << endl;
    outfile << str << "---------CONSL----------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "CONST" << endl;
    for(int i = 0; i < CONSL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << CONSL[i] << endl;
    }
    outfile << endl;
    outfile << str << "---------LENL-----------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "LEN" << endl;
    for(int i = 0; i < LENL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << LENL[i] << endl;
    }
    outfile << endl;
    outfile << str << "---------RINFL----------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "ID" << setw(10) << "OFF" << setw(10) << "TP" << endl;
    for(int i = 0; i < RINFL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << RINFL[i].ID << setw(10) << RINFL[i].OFF
        << setw(10) << RINFL[i].TP << endl;
    }
    outfile << endl;
    outfile << str << "---------PFINFL---------" << str << endl;
    outfile << setw(6) << "IDX" << setw(20) << "FN" << setw(20) << "PARAM" << setw(10) << "ENTRY" << endl;
    for(int i = 0; i < PFINFL.size(); i++){
        outfile << setw(6) << "[" + to_string(i) + "]" << setw(20) << PFINFL[i].FN << setw(20) << "RINFL[" + to_string(PFINFL[i].PARAM) + "]"
        << setw(10) << PFINFL[i].ENTRY << endl;
    }
    outfile << endl;   
    outfile << str << "  End of Symbol System: " << str << endl;

    // 控制台输出
    cout << str << "  Output of Symbol System: " << str << endl;
    cout << setiosflags(ios::right);
    cout << setw(6) << "IDX" << setw(20) << "NAME" << setw(10) << "TYP"
            << setw(10) << "CAT" << setw(17) << "ADDR" << setw(14) << "L" << endl;
    for(int i = 0; i < SYNBL.size(); i++){
        cout << setw(6) << "[" + to_string(i) + "]" << setw(20) << SYNBL[i].NAME << setw(10)
            << convert_typ(SYNBL[i].TYP) << setw(10) << SYNBL[i].CAT << setw(17) << get_addr(i) 
            << setw(14) << bool_to_string(SYNBL[i].L) << endl;
    }
    cout << endl;
}

#endif