//**************************************************************************
/* File:     quater.hpp
 * 
 * Note:     声明与定义四元式结构体
 * 
 */
 //**************************************************************************
#ifndef _QUATER_HPP
#define _QUATER_HPP
#include <string>

struct quater{
public:
    std::string op;
    std::string exp1;
    std::string exp2;
    std::string res;
    bool act1;  //活跃信息
    bool act2;
    bool act3;
    quater(std::string o = "", std::string e1 = "", std::string e2 = "", std::string r = ""):op(o), exp1(e1), exp2(e2), res(r){ };
    void clear(){
        op = "";
        exp1 = "";
        exp2 = "";
        res = "";
    }

    // 判断四元式是否为空
    bool empty(){
        return op == "";
    }

    // 返回四元式打印格式
    std::string get_string(){
        return op + " " + exp1 + " " + exp2 + " " + res;
    }

    // 返回活跃信息
    std::string is_active(std::string exp, bool active){
        return exp == "_" ? ("") : (active ? "(y)" : "(n)");
    }

    // 返回四元式打印格式(包含活跃信息)
    std::string get_active_string(){
        return op + " " + exp1 + is_active(exp1, act1) + " " + exp2 + is_active(exp2, act2) + " " + res + is_active(res, act3);
    }

    // 重写操作符, 判断四元式是否相等
    bool operator ==(quater &others){
        if(op == others.op && exp1 == others.exp1 && exp2 == others.exp2 && res == others.res){
            return true;
        } else {
            return false;
        }
    }
};

#endif