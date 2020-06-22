//
// Created by 95792 on 2020/6/1.
//
#pragma once
#ifndef COMPILER_CPP_GENERATECODE_H
#define COMPILER_CPP_GENERATECODE_H

#include "FunElement.h"
#include "VarElement.h"
#include "IntermediateCode.h"


class SymbolTable;
/*这里为生成中间代码的类*/

class GenerateCode {
    SymbolTable& symbolTable;
    //label的标记号
    static int label_num;
    //定义指令入口和出口管理的vector变量，用于实现break和continue的作用域管理以及代码生成
    vector<InterInstruction*> entry_stack;
    vector<InterInstruction*> exit_stack;
public:
    //构造方法
    GenerateCode(SymbolTable& table);
    //类型检查
    bool typeCheck(VarElement* var1, VarElement* var2);
    //进行代码入口和出口的管理
    void pushEntryAndExit(InterInstruction* entry, InterInstruction* exit);
    void popEntryAndExit();
    //生成唯一的label名称
    static string genLabel();
    //生成函数入口和函数出口 参数为函数元素
    /**
     *  C代码形式：
     *  int function(){
     *      return 1;
     *      return 0;
     *  }
     *  四元式形式形式：
     *  function:
     *      OP_ENTRY fun
     *      OP_RETURN_VALUE 1 goto return_point
     *      OP_RETURN_VALUE 0 goto return_point
     *  return_point:
     *      OP_EXIT function
     * **/
    void genFunctionEntry(FunElement* f);
    void genFunctionExit(FunElement* f);
    //生成return语句 参数为返回值变量
    void genReturn(VarElement* var);
    //生成赋值语句
    //处理指针运算作为右值的赋值
    VarElement* genPointerAssign(VarElement* var);
    VarElement* genAssign(VarElement* l_var, VarElement* r_var);
    //双目运算符
    VarElement* genTwoOperation(Operator o, VarElement* var1, VarElement* var2);
    VarElement* genAdd(VarElement* var1, VarElement* var2);
    VarElement* genSub(VarElement* var1, VarElement* var2);
    VarElement* genMul(VarElement* var1, VarElement* var2);
    VarElement* genDiv(VarElement* var1, VarElement* var2);
    VarElement* genMod(VarElement* var1, VarElement* var2);
    VarElement* genAnd(VarElement* var1, VarElement* var2);
    VarElement* genOr(VarElement* var1, VarElement* var2);
    VarElement* genEqual(VarElement* var1, VarElement* var2);
    VarElement* genNotEqual(VarElement* var1, VarElement* var2);
    VarElement* genGreater(VarElement* var1, VarElement* var2);
    VarElement* genGreaterEqual(VarElement* var1, VarElement* var2);
    VarElement* genLess(VarElement* var1, VarElement* var2);
    VarElement* genLessEqual(VarElement* var1, VarElement* var2);
    //单目运算符 ++ -- ！ & * -
    VarElement* genOneOperation(Operator o, VarElement* var);
    VarElement* genInc(VarElement* var);
    VarElement* genDec(VarElement* var);
    VarElement* genNot(VarElement* var);
    VarElement* genLea(VarElement* var);
    VarElement* genPointer(VarElement* var);
    VarElement* genNegative(VarElement* var);
    //后缀单目运算符 ++ --
    VarElement* genOneOperationRight(Operator o, VarElement* var);
    VarElement* genIncRight(VarElement* var);
    VarElement* genDecRight(VarElement* var);
    //数组索引运算 array[5] 返回值为变量元素
    VarElement* genArrayIndex(VarElement* array, VarElement* index);
    //函数调用
    void genArgs(VarElement* arg);
    VarElement* genFunctionCall(FunElement* fun, vector<VarElement*>& args);
    /*复合语句的翻译*/
    //产生if头部，输入参数为判断条件condition，跳转else目标的中间指令指针的引用
    void genIfHead(VarElement* condition, InterInstruction*& t_else);
    void genIfTail(InterInstruction*& t_else);
    void genElseHead(InterInstruction*& t_else, InterInstruction*& t_exit);
    void genElseTail(InterInstruction*& t_exit);
    //switch语句的生成
    void genSwitchHead(InterInstruction*& t_exit);
    void genCaseHead(VarElement* condition, VarElement* label, InterInstruction*& t_case_exit);
    void genCaseTail(InterInstruction*& t_case_exit);
    void genSwitchTail(InterInstruction*& t_exit);
    //while语句的生成 while语句首部 循环条件condition while语句结束
    void genWhileHead(InterInstruction*& t_while, InterInstruction*& t_while_exit);
    void genWhileCondition(VarElement* condition, InterInstruction*& t_while_exit);
    void genWhileTail(InterInstruction*& t_while, InterInstruction*& t_while_exit);
    //do-while语句
    void genDoWhileHead(InterInstruction*& t_do, InterInstruction*& t_exit);
    void genDoWhileTail(VarElement* condition, InterInstruction*& t_do, InterInstruction*& t_exit);
    //for语句的生成
    void genForHead(InterInstruction*& t_for, InterInstruction*& t_exit);
    void genForConditionHead(VarElement* condition, InterInstruction*& t_step, InterInstruction*& t_for_block, InterInstruction*& t_exit);
    void genForConditionTail(InterInstruction*& t_for, InterInstruction*& t_for_block);
    void genForTail(InterInstruction*& t_step, InterInstruction*& t_exit);
    //break和continue语句
    void genContinue();
    void genBreak();
};


#endif //COMPILER_CPP_GENERATECODE_H
