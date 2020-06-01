//
// Created by 95792 on 2020/6/1.
//

#ifndef COMPILER_CPP_VARELEMENT_H
#define COMPILER_CPP_VARELEMENT_H

#include "common.h"
#include <ext/hash_map>
#include "Token.h"
using namespace __gnu_cxx;

/****符号表内——变量/常量元素*****/
class VarElement {
private:
    /***变量元素的属性值****/
    Tag type;//变量类型
    string name;//变量名称
    //extern暂不考虑
//    //是否是显式变量
//    bool isExtern;
    bool isArray;//是否为数组变量
    int array_length;//数组长度
    bool isConstant;//是否为常量
    bool isPointer;//是否为指针变量
    bool isLeft;//是否是左值的

    //初值数据
    VarElement* initData;

    //变量是否有初始值
    bool initialed;
    //int char 和 string 类型变量的value
    int intValue;
    char charValue;
    string stringValue;
    //指针变量的处理
    VarElement* pointerValue;

    //变量的作用域路径
    vector<int> scopePath;

    /***代码生成和栈帧分配时需要用到的变量****/
    int size;//变量大小
    int offset;//局部变量的偏移 全局变量的偏移为0

public:
    //(指针)变量的创建
    VarElement(vector<int>& sp, Tag t, string n, bool isPtr);
    //(数组)变量的创建
    VarElement(vector<int>& sp, Tag t, string n, int len);
    //常量对象的创建
    VarElement(Token* token);

    ~VarElement();

    //获取变量名字
    string getVarName();//获取变量名字
    vector<int>& getScopePath();//获取变量作用域
    string getStrConstantValue();//获取字符常量的值
    //设置局部变量偏移
    void setOffset(int off);
};


#endif //COMPILER_CPP_VARELEMENT_H
