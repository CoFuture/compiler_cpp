//
// Created by 95792 on 2020/7/8.
//

#ifndef COMPILER_CPP_REGISTERASSIGN_H
#define COMPILER_CPP_REGISTERASSIGN_H

#include "common.h"
#include "Set.h"

//可使用的寄存器个数6个 eax ebx ecx edx esi edi
#define REGISTER_COUNT 6

class VarElement;
class FunElement;
class InterInstruction;
class LiveAnalyse;
/******构造冲突图，图着色法进行寄存器分配******/

//定义冲突图节点
struct Node{
    VarElement* var;        //节点变量
    int degree;             //节点的度
    int nodeColor;          //节点着色
    Set excludedColor;      //节点排斥颜色集合
    vector<Node*> links;    //关联边

    //基本操作函数
    Node(VarElement* v, Set& e);        //创建变量v的节点并初始化
    void addLink(Node* node);           //将当前节点添加到节点node的冲突边
    void addExcludedColor(int color);   //添加排斥颜色
    void paint(Set& colorBox);          //根据排斥颜色集合选择一个有效颜色
};

//冲突图
class ConflictGraph{
    struct nodeCompare{
        //定义操作，判断左节点度数小于右节点度数
        bool operator()(Node* left, Node* right){
            return left->degree <= right->degree;
        }
    };
    vector<Node*> nodes;    //节点序列
    //分析必须的对象
    list<InterInstruction*> optimizedCode;  //优化了的中间代码
    vector<VarElement*> varList;            //缓存需要分配的变量列表
    LiveAnalyse* liveAnalyse;               //变量活跃性分析结果

    //数据流初始集合
    Set U;
    Set E;
    FunElement* function;   //当前函数对象

    Node* pickNode();       //选择度最大的未处理节点，利用最大堆根据节点度堆排序
public:
    ConflictGraph(list<InterInstruction*>& optCode, vector<VarElement*>& paraVar, LiveAnalyse* live, FunElement* f);
    void registerAssign();  //寄存器分配
};


#endif //COMPILER_CPP_REGISTERASSIGN_H
