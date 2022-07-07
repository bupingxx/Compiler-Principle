//**************************************************************************
/* File:     compiler.hpp
 * 
 * Input:    源程序
 * Output:   编译环节中的所有输出
 * 
 * Note:     这是一个完整的编译器, 此头文件包含了前端与后端中的所有头文件
 * 
 */
 //**************************************************************************

#ifndef _COMPILER_HPP
#define _COMPILER_HPP

#include "./front_end/semantic.hpp"
#include "./back_end/object.hpp"

// 编译器类
class Compiler{
protected:
    Lexical lex;
    Semantical sem;
    Object obj;

public:
    Compiler(){ };

    // 编译
    void compile(const char* source_code){
        // 词法分析和生成符号表        
        lex.lexical_analyse(source_code);

        // 语法分析和语义分析
        sem.semantical_analyse(lex.get_results());

        // 中间代码优化和生成目标代码
        obj.generate_object(sem.get_result(), lex.get_symbol_system());

    }
    ~Compiler(){ };
};

#endif