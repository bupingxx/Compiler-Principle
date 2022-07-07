//**************************************************************************
/* File:     syntax.hpp
 * 
 * Purpose:  语法分析器, 使用LL(1)分析法, 判断Token序列是否符合文法
 * 
 * Input:    词法分析器的输出-Token序列
 * Output:   是否符合文法
 * 
 * Note:     此文件仅声明了语法分析器的抽象类, 具体实现请查看语义分析器
 */
 //**************************************************************************

#ifndef _SYNTAX_HPP
#define _SYNTAX_HPP
#include "lexic.hpp"

// 语法分析器类
class Syntactic{
protected:
    // 词法分析器输出的Token序列
    vector< pair<string, int> > tokens;

    // 语法分析表
    vector<vector<vector<string>>> table;
    
public:
    Syntactic(){ };
    ~Syntactic(){ };
    // 纯虚函数, 由子类进行定义
    virtual void init_table() = 0;
    virtual void syntactic_analyse(queue<string> expr, queue<string> input) = 0;
};

#endif