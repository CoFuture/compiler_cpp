//
// Created by 95792 on 2020/4/21.
//

#ifndef COMPILER_CPP_SYMBOL_TABLE_H
#define COMPILER_CPP_SYMBOL_TABLE_H

#include "common.h"
#include "Token.h"
#include <ext/hash_map>
using namespace __gnu_cxx;

class VarElement;
class FunElement;
/**这里是符号表类**/

class SymbolTable {
private:
    //hash函数
    struct hash_string{
        size_t operator()(const string& str) const {
            return __stl_hash_string(str.c_str());
        }
    };
    //变量表(包含数组，字符常量)
    hash_map<string, vector<VarElement*>* ,hash_string> variableTable;
    //字符串常量表
    hash_map<string, VarElement*, hash_string> constantTable;
    //函数表
    hash_map<string, FunElement*, hash_string> functionTable;

    //作用域和函数处理相关信息
    FunElement* currentFunction;
    //作用域的Id号
    int scopeId;
    //作用域路径记录
    vector <int> scopePath;

public:
    //构造和析构函数
    SymbolTable();
    ~SymbolTable();
    /***符号表管理***/
    //变量的相关操作
    void addVariable(VarElement* var);//添加变量
    void addConstant(VarElement* var);//添加字符串常量
    VarElement* getVariable(string name);//获取变量

    //函数的相关操作
    //decOrDef是标志位，表示函数是声明还是定义
    void defineFunction(FunElement* fun);//函数定义
    void endDefineFunction(FunElement* fun);//结束函数定义，配合函数定义进行作用域管理
    void declareFunction(FunElement* fun);//函数声明
    FunElement* getFunction(string name, vector <VarElement*>& parameters);//根据函数名称和参数列表获取函数

    //进入和退出作用域
    void scopeEnter();
    void scopeExit();
    //获取当前作用域路径 用于创建变量or函数
    vector<int>& getScopePath();
};

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

    //TODO 目标代码生成
public:
    //构造函数 函数名字 返回值类型 参数列表
    FunElement(string n, Tag t, vector<VarElement*> &l);
    ~FunElement();
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
};

#endif //COMPILER_CPP_SYMBOL_TABLE_H
