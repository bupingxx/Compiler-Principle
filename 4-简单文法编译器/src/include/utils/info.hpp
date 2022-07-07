//**************************************************************************
/* File:     info.hpp
 * 
 * Note:     工具包
 * 
 */
 //**************************************************************************

#ifndef _INFO_HPP
#define _INFO_HPP
#include <string>
#include <vector>
#include <queue>
#include <stack>

// 返回类型变量的长度
int _size_of_(char tval){
    if(tval == 'i'){
        return 4;
    } else if(tval == 'r'){
        return 8;
    } else if(tval == 'c'){
        return 1;
    } else if(tval == 'b'){
        return 1;
    }
    return 0;
}

// 将大等于0的数字转换为字符串, 负数返回/
std::string convert_typ(int t){
    if(t >= 0){
        return std::to_string(t);
    }
    return "/";
}

// 检查是否为字母
bool is_letter(char ch){
    return ch == '_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

// 检查是否为数字
bool is_digital(char ch){
    return (ch >= '0' && ch <= '9');
}

// 检查是否是常数
bool is_constant(std::string str){
    for(auto ch:str){
        if(is_digital(ch) || ch == '.' || ch == '_'){
            continue;
        } else {
            return str == "true" || str == "false";
        }
    }
    return true;
}

// 检查是否是实数
bool is_real(std::string str){
    for(auto ch:str){
        if(ch == '.'){
            return true;
        }
    }
    return false; 
}

// 检查vector中是否存在某个元素
bool vector_find(std::vector<std::string> v, std::string obj){
    for(auto item: v){
        if(item == obj){
            return true;
        }
    }
    return false;
}

// 将队列的元素转换为string
std::string get_queue(std::queue<std::string> que){
    std::string res = "";
    while(!que.empty()){
        res += que.front();
        que.pop();
    }
    return res;
}

// 将栈的元素转换为string
std::string get_stack(std::stack<std::string> stk, int type){
    std::vector<std::string> tmp;
    while(!stk.empty()){
        tmp.emplace_back(stk.top());
        stk.pop();
    }

    std::string res = "";
    for(int i = tmp.size()-1; i >= 0; i--){
        res += tmp[i];
        if(type == 1)
            res += " ";
    }
        
    return res;
}

// 将bool转换为string
std::string bool_to_string(bool b){
    if(b==true){
        return "true";
    } else {
        return "false";
    }
}

// 识别字符串中的斜杠/
std::pair<std::string, std::string> string_convert(std::string str){
    int idx = -1;
    for(int i = 0; i < str.size(); i++){
        if(str[i] == '/'){
            idx = i;
            break;
        }
    }
    std::string a = str.substr(0, idx);
    std::string b = str.substr(idx + 1, str.size()-idx-1);
    return make_pair(a, b);
}

#endif