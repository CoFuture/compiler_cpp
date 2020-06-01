//
// Created by 95792 on 2020/6/1.
//

#include "FunElement.h"
/****函数元素*****/
FunElement::FunElement(string n, Tag t, vector<VarElement *> &l) {
    //默认处于声明状态
    isDeclare = true;
    name = n;
    returnType = t;
    parameterList = l;
    currentEspPosition = 0;
    maxEspDepth = 0;
    //函数局部变量的栈帧偏移初始化，为返回地址和栈顶开辟位置，所以从offset从8开始分配
    for (int i = 0, tempOff = 8; i < parameterList.size(); i++, tempOff += 4) {
        parameterList[i]->setOffset(tempOff);
    }
}

FunElement::~FunElement() = default;

//进入函数局部作用域
void FunElement::funScopeEnter() {
    scopeEsp.push_back(0);
}

//退出函数局部作用域
void FunElement::funScopeExit() {
    if (currentEspPosition > maxEspDepth)
        maxEspDepth = currentEspPosition;
    //当前栈指针位置减去作用域末元素
    currentEspPosition = currentEspPosition - scopeEsp.back();
    scopeEsp.pop_back();
}

//函数声明与定义匹配
bool FunElement::funMatched(FunElement *f) {
    //名字匹配
    if (name!= f->name)
        return false;
    //返回值检查
    if (returnType != f->returnType)
        return false;
    //参数列表匹配
    if (parameterList.size() != f->parameterList.size())
        return false;
    int paraListLength = parameterList.size();
    for (int i = 0; i < paraListLength; ++i) {
        //todo 类型兼容性检查
    }
    //经过检查无问题，返回正确值
    return true;
}

//函数参数匹配
bool FunElement::parameterMatched(vector<VarElement *> &p) {
    //参数列表大小检查
    if (parameterList.size() != p.size())
        return false;
    //逐项检查
    int length =parameterList.size();
    for (int i = 0; i < length; ++i) {
        //todo 类型检查
    }
    return true;
}

bool FunElement::getIsDeclare() {
    return isDeclare;
}

void FunElement::setIsDeclare(bool isDec) {
    isDeclare = isDec;
}

string FunElement::getName() {
    return name;
}

void FunElement::copyParameterList(vector<VarElement *> p) {
    parameterList = p;
}

vector<VarElement *> FunElement::getParameterList() {
    return parameterList;
}