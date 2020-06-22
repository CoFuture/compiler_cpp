//
// Created by 95792 on 2020/6/1.
//

#ifndef COMPILER_CPP_FUNELEMENT_H
#define COMPILER_CPP_FUNELEMENT_H
#include "common.h"
#include "VarElement.h"
#include "IntermediateCode.h"

/****符号表内——函数元素*****/
class FunElement {
    //暂时不考虑extern
    //是否是函数声明
    bool isDeclare;
    //返回类型和函数名字
    Tag returnType;
    string name;
    //参数变量表
    vector<VarElement*> parameterList;

    /***代码生成和栈帧分配时需要用到的变量****/
    int maxEspDepth;
    int currentEspPosition;
    //作用域栈指针位置
    vector <int> scopeEsp;
    //函数的返回中间指令
    InterInstruction* returnPoint;

    /****对于函数体中的代码进行中间代码生成并且保存****/
    IntermediateCode interIC;
public:
    //构造函数 函数名字 返回值类型 参数列表
    FunElement(string n, Tag t, vector<VarElement*> &l);
    ~FunElement();
    //获取函数的相关信息
    Tag getReturnType();
    //判断函数返回值是否是基本类型
    bool isBasicReturnType();

    //函数内部作用域管理，栈帧计算
    void funScopeEnter();
    void funScopeExit();
    //函数匹配
    bool funMatched(FunElement* f);//声明，定义函数匹配
    bool parameterMatched(vector<VarElement*> &p);//参数列表匹配
    //设置or访问私有变量函数
    void setIsDeclare(bool isDec);
    bool getIsDeclare();
    string getName();
    void copyParameterList(vector<VarElement*> p);
    vector<VarElement*> getParameterList();

    //设置or获取函数的返回点
    void setFunReturnPoint(InterInstruction* point);
    InterInstruction* getFunReturnPoint();

    //添加中间代码
    void addInterInstruction(InterInstruction* i);
};


#endif //COMPILER_CPP_FUNELEMENT_H
