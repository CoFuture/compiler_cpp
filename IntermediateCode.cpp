//
// Created by 95792 on 2020/6/1.
//

#include "IntermediateCode.h"
#include "GenerateCode.h"

/*中间指令部分函数的具体实现*/
InterInstruction::InterInstruction() {
    this->op = OP_NOP;
    this->result = nullptr;
    this->arg1 = nullptr;
    this->arg2 = nullptr;
    this->function = nullptr;
    this->jump_target = nullptr;
    //todo genLabel功能单独抽出
    this->label_name = GenerateCode::genLabel();
}

//函数入口和出口，函数调用，赋值调用
InterInstruction::InterInstruction(Operator o, FunElement *f, VarElement *v) {
    this->op = o;
    this->result = v;
    this->arg1 = nullptr;
    this->arg2 = nullptr;
    this->function = f;
    this->jump_target = nullptr;
    this->label_name = "";
}

InterInstruction::InterInstruction(Operator o, VarElement *r, VarElement *v1, VarElement *v2) {
    this->op = o;
    this->result = r;
    this->arg1 = v1;
    this->arg2 = v2;
    this->function = nullptr;
    this->jump_target = nullptr;
    this->label_name = "";
}

InterInstruction::InterInstruction(Operator o, InterInstruction *t, VarElement *var1, VarElement* var2) {
    this->op = o;
    this->result = nullptr;
    this->arg1 = var1;
    this->arg2 = var2;
    this->function = nullptr;
    this->jump_target = t;
    this->label_name = "";
}

InterInstruction::InterInstruction(Operator o, VarElement *arg) {
    this->op = o;
    this->result = nullptr;
    this->arg1 = arg;
    this->arg2 = nullptr;
    this->function = nullptr;
    this->jump_target = nullptr;
    this->label_name = "";
}

void InterInstruction::tostring() {
    //todo 四元式输出
    if (!label_name.empty()){
        cout << label_name << endl;
        return;
    }

    switch (op) {
        case OP_NOP:
            break;
        case OP_LABEL:
            break;
        case OP_FUN_ENTRY:
            break;
        case OP_FUN_EXIT:
            break;
        case OP_DECLARE:
            break;
        case OP_ASSIGN:
            break;
        case OP_ADD:
            break;
        case OP_SUB:
            break;
        case OP_MUL:
            break;
        case OP_DIV:
            break;
        case OP_MOD:
            break;
        case OP_NEGATIVE:
            break;
        case OP_GREATER:
            break;
        case OP_GREATER_EQUAL:
            break;
        case OP_LESS:
            break;
        case OP_LESS_EQUAL:
            break;
        case OP_EQUAL:
            break;
        case OP_NOT_EQUAL:
            break;
        case OP_NOT:
            break;
        case OP_AND:
            break;
        case OP_OR:
            break;
        case OP_LEA:
            break;
        case OP_POINTER_SET:
            break;
        case OP_POINTER_GET:
            break;
        case OP_JUMP:
            break;
        case OP_JUMP_TRUE:
            break;
        case OP_JUMP_FALSE:
            break;
        case OP_ARG:
            break;
        case OP_FUN:
            break;
        case OP_FUN_CALL:
            break;
        case OP_RETURN:
            break;
        case OP_RETURN_VALUE:
            break;
        default:
            //默认为NOP指令
            break;
    }
}

InterInstruction::~InterInstruction() = default;


/***中间代码部分函数的具体实现***/
IntermediateCode::~IntermediateCode() {
    //清理intermediateCode
    for (int i = 0; i < inter_code.size(); ++i) {
        delete inter_code[i];
    }
}

//添加一条四元式指令
void IntermediateCode::addInstruction(InterInstruction *i) {
    inter_code.push_back(i);
}

vector<InterInstruction *> &IntermediateCode::getIntermediateCode() {
    return inter_code;
}

void IntermediateCode::showIntermediateCode() {
    for (int i = 0; i < inter_code.size(); ++i) {
        inter_code[i]->tostring();
    }
}
