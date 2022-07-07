//**************************************************************************
/* File:     optimize.hpp
 * 
 * Purpose:  生成基本块, 并优化中间代码
 * 
 * Input:    语义分析器的输出-四元式序列
 * Output:   优化后的四元式序列
 * 
 */
 //**************************************************************************
#ifndef _OPTIMIZE_HPP
#define _OPTIMIZE_HPP

#include "../utils/quater.hpp"
#include "../utils/info.hpp"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <unordered_map>
using namespace std;


// 基本块结构体
struct basic_block{
public:
    vector<quater> block_quaters;
    int _goto_true;
    int _goto_false;
    bool in_while;
    bool in_if;
    basic_block(){ }
    basic_block(vector<quater> input, int _gt, int _gf, int is_in_while, int is_in_if){
        block_quaters = input;
        _goto_true = _gt;
        _goto_false = _gf;
        in_while = (is_in_while == -1 ? false : true);
        in_if = (is_in_if == -1 ? false: true);
    };
    bool empty(){
        return block_quaters.size() == 0;
    }
    void clear(){
        block_quaters.clear();
    }
};

// 空基本块
basic_block empty_bb = basic_block();

// 优化器
class Optimizer{
protected:
    ofstream outfile;
    // 语义分析器输出的四元式序列, 作为优化器的输入
    vector<quater> input_quaters;

    // 基本块序列
    vector<basic_block> bbs;

    // 优化结果
    vector<quater> output_quaters;

    // if语句的出口
    int _if_out;
public:
    Optimizer(){
        _if_out = -1;
    };
    void optimize(vector<quater> input);
    void build_basic_blocks();
    int build_a_bb(int entry, int while_bb_idx, int& if_bb_idx, int cur_bb_idx, int &bb_cnt);
    void analyse();
    void print_bbs();
    void generate_results();
    vector<quater> get_result();
    friend class Object;
};

// 进行中间代码优化
void Optimizer::optimize(vector<quater> input){
    outfile.open("./output/Optimizer_output.log");
    for(int i = 0; i < 256; i++){
        bbs.push_back(empty_bb);    // 预留256个基本块的内存
    }
    input_quaters = input;
    analyse();
    generate_results();
}

// 构建基本块
void Optimizer::build_basic_blocks(){
    int if_bb_idx = -1;
    int bb_cnt = 0;
    build_a_bb(0, -1, if_bb_idx, 0, bb_cnt);

    // 保留有效的基本块
    for(int i = 0; i < 256; i++){
        if(bbs[i].empty()){
            bbs.resize(i);
            break;
        }
    }
}

// 递归构建基本块函数
// 四元式序列-q下标, 基本块序列-bb下标
// entry 基本块入口的q下标
// while_bb_idx 若当前基本块在while循环内，则为while条件基本块的bb下标; 若不在则为-1
// if_bb_idx    不符合if条件的基本块的bb下标
// cur_bb_idx   当前基本块的bb下标
// 返回该基本块出口的q下标
int Optimizer::build_a_bb(int entry, int while_bb_idx, int& if_bb_idx, int cur_bb_idx, int &bb_cnt){
    // q下标
    int q_idx = entry;

    // 是否是最后一个基本块
    bool last_bb = false;

    // 该基本块的四元式序列
    vector<quater> cur_quaters;

    // 递归生成的基本块数量
    int cur_bb_cnt = 1;

    while(q_idx < input_quaters.size()){
        // 当前四元式即为当前基本块的入口
        quater quad = input_quaters[q_idx];
        cur_quaters.emplace_back(quad);

        if(quad.op == "wh"){
            // 遇到wh
            int _goto_idx = cur_bb_idx + cur_bb_cnt;
            q_idx = build_a_bb(q_idx+1, while_bb_idx, if_bb_idx, _goto_idx, cur_bb_cnt);  // 构建while条件的基本块
            basic_block bb = basic_block(cur_quaters, _goto_idx, -1, while_bb_idx, -1);
            bbs[cur_bb_idx] = bb;
            break;      // 只要当前基本块构造完了 就要break

        } else if(quad.op == "do"){

            int _goto_true_idx = cur_bb_idx + cur_bb_cnt;
            q_idx = build_a_bb(q_idx+1, cur_bb_idx, if_bb_idx, _goto_true_idx, cur_bb_cnt);  // 构建符合while条件的基本块
            int _goto_false_idx = cur_bb_idx + cur_bb_cnt;
            q_idx = build_a_bb(q_idx+1, while_bb_idx, if_bb_idx, _goto_false_idx, cur_bb_cnt);  // 构建不符合while条件的基本块

            basic_block bb = basic_block(cur_quaters, _goto_true_idx, _goto_false_idx, while_bb_idx, -1);
            bbs[cur_bb_idx] = bb;
            break;      // 只要当前基本块构造完了 就要break

        } else if(quad.op == "we"){

            basic_block bb = basic_block(cur_quaters, while_bb_idx, -1, while_bb_idx, -1);
            bbs[cur_bb_idx] = bb;
            break;      // 只要当前基本块构造完了 就要break
        } else if(quad.op == "if" || quad.op == "elif"){

            int _goto_true_idx = cur_bb_idx + cur_bb_cnt;
            q_idx = build_a_bb(q_idx+1, while_bb_idx, if_bb_idx, _goto_true_idx, cur_bb_cnt);  // 构建符合if条件的基本块
            /* 
                注意: 不符合条件的基本块应该由ife或elife对应的四元式去构建， 否则会造成重复
            */
            basic_block bb = basic_block(cur_quaters, _goto_true_idx, if_bb_idx, while_bb_idx, 1);
            bbs[cur_bb_idx] = bb;
            break;      // 只要当前基本块构造完了 就要break

        } else if(quad.op == "ife" || quad.op == "elife"){

            int _goto_idx = cur_bb_idx + cur_bb_cnt;
            q_idx = build_a_bb(q_idx+1, while_bb_idx, if_bb_idx, _goto_idx, cur_bb_cnt);  // 构建不符合if条件的基本块
            if(_if_out == -1){
                _if_out = _goto_idx;
            }
            basic_block bb = basic_block(cur_quaters, _if_out, -1, while_bb_idx, 1);
            bbs[cur_bb_idx] = bb;
            if_bb_idx = _goto_idx;  // 不符合if条件的基本块的bb下标
            break;      // 只要当前基本块构造完了 就要break
        } else if(quad.op == "ie"){
            int _goto_idx = cur_bb_idx + cur_bb_cnt;
            _if_out = _goto_idx;
            q_idx = build_a_bb(q_idx+1, while_bb_idx, if_bb_idx, _goto_idx, cur_bb_cnt);
            basic_block bb = basic_block(cur_quaters, _goto_idx, -1, while_bb_idx, -1);
            bbs[cur_bb_idx] = bb;
            break;
        } else {
            q_idx++;
            if(q_idx == input_quaters.size()){
                last_bb = true;
            }
        }
    }

    // 最后的基本块
    if(last_bb){ 
        basic_block bb = basic_block(cur_quaters, -1, -1, while_bb_idx, -1);
        bbs[cur_bb_idx] = bb;
    }

    bb_cnt += cur_bb_cnt;
    return q_idx;
}

// 核心函数: 进行优化
void Optimizer::analyse(){
    string str(29, '*');
    cout << str << " Output of Optimizer:  " << str << endl;
    outfile << str << " Output of Optimizer:  " << str << endl << endl;

    // 构建基本块
    build_basic_blocks();

    // 打印未优化的基本块
    outfile << str << "     Basic Blocks:    " << str << endl;
    print_bbs();

    // 基于基本块进行局部优化
    outfile << str << " Optimize Information:" << str << endl;
    for(int i = 0; i < bbs.size(); i++){
        basic_block& bb = bbs[i];

        // 经过优化后的基本块可能是空的, 不打印
        if(bb.empty()){
            continue;
        }

        // 打印当前基本块的优化内容
        outfile << "In Basic Block " << i << ":"<< endl;
        outfile << "** Optimize Operation:" << endl;
        // 第一步, 常数合并
        unordered_map<string, string> const_map;
        
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater& quad = bb.block_quaters[j];

            if(const_map.count(quad.exp1)){
                quad.exp1 = const_map[quad.exp1];
            }
            if(const_map.count(quad.exp2)){
                quad.exp2 = const_map[quad.exp2];
            }

            // 运算符的两个操作数均为常数
            if(quad.res != "_" && is_constant(quad.exp1) && is_constant(quad.exp2) ){
                bool to_print = true;
                if(quad.op == "+"){
                    quad.op = ":=";
                    quad.exp1 = to_string(stoi(quad.exp1) + stoi(quad.exp2));
                    quad.exp2 = "_";
                } else if(quad.op == "-"){
                    quad.op = ":=";
                    quad.exp1 = to_string(stoi(quad.exp1) - stoi(quad.exp2));
                    quad.exp2 = "_";
                } else if(quad.op == "*"){
                    quad.op = ":=";
                    quad.exp1 = to_string(stoi(quad.exp1) * stoi(quad.exp2));
                    quad.exp2 = "_";
                } else if(quad.op == "/"){
                    quad.op = ":=";
                    quad.exp1 = to_string((int)(stoi(quad.exp1) / stoi(quad.exp2)));
                    quad.exp2 = "_";
                } else if(quad.op == ">"){
                    quad.op = ":=";
                    quad.exp1 = stoi(quad.exp1) > stoi(quad.exp2) ? "true" : "false";
                    quad.exp2 = "_";
                } else if(quad.op == "<"){
                    quad.op = ":=";
                    quad.exp1 = stoi(quad.exp1) < stoi(quad.exp2) ? "true" : "false";
                    quad.exp2 = "_";
                } else if(quad.op == "="){
                    quad.op = ":=";
                    quad.exp1 = stoi(quad.exp1) == stoi(quad.exp2) ? "true" : "false";
                    quad.exp2 = "_";
                } else if(quad.op == "and"){
                    quad.op = ":=";
                    quad.exp1 = (quad.exp1 == "true" && quad.exp2 == "true")? "true" : "false";
                    quad.exp2 = "_";
                } else if(quad.op == "or"){
                    quad.op = ":=";
                    quad.exp1 = (quad.exp1 == "true" || quad.exp2 == "true")? "true" : "false";
                    quad.exp2 = "_";
                } else if(quad.op == ":="){
                    if(const_map.count(quad.exp1)){
                        quad.res = const_map[quad.exp1];
                    } else {
                        to_print = false;
                    }
                }
                // 记录常数表
                const_map[quad.res] = quad.exp1;
                if(to_print)
                    outfile << "-- Union const: " << quad.res << " := " << quad.exp1 << endl;
            } else {
                // 否则, 该结果不再是常数, 须从map中删除
                if(const_map.count(quad.res)){
                    const_map.erase(quad.res);
                }
            }
        }

        // 第二步, 公共表达式节省
        vector<quater> del_quater;
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater& quad = bb.block_quaters[j];
            if(quad.res[0] == 't'){
                for(int t = j + 1; t < bb.block_quaters.size(); t++){
                    quater& quad_2 = bb.block_quaters[t];
                    // 找到公共表达式
                    if(quad_2.res[0] == 't' && quad_2.op == quad.op && quad_2.exp1 == quad.exp1 && quad_2.exp2 == quad.exp2){
                        del_quater.emplace_back(quad_2);
                        // 替换
                        for(int k = t + 1; k < bb.block_quaters.size(); k++){
                            quater& quad_3 = bb.block_quaters[k];
                            if(quad_3.exp1 == quad_2.res)
                                quad_3.exp1 = quad.res;
                            if(quad_3.exp2 == quad_2.res)
                                quad_3.exp2 = quad.res;                            
                            if(quad_3.res == quad_2.res)
                                quad_3.res = quad.res;                            
                        }
                    }
                }
            }
        }
        // 删除重复的公共表达式
        for(auto del:del_quater){
            for(int j = 0; j < bb.block_quaters.size(); j++){
                if(del == bb.block_quaters[j]){
                    bb.block_quaters.erase(bb.block_quaters.begin()+j);
                    break;
                }
            }
            outfile << "-- Remove common expr: " << del.res << " " << del.exp1 << " " << 
            del.op << " " << del.exp2 << endl;
        }
        del_quater.clear();

        // 第三步, 删除无用赋值
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater& quad = bb.block_quaters[j];
            if(quad.res[0] == 't' && quad.op == ":="){
                bool use = false; 
                for(int t = j + 1; t < bb.block_quaters.size(); t++){
                    quater& quad_2 = bb.block_quaters[t];
                    if(quad_2.exp1 == quad.res || quad_2.exp2 == quad.res || quad_2.res == quad.res){
                        use = true;
                        break;
                    }
                }
                // 找到无用的赋值
                if(!use){
                    del_quater.emplace_back(quad);
                }
            }
        }
        // 删除无用的赋值
        for(auto del:del_quater){
            for(int j = 0; j < bb.block_quaters.size(); j++){
                if(del == bb.block_quaters[j]){
                    bb.block_quaters.erase(bb.block_quaters.begin()+j);
                    break;
                }
            }
            outfile << "-- Remove useless expr: " << del.res << " " << del.op << " " << 
            del.exp1 << endl;
        }
        del_quater.clear();

        // 第四步, 条件语句优化
        // for(int j = 0; j < bb.block_quaters.size(); j++){
        //     quater& quad = bb.block_quaters[j];

        //     // 条件永真, 删除if语句的基本块
        //     if(quad.op == "if" && quad.exp1 == "true"){
        //         // 删除当前基本块内的所有四元式
        //         bb.block_quaters.clear();

        //         // 让上一个基本块指向真值的基本块
        //         bbs[i-1]._goto_true = bb._goto_true; 

        //         // 删除真值基本块中ife的四元式
        //         int _true_idx = bb._goto_true;
        //         bbs[_true_idx].block_quaters.pop_back();

        //         // 将假值转向的基本块中的四元式合并到真值转向基本块中
        //         int _false_idx = bb._goto_false;
        //         bbs[_true_idx].block_quaters.insert(bbs[_true_idx].block_quaters.end(), 
        //                                             bbs[_false_idx].block_quaters.begin(),
        //                                             bbs[_false_idx].block_quaters.end());
        //         bbs[_false_idx].clear();

        //         // 修改真值基本块的转向
        //         bbs[_true_idx]._goto_true = bbs[_false_idx]._goto_true;

        //         outfile << "-- Remove if block: Block " << i << " and Block " << _false_idx << endl;
        //     } else if(quad.op == "if" && quad.exp1 == "false"){
        //         /*
        //             条件永假时, 逻辑相反, 由于demo中没有对应测例, 不再赘述.
        //         */
        //     }
        // }

        // 第五步, 循环优化
        if(bb.empty() || bb.in_while == false || bb.in_if == true){
            outfile << endl;
            continue;
        }
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater& quad = bb.block_quaters[j];
            // 找到循环内不变的赋值表达式
            if(quad.op == ":=" && quad.res[0] != 't' && is_constant(quad.exp1) && is_constant(quad.exp2)){
                del_quater.emplace_back(quad);
                
                // 定位到while语句前的基本块
                int _before_while = bb._goto_true-1;

                // 将不变的赋值表达式提出循环外
                int _last_idx = bbs[_before_while].block_quaters.size()-1;
                string _last_op = bbs[_before_while].block_quaters[_last_idx].op;
                if( _last_op == "wh" || _last_op == "if"){
                    bbs[_before_while].block_quaters.emplace_back(bbs[_before_while].block_quaters[_last_idx]);
                    bbs[_before_while].block_quaters[_last_idx] = quad;
                } else {
                    bbs[_before_while].block_quaters.emplace_back(quad);
                }
                
                // 打印
                outfile << "-- Move out of cycle: " << quad.res << " " << quad.op << " " << quad.exp1 << 
                " into Block " << _before_while << endl;
            }
        }
        // 删除当前块内不变的赋值表达式
        for(auto del:del_quater){
            for(int j = 0; j < bb.block_quaters.size(); j++){
                if(del == bb.block_quaters[j]){
                    bb.block_quaters.erase(bb.block_quaters.begin()+j);
                    break;
                }
            }
        }
        del_quater.clear();

        // 优化完成
        outfile << endl;
    }
    outfile << str << "Optimized Basic Blocks:" << str << endl;
    print_bbs();
    outfile << str << "   End of Optimizer:   " << str << endl;

}

// 生成最终结果
void Optimizer::generate_results(){
    for(int i = 0; i < bbs.size(); i++){
        basic_block bb = bbs[i];
        // 经过优化后的基本块可能是空的, 不打印
        if(bb.empty()){
            continue;
        }
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater quad = bb.block_quaters[j];
            output_quaters.emplace_back(quad);              // 记录结果
        }
    }
    // 控制台输出
    for(int i = 0; i < output_quaters.size(); i++){
        cout << "(" << i+1 << ") " << output_quaters[i].get_string() << endl;
    }
    cout << endl;
}


// 打印基本块
void Optimizer::print_bbs(){
    for(int i = 0; i < bbs.size(); i++){
        basic_block bb = bbs[i];
        // 经过优化后的基本块可能是空的, 不打印
        if(bb.empty()){
            continue;
        }
        outfile << "In Basic Block " << i << ":"<< endl;
        for(int j = 0; j < bb.block_quaters.size(); j++){
            quater quad = bb.block_quaters[j];
            outfile << "(" << j+1 << ") " << quad.op << " " 
                    << quad.exp1 << " " << quad.exp2
                    << " " << quad.res << endl;
        }
        if(bb._goto_true == -1){
             outfile << "#END" << endl;
        } else if(bb._goto_false == -1){
            outfile << "goto: Block " << bb._goto_true << endl;
        } else {
            outfile << "if true goto: Block " << bb._goto_true << endl; 
            outfile << "if false goto: Block " << bb._goto_false << endl; 
        }
        outfile << endl;
    }
}

// 返回优化后的中间代码
vector<quater> Optimizer::get_result(){
    return output_quaters;
}

#endif