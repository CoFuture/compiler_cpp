//
// Created by 95792 on 2020/6/28.
//

#ifndef COMPILER_CPP_CONSTPROPAGATION_H
#define COMPILER_CPP_CONSTPROPAGATION_H

#include "common.h"

/***常量传播分析***/
//实现1、常量合并
//实现2、代数化简
//实现3、不可达代码消除

//定义半格的最大值和最小值
#define UNDEFINED 0.5
#define UNCERTAIN -0.5

class SymbolTable;
class DataFlowGraph;
class Block;
class VarElement;
class InterInstruction;

class ConstPropagation {
    SymbolTable* table;             //符号表
    DataFlowGraph* dataFlowGraph;   //数据流图

    vector<VarElement*> vars;       //变量集合
    vector<VarElement*> globalVars; //全局变量集合
    vector<double> boundValues;     //边界集合 Entry.out
    vector<double> initValues;      //初值集合 Block.out

    //运算函数
    double join(double left, double right); //元素交汇运算
    void join(Block* block);                //基本块的交汇运算
    //传递函数
    void transmit(InterInstruction* instruction, vector<double>& in, vector<double>& out);
    bool transmit(Block* block);

    void startAnalyse();            //根据数据流图的常量传播分析
    void simplifyAlgebra();         //代数运算化简
    void optimizeConditionJump();   //不可达代码消除，优化条件跳转

public:
    //构造方法
    ConstPropagation(SymbolTable* t, DataFlowGraph* dfg, vector<VarElement*>& paraVars);
    //执行常量传播优化
    void optimize();
};


#endif //COMPILER_CPP_CONSTPROPAGATION_H
