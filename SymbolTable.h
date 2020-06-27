//
// Created by 95792 on 2020/4/21.
//

#ifndef COMPILER_CPP_SYMBOL_TABLE_H
#define COMPILER_CPP_SYMBOL_TABLE_H

#include "common.h"
#include "Token.h"
#include "VarElement.h"
#include "FunElement.h"
#include <ext/hash_map>
using namespace __gnu_cxx;

class GenerateCode;
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

    //用来标记当前符号表是否进入函数分析
    FunElement* currentFunction;

    //作用域的Id号
    int scopeId;
    //作用域路径记录
    vector<int> scopePath;
    //代码生成器
    GenerateCode* generateCode;

    //保存变量和函数的添加顺序
    vector<string> varList;
    vector<string> funList;

public:
    //一些特殊的变量，提前创建并由符号表管理
    static VarElement* var_void;    //void空变量
    static VarElement* num_zero;    //数字0 false
    static VarElement* num_one;     //数字1 true
    static VarElement* num_four;    //数字4 4字节对齐，int类型变量大小

    //构造和析构函数
    SymbolTable();
    ~SymbolTable();
    //设置符号表对应的代码生成器
    void setGenerateCoder(GenerateCode* g);
    /***符号表管理***/
    //变量的相关操作
    void addVariable(VarElement* var);          //添加变量
    void addString(VarElement* var);          //添加字符串常量
    VarElement* getVariable(string name, vector<int>& sp);       //获取变量,input:name,scopePath
    vector<VarElement*> getGlobalVariables();

    //函数的相关操作
    //decOrDef是标志位，表示函数是声明还是定义
    void defineFunction(FunElement* fun);       //函数定义
    void endDefineFunction();    //结束函数定义，配合函数定义进行作用域管理
    void declareFunction(FunElement* fun);      //函数声明
    FunElement* getFunction(string name, vector <VarElement*>& parameters);//根据函数名称和参数列表获取函数
    FunElement* getCurrentFunction();           //获取当前分析的函数

    //进入和退出作用域
    void scopeEnter();
    void scopeExit();
    //获取当前作用域路径 用于创建变量or函数
    vector<int>& getScopePath();

    void addInterInstruction(InterInstruction* i);

    //执行优化
    void optimize();

    //输出显示相关信息
    void showInformation();     //输出符号表信息
    void showInterCode();       //输出中间代码
    void showOptimizedCode();   //输出优化后的中间代码

};

#endif //COMPILER_CPP_SYMBOL_TABLE_H
