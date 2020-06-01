//
// Created by 95792 on 2020/6/1.
//

#include "IntermediateCode.h"

/*中间指令部分函数的具体实现*/
void InterInstruction::initial() {
    this->op = OP_NOP;
    this->result = nullptr;
    this->arg1 = nullptr;
    this->arg2 = nullptr;
    this->function = nullptr;
    this->jump_target = nullptr;
    this->label_name = "";
}

InterInstruction::InterInstruction() {
    this->op = OP_NOP;
    this->result = nullptr;
    this->arg1 = nullptr;
    this->arg2 = nullptr;
    this->function = nullptr;
    this->jump_target = nullptr;
    //TODO 随机生成唯一的label名称
    this->label_name = "";
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

InterInstruction::InterInstruction(Operator o, InterInstruction *t, VarElement *c) {
    this->op = o;
    this->result = nullptr;
    this->arg1 = c;
    this->arg2 = nullptr;
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

InterInstruction::~InterInstruction() {

}




/*中间代码部分函数的具体实现*/
IntermediateCode::~IntermediateCode() {

}

void IntermediateCode::addInstruction(InterInstruction *i) {

}

vector<InterInstruction *> &IntermediateCode::getIntermediateCode() {

}
