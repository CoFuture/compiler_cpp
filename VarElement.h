//
// Created by 95792 on 2020/6/1.
//

#ifndef COMPILER_CPP_VARELEMENT_H
#define COMPILER_CPP_VARELEMENT_H

#include "common.h"
#include <ext/hash_map>
#include "Token.h"
using namespace __gnu_cxx;

/***创建变量对象的三种情况***/
//1、声明的变量 int x = 5;
//2、常量的定义
//3、表达式运算的临时结果变量

/****符号表内——变量/常量元素*****/
class VarElement {
private:
    /***变量元素的属性值****/
    Tag type;//变量类型
    string name;//变量名称
    bool isArray;//是否为数组变量
    int array_length;//数组长度
    bool isConstant;//是否为常量
    bool isPointer;//是否为指针变量
    bool isLeft;//是否是左值的

    /**变量初始化的处理 例如: int x = 10 则将10保存到initData中**/
    //变量是否有初始值
    bool initialed;
    //初值数据
    VarElement* initData;

    //int char 和 string 类型变量的value
    int intValue;
    char charValue;
    string stringValue;
    //指针变量的处理 举例：*p = x, x为当前的VarElement，则将p保存到PointerValue中
    VarElement* pointerValue;

    //变量的作用域路径
    vector<int> scopePath;

    /***代码生成和栈帧分配时需要用到的变量****/
    int size;//变量大小
    int offset;//局部变量的偏移 全局变量的偏移为0
    void create_init();     //变量初始化
    void setType(Tag t);    //设定变量类型
    void setName(string n); //指定变量名字
    void generateName();    //生成随机分配的名字
public:
    /**构造方法定义**/
    //无参的构造方法，用来创建void空变量
    VarElement();
    //todo 构造方法改写
    //todo 布尔类型变量 true false变量的创建
//    VarElement(vector<int>& sp, Tag t, bool b = false);
    //特殊变量 0 1 4变量创建
    VarElement(int num);
    //(指针)变量的创建，需要的input:变量作用域、变量类型、变量名称、
    VarElement(vector<int>& sp, Tag t, string n, bool isPtr, VarElement* init);
    //(数组)变量的创建
    VarElement(vector<int>& sp, Tag t, string n, int len);
    //常量对象的创建
    VarElement(Token* token);
    //拷贝一个临时变量
    VarElement(vector<int>& sp, VarElement* var);
    //临时变量的创建
    VarElement(vector<int>& sp, Tag t, bool isPtr);
    //析构方法
    ~VarElement();

    /**获取变量元素的相关属性**/
    string getVarName();            //获取变量名字
    Tag getVarType();               //获取变量类型
    vector<int>& getScopePath();    //获取变量作用域
    string getStrConstantValue();   //获取字符常量的值
    VarElement* getPointer();       //获取指向当前变量的指针
    //todo isBasicType改写
    bool isBasicType();             //判断变量是否是基本类型 int or char（非数组和指针）
    bool getIsPointer();            //是否是指针变量
    bool getIsPointed();            //此变量被引用，存在指向该变量的指针（也可也理解为该变量是指针运算结果）
    bool getIsArray();              //是否是数组
    bool getIsConstant();           //是否是常量
    bool getIsLeft();               //是否是左值的
    bool getIsVoid();               //是否是void变量
    //获取变量步长——用于处理指针变量 char*为1 int*为4，基本类型均为1，视为++
    static VarElement* getStep(VarElement* var);

    /**设置变量元素的相关属性**/
    //设置局部变量偏移
    void setOffset(int off);
    void setPointerValue(VarElement* var);   //设置为指针变量,其中var为指向当前变量的指针
    void setLeft(bool flag);
};


#endif //COMPILER_CPP_VARELEMENT_H
