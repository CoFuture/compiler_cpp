//
// Created by 95792 on 2020/6/1.
//

#include "GenerateCode.h"

GenerateCode::GenerateCode(SymbolTable &table):symbolTable(table) {
    //入口和出口的初始化
    entry_stack.push_back(nullptr);
    exit_stack.push_back(nullptr);
}

string GenerateCode::genLabel() {
    return std::string();
}

//进行两个变量元素的类型检查
bool GenerateCode::typeCheck(VarElement *var1, VarElement *var2) {
    //两个变量存在空值则返回false
    if (var1 == nullptr || var2 == nullptr)
        return false;
    //同为基本类型，或者不是基本类型但类型相同则为true
    if (var1->isBasicType() && var2->isBasicType())
        return true;
    else{
        if (var1->getVarType() == var2->getVarType())
            return true;
    }
    return false;
}

//添加入口和出口
void GenerateCode::pushEntryAndExit(InterInstruction *entry, InterInstruction *exit) {
    entry_stack.push_back(entry);
    exit_stack.push_back(exit);
}

//删除入口和出口
void GenerateCode::popEntryAndExit() {
    entry_stack.pop_back();
    exit_stack.pop_back();
}

void GenerateCode::genFunctionEntry(FunElement *f) {

}

void GenerateCode::genFunctionExit(FunElement *f) {

}

void GenerateCode::genReturn(VarElement *var) {

}


VarElement *GenerateCode::genAssign(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genTwoOperation(Operator o, VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genAdd(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genSub(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genMul(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genDiv(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genAnd(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genMod(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genOr(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genEqual(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genNotEqual(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genGreater(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genGreaterEqual(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genLess(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genLessEqual(VarElement *var1, VarElement *var2) {
    return nullptr;
}

VarElement *GenerateCode::genOneOperation(Operator o, VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genInc(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genDec(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genNot(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genPointer(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genLea(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genNegative(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genOneOperationRight(Operator o, VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genIncRight(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genDecRight(VarElement *var) {
    return nullptr;
}

VarElement *GenerateCode::genArrayIndex(VarElement *array, VarElement *index) {
    return nullptr;
}

void GenerateCode::genArgs(VarElement *arg) {

}

VarElement *GenerateCode::genFunctionCall(FunElement *fun, vector<VarElement *> &args) {
    return nullptr;
}

void GenerateCode::genIfHead(VarElement *condition, InterInstruction *&t_else) {

}

void GenerateCode::genIfTail(InterInstruction *&t_else) {

}

void GenerateCode::genElseHead(InterInstruction *t_else, InterInstruction *&t_exit) {

}

void GenerateCode::genElseTail(InterInstruction *&t_exit) {

}

void GenerateCode::genSwitchHead(InterInstruction *&t_exit) {

}

void GenerateCode::genSwitchTail(InterInstruction *t_exit) {

}

void GenerateCode::genCaseHead(VarElement *condition, VarElement *label, InterInstruction *&t_case_exit) {

}

void GenerateCode::genCaseTail(InterInstruction *t_case_exit) {

}

void GenerateCode::genWhileHead(InterInstruction *&t_while, InterInstruction *&t_while_exit) {

}

void GenerateCode::genWhileCondition(VarElement *condition, InterInstruction*& t_while_exit) {

}

void GenerateCode::genWhileTail(InterInstruction *&t_while, InterInstruction *&t_while_exit) {

}

void GenerateCode::genDoWhileHead(InterInstruction *&t_do, InterInstruction *&t_exit) {

}

void GenerateCode::genDoWhileTail(VarElement *condition, InterInstruction *t_do, InterInstruction *t_exit) {

}

void GenerateCode::genForHead(InterInstruction *&t_for, InterInstruction *&t_exit) {

}

void GenerateCode::genForConditionHead(VarElement *condition, InterInstruction *&t_step, InterInstruction *&t_for_block,
                                       InterInstruction *t_exit) {

}

void GenerateCode::genForConditionTail(InterInstruction *t_for, InterInstruction *t_for_block) {

}

void GenerateCode::genForTail(InterInstruction *&t_step, InterInstruction *t_for) {

}

void GenerateCode::genContinue() {

}

void GenerateCode::genBreak() {

}













