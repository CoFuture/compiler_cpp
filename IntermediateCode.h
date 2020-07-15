//
// Created by 95792 on 2020/6/1.
//

#ifndef COMPILER_CPP_INTERMEDIATECODE_H
#define COMPILER_CPP_INTERMEDIATECODE_H

#include "common.h"
#include "Set.h"
/** 四元式定义
 * 标签指令label        OP_LABEL, label, -, -;
 * 函数入口             OP_FUN_ENTRY, function, -, -;
 * 函数出口             OP_FUN_EXIT, function, -, -;
 * 变量函数声明          OP_DECLARE, name, -, -;
 * 赋值 x = y        OP_ASSIGN, x, y, -;
 * x = y + z        OP_ADD, x, y, z;
 * x = y - z        OP_SUB, x, y, z;
 * x = y * z        OP_MUL, x, y, z;
 * x = y / z        OP_DIV, x, y, z;
 * x = y mod z      OP_MOD, x, y, z;
 * x = -y           OP_NEGATIVE, x, y, -;
 * x = (y > z)      OP_GREATER, x, y, z;
 * x = (y >= z)     OP_GREATER_EQUAL, x, y, z;
 * x = (y < z)      OP_LESS, x, y, z;
 * x = (y <= z)     OP_LESS_EQUAL, x, y, z;
 * x = (y == z)     OP_EQUAL, x, y, z
 * x = (y != z)     OP_NOT_EQUAL, x, y, z;
 * x = !y           OP_NOT, x, y, -;
 * x = y && z       OP_AND, x, y, z;
 * x = y || z       OP_OR, x, y, z;
 * x = &y           OP_LEA, x, y, -;
 * *x = y           OP_POINTER_SET, x, y, -;
 * x = *y           OP_POINTER_GET, x, y, -;
 * goto label       OP_JUMP, label, -, -;
 * if(condition)goto label      OP_JUMP_TRUE, label, condition, -;
 * if(!condition)goto label     OP_JUMP_FALSE, label, condition, -;
 * if(var1 != var2)goto label     OP_JUMP_NOT_EQUAL, label, var1, var2;
 * function(args)   OP_ARG, -, arg, -;
 * function()       OP_FUN, -, function, -;
 * x = function()   OP_FUN_CALL, x, function, -;
 * return           OP_RETURN, -, -, -;
 * return value     OP_RETURN_VALUE, -, value, -;
 * **/
class VarElement;
class FunElement;
class Block;


// 这里是定义的中间指令，四元式形式
class InterInstruction{
    Operator op;  //四元式1——操作符
    VarElement* result; //四元式2——操作结果
    VarElement* arg1;  //四元式3——运算数1
    VarElement* arg2;  //四元式4——运算数2
    string label_name;  //标签名称，用于标签四元式
    FunElement* function;   //函数指针，用于函数调用
    InterInstruction* jump_target;  //中间指令指针，用于跳转

    //优化相关
    bool first;
public:
    /*定义构造方法*/
    //无参构造方法，课用于生成唯一的label名称
    InterInstruction();
    //函数入口和出口，函数调用，赋值调用
    InterInstruction(Operator o, FunElement* f, VarElement* v = nullptr);
    //数学运算
    InterInstruction(Operator o, VarElement* r, VarElement* v1, VarElement* v2 = nullptr);
    //跳转指令
    InterInstruction(Operator o, InterInstruction* t, VarElement* var1= nullptr, VarElement* var2 = nullptr);
    //函数参数传入指令和NOP指令
    InterInstruction(Operator o, VarElement* arg = nullptr);
    //
    ~InterInstruction();

    //获取中间代码的相关信息
    Operator getOperator();             //获取操作指令
    VarElement* getResult();            //获取result
    VarElement* getArg1();              //获取arg1
    VarElement* getArg2();              //获取arg2
    FunElement* getFunction();          //获取Function
    InterInstruction* getJumpTarget();  //获取跳转目标指令
    string getLabel();                  //获取标签label
    void setArg1(VarElement* var);     //设置第一个参数

    /***优化的相关element和function***/
    Block* block;               //标价指令所在的基本块
    vector<double> inValues;    //常量传播的in集合
    vector<double> outValues;   //常量传播的out集合

    //变量活跃性分析
    LiveInfo liveInfo;          //活跃变量数据流信息
    bool isDead;                //标识指令是否是死代码

    void setFirst();            //将指令标记为首指令

//    bool isAlgebraTwoOpt();     //是否是算数二元操作数
//    bool isAlgebraOneOpt();     //是否是算数一元操作数
    bool isFirst();             //是否是首指令
    bool isJump();              //是否无条件跳转
    bool isJumpCondition();     //是否是有条件跳转
    bool isLabel();             //是否是label
    bool isDeclare();           //是否是变量声明
    bool isExpression();        //是否是运算表达式

    //不确定的运算包括指针的赋值，函数的调用
    bool isUncertain();         //运算结果是否确定

    //指令替换
    void replace(Operator o, VarElement* r, VarElement* var1, VarElement* var2 = nullptr);
    void replace(Operator o, InterInstruction* t, VarElement* var1 = nullptr, VarElement* var2 = nullptr);
    void toSimpleCall();        //将有返回值的函数调用转换为简单的函数调用
    /***优化的相关element和function***/

    //显示四元式指令
    void tostring();

    /*****生成目标代码 NASM intel x86*****/
    void loadVar(string reg, VarElement* var, FILE* fp);      //将变量加载到寄存器
    void leaVar(string reg, VarElement* var, FILE* fp);       //加载变量地址
    void storeVar(string reg, VarElement* var, FILE* fp);     //将变量存储到内存
    void initVar(VarElement* var, FILE* fp);                  //局部变量的初始化
    void toX86();                                   //生成x86代码
};

// 这里是中间代码，是中间指令的集合
class IntermediateCode {
    vector<InterInstruction*> inter_code;
public:
    ~IntermediateCode();
    //添加四元式指令
    void addInstruction(InterInstruction* i);

    //优化相关操作
    void markFirst();                                   //标记指令集合中的首指令
    vector<InterInstruction*>& getIntermediateCode();   //获取中间代码

    //输出中间代码
    void showIntermediateCode();
};


#endif //COMPILER_CPP_INTERMEDIATECODE_H
