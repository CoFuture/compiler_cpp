//
// Created by 95792 on 2020/6/28.
//

#ifndef COMPILER_CPP_DATAFLOWGRAPH_H
#define COMPILER_CPP_DATAFLOWGRAPH_H

#include "common.h"
#include "Set.h"

//需要用到的类的提前声明
class InterInstruction;
class IntermediateCode;

//基本块
class Block{
public:
    list<InterInstruction*> instructions;   //基本块中的指令序列
    list<Block*> precursor;                 //前驱
    list<Block*> successor;                 //后继
    bool visited;
    bool reachable;                         //块可达标记,不可达块不作为数据流处理对象


    //数据流分析
    vector<double> inValues;                //常量传播的输入值集合
    vector<double> outValues;               //常量传播的输出值集合
    LiveInfo liveInfo;                      //活跃变量数据流信息

    //构造方法
    Block(vector<InterInstruction*>& codes);

    //输出显示
    void showInformation();
};

//数据流图
class DataFlowGraph {

    void createBlocks();    //创建基本块
    void linkBlocks();      //链接基本块

    void resetVisit();                  //重置block访问标志
    bool blockReachable(Block* block);  //判断block是否可达
    void releaseBlock(Block* block);    //释放block，删除后继信息
public:
    //定义构造方法
    DataFlowGraph(IntermediateCode& code);
    ~DataFlowGraph();

    vector<InterInstruction*> interCodeList;    //中间代码
    vector<Block*> blocks;                      //基本块
    vector<InterInstruction*> optimizedCode;    //优化生成的中间代码块

    //块不可达，删除块间联系
    void deleteLink(Block* begin, Block* end);

    //将数据流图转换为中间代码
    void toInterCode(list<InterInstruction*>& optimized);

    //输出显示
    void showInformation();
};


#endif //COMPILER_CPP_DATAFLOWGRAPH_H
