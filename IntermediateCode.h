//
// Created by 95792 on 2020/6/1.
//

#ifndef COMPILER_CPP_INTERMEDIATECODE_H
#define COMPILER_CPP_INTERMEDIATECODE_H

#include "common.h"
#include "SymbolTable.h"

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
 * function()       OP_FUN, -, function, -;
 * x = function()   OP_FUN_CALL, x, function, -;
 * return           OP_RETURN, -, -, -;
 * return value     OP_RETURN_VALUE, -, value, -;
 * **/


// 这里是定义的中间指令，四元式形式
class InterInstruction{
    Operator op;  //四元式1——操作符
    VarElement* result; //四元式2——操作结果
    VarElement* arg1;  //四元式3——运算数1
    VarElement* arg2;  //四元式4——运算数2
    string label_name;  //标签名称，用于标签四元式
    FunElement* function;   //函数指针，用于函数调用
    InterInstruction* jump_target;  //中间指令指针，用于跳转

    void initial();
public:
    /*定义构造方法*/
    //无参构造方法，课用于生成唯一的label名称
    InterInstruction();
    //函数入口和出口，函数调用，赋值调用
    InterInstruction(Operator o, FunElement* f, VarElement* v = nullptr);
    //数学运算
    InterInstruction(Operator o, VarElement* r, VarElement* v1, VarElement* v2 = nullptr);
    //跳转指令
    InterInstruction(Operator o, InterInstruction* t, VarElement* c = nullptr);
    //函数参数传入指令和NOP指令
    InterInstruction(Operator o, VarElement* arg = nullptr);
    //
    ~InterInstruction();

};

// 这里是中间代码，是中间指令的集合
class IntermediateCode {
    vector<InterInstruction*> inter_code;
public:
    ~IntermediateCode();
    //添加四元式指令
    void addInstruction(InterInstruction* i);
    //获取中间代码
    vector<InterInstruction*>& getIntermediateCode();
};


#endif //COMPILER_CPP_INTERMEDIATECODE_H
