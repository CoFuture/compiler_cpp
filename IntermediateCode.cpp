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
            cout << "OP_NOP" << endl;
            break;
        case OP_LABEL:
            break;
        case OP_FUN_ENTRY:
            cout << "OP_FUN_ENTRY " << endl;
            break;
        case OP_FUN_EXIT:
            cout << "OP_FUN_EXIT " << endl;
            break;
        case OP_DECLARE:
            cout << "OP_DECLARE ";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_ASSIGN:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_ADD:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " + ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_SUB:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " - ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_MUL:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " * ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_DIV:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " / ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_MOD:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " % ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_NEGATIVE:
            result->showInterValue();
            cout << " = -";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_GREATER:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " > ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_GREATER_EQUAL:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " >= ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_LESS:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " < ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_LESS_EQUAL:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " <= ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_EQUAL:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " == ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_NOT_EQUAL:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " != ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_NOT:
            result->showInterValue();
            cout << " = !";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_AND:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " && ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_OR:
            result->showInterValue();
            cout << " = ";
            arg1->showInterValue();
            cout << " || ";
            arg2->showInterValue();
            cout << endl;
            break;
        case OP_LEA:
            result->showInterValue();
            cout << " = &";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_POINTER_SET:
            cout << "*";
            arg1->showInterValue();
            cout << " = ";
            result->showInterValue();
            cout << endl;
            break;
        case OP_POINTER_GET:
            result->showInterValue();
            cout << " = *";
            arg1->showInterValue();
            cout << endl;
            break;
        case OP_JUMP:
            cout << "OP_JUMP " << jump_target->label_name << endl;
            break;
        case OP_JUMP_TRUE:
            cout << "if( ";
            arg1->showInterValue();
            cout << " ) jump to "  << jump_target->label_name << endl;
            break;
        case OP_JUMP_FALSE:
            cout << "if( !";
            arg1->showInterValue();
            cout << " ) jump to "  << jump_target->label_name << endl;
            break;
        case OP_ARG:
            cout << "OP_ARG ";
            arg1->showInterValue();
            break;
        case OP_FUN:
            cout << function->getName() << " () ";
            break;
        case OP_FUN_CALL:
            result->showInterValue();
            cout << " = " << function->getName() << endl;
            break;
        case OP_RETURN:
            cout << "RETURN goto " << jump_target->label_name << endl;
            break;
        case OP_RETURN_VALUE:
            cout << "RETURN value:";
            arg1->showInterValue();
            cout << "goto " << jump_target->label_name << endl;
            break;
        default:
            //默认为NOP指令
            cout << "DEFAULT OP_NOP" << endl;
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
    cout << "InterCode size = " << inter_code.size() << endl;
    for (int i = 0; i < inter_code.size(); ++i) {
        inter_code[i]->tostring();
    }
}
