//
// Created by 95792 on 2020/7/4.
//

#ifndef COMPILER_CPP_LIVEANALYSE_H
#define COMPILER_CPP_LIVEANALYSE_H

#include "common.h"
#include "Set.h"

class SymbolTable;
class DataFlowGraph;
class InterInstruction;
class VarElement;
class Block;

//变量活跃性分析
class LiveAnalyse {
    SymbolTable* table;
    DataFlowGraph* dataFlowGraph;
    list<InterInstruction*> optimizedCode;
    vector<VarElement*> varList;

    //定义集合，进行活跃变量分析
    Set U;//全集
    Set E;//空集
    Set G;//全局变量集

    //针对基本块的变量活跃性分析
    bool transmit(Block* block);

public:
    //构造函数定义
    LiveAnalyse(DataFlowGraph* graph, SymbolTable* t, vector<VarElement*>& paraVar);

    void startAnalyse();                            //进行活跃性分析
    void removeDeadCode(int stop = false);            //死代码消除
    Set& getE();                                    //返回空集
    vector<VarElement*> getCoVar(Set liveOut);             //根据提供的liveOut集合提取优化后的变量集合——冲突变量
};


#endif //COMPILER_CPP_LIVEANALYSE_H
