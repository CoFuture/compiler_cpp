//
// Created by 95792 on 2020/6/1.
//

#include "IntermediateCode.h"
#include "GenerateCode.h"

#define emit(code, args...) fprintf(file, "\t" code "\n", ##args)
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
    //优化相关
    this->first = false;
    this->block = nullptr;
    //默认不是死代码
    isDead=false;
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
    //优化相关
    this->first = false;
    this->block = nullptr;
    //默认不是死代码
    isDead=false;
}

InterInstruction::InterInstruction(Operator o, VarElement *r, VarElement *v1, VarElement *v2) {
    this->op = o;
    this->result = r;
    this->arg1 = v1;
    this->arg2 = v2;
    this->function = nullptr;
    this->jump_target = nullptr;
    this->label_name = "";
    //优化相关
    this->first = false;
    this->block = nullptr;
    //默认不是死代码
    isDead=false;
}

InterInstruction::InterInstruction(Operator o, InterInstruction *t, VarElement *var1, VarElement* var2) {
    this->op = o;
    this->result = nullptr;
    this->arg1 = var1;
    this->arg2 = var2;
    this->function = nullptr;
    this->jump_target = t;
    this->label_name = "";
    //优化相关
    this->first = false;
    this->block = nullptr;
    //默认不是死代码
    isDead=false;
}

InterInstruction::InterInstruction(Operator o, VarElement *arg) {
    this->op = o;
    this->result = nullptr;
    this->arg1 = arg;
    this->arg2 = nullptr;
    this->function = nullptr;
    this->jump_target = nullptr;
    this->label_name = "";
    //优化相关
    this->first = false;
    this->block = nullptr;
    //默认不是死代码
    isDead=false;
}

void InterInstruction::tostring() {
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
            cout << endl;
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

Operator InterInstruction::getOperator() {
    return op;
}

VarElement *InterInstruction::getResult() {
    return result;
}

VarElement *InterInstruction::getArg1() {
    return arg1;
}

VarElement *InterInstruction::getArg2() {
    return arg2;
}

FunElement *InterInstruction::getFunction() {
    return function;
}

InterInstruction *InterInstruction::getJumpTarget() {
    return jump_target;
}

string InterInstruction::getLabel() {
    return label_name;
}

//将指令标记为首指令
void InterInstruction::setFirst() {
    this->first = true;
}

bool InterInstruction::isFirst() {
    return first;
}

bool InterInstruction::isJump() {
    //无条件跳转指令和 return语句
    return op == OP_JUMP || op == OP_RETURN || op == OP_RETURN_VALUE;
}

bool InterInstruction::isJumpCondition() {
    return op == OP_JUMP_FALSE || op == OP_JUMP_TRUE || op == OP_JUMP_NOT_EQUAL;
}

bool InterInstruction::isLabel() {
    return !label_name.empty();
}

bool InterInstruction::isDeclare() {
    return op == OP_DECLARE;
}

//指针的取值包括在内
bool InterInstruction::isExpression() {
    return ((op >= OP_ASSIGN && op <= OP_OR) || op == OP_POINTER_GET);
}

bool InterInstruction::isUncertain() {
    return op == OP_POINTER_SET || op == OP_FUN || op == OP_FUN_CALL;
}

//替换普通运算指令
void InterInstruction::replace(Operator o, VarElement *r, VarElement *var1, VarElement *var2) {
    this->op = o;
    this->result = r;
    this->arg1 = var1;
    this->arg2 = var2;
}

//替换跳转指令
void InterInstruction::replace(Operator o, InterInstruction *t, VarElement *var1, VarElement *var2) {
    this->op = o;
    this->jump_target = t;
    this->arg1 = var1;
    this->arg2 = var2;
}

void InterInstruction::setArg1(VarElement* var) {
    this->arg1 = var;
}

void InterInstruction::toSimpleCall() {
    this->result = nullptr;
    this->op = OP_FUN;
}

void InterInstruction::loadVar(string reg, VarElement *var, FILE* fp) {
    if (!var)
        return;
    if (var->getVarType() == KW_CHAR && var->isBasicType())
        fprintf(fp, "\tmov %s,0\n", reg.c_str());

    const char* reg_name = reg.c_str();
    const char* var_name = (var->getVarName()).c_str();
    if (!var->getIsConstant()){
        int off = var->getOffset();
        if (!off){
            if (!var->getIsArray())
                fprintf(fp, "\tmov %s,[%s]\n", reg_name, var_name);
            else
                fprintf(fp, "\tmov %s,%s\n", reg_name, var_name);
        } else {
            if (!var->getIsArray())
                fprintf(fp, "\tmov %s,[ebp%+d]\n", reg_name, off);
            else
                fprintf(fp, "\tlea %s,[ebp%+d]\n", reg_name, off);
        }
    } else {
        //常量
        if (var->isBasicType())
            fprintf(fp, "\tmov %s,%d\n", reg_name, var->getConstantValue());
        else
            fprintf(fp, "\tmov %s,%s\n", reg_name, var_name);
    }
}

void InterInstruction::leaVar(string reg, VarElement *var, FILE* fp) {
    if (!var)
        return;
    const char* reg_name = reg.c_str();
    const char* var_name = var->getVarName().c_str();
    int off = var->getOffset();
    if (off)
        fprintf(fp, "\tlea %s,[ebp%+d]\n", reg_name, off);
    else
        fprintf(fp, "\tmov %s,%s\n", reg_name, var_name);
}

void InterInstruction::storeVar(string reg, VarElement *var, FILE* fp) {
    if (!var)
        return;
    const char* reg_name = reg.c_str();
    const char* var_name = var->getVarName().c_str();
    int off = var->getOffset();
    if (off)
        fprintf(fp, "\tlea [ebp%+d],%s\n", off, reg_name);
    else
        fprintf(fp, "\tmov [%s],%s\n", var_name, reg_name);
}

void InterInstruction::initVar(VarElement *var, FILE* fp) {
    if (!var)
        return;
    if (!var->getIsInited()){
        if (var->isBasicType())
            fprintf(fp, "\tmov eax,%d\n",var->getConstantValue());
        else
            fprintf(fp, "\tmov eax,%s\n",var->getCharPointerValue().c_str());
    }
}

void InterInstruction::toX86() {
    vector<string> regList;
    regList.emplace_back("eax");
    regList.emplace_back("ebx");
    regList.emplace_back("ecx");
    regList.emplace_back("edx");
    regList.emplace_back("esi");
    regList.emplace_back("edi");
    FILE * file;
    file = fopen(R"(F:\CodeFiles\CLion Projects\compiler-cpp\x86Code.txt)","a");
    if (!label_name.empty()){
        fprintf(file, "%s:\n", label_name.c_str());
        fclose(file);
        return;
    }
    switch (op) {
        case OP_FUN_ENTRY:
            fprintf(file, "\tpush ebp\n");
            fprintf(file, "\tmov ebp,esp\n");
            fprintf(file, "\tsub esp,%d\n", this->getFunction()->getMaxEsp());
            break;
        case OP_FUN_EXIT:
            fprintf(file, "\tmov esp,ebp\n");
            fprintf(file, "\tpop ebp\n");
            fprintf(file, "\tret\n");
            break;
        case OP_DECLARE:
            initVar(arg1, file);
            break;
        case OP_ASSIGN:
            loadVar("eax", arg1, file);
            storeVar("eax", result, file);
            break;
        case OP_ADD:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tadd eax,ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_SUB:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tsub eax,ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_MUL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\timul ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_DIV:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tidiv ebx\n");
            storeVar("edx", result, file);
            break;
        case OP_MOD:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tidiv ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_NEGATIVE:
            loadVar("eax", arg1, file);
            fprintf(file, "\tneg eax\n");
            storeVar("eax", result, file);
            break;
        case OP_GREATER:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsetg cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_GREATER_EQUAL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsetge cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_LESS:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsetl cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_LESS_EQUAL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsetle cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_EQUAL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsete cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_NOT_EQUAL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tmov ecx,0\n");
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tsetne cl\n");
            storeVar("ecx", result, file);
            break;
        case OP_NOT:
            loadVar("eax", arg1, file);
            fprintf(file, "\tmov ebx,0\n");
            fprintf(file, "\tcmp eax,0\n");
            fprintf(file, "\tsete bl\n");
            storeVar("ebx", result, file);
            break;
        case OP_AND:
            loadVar("eax", arg1, file);
            fprintf(file, "\tcmp eax,0\n");
            fprintf(file, "\tsetne cl\n");
            loadVar("ebx", arg2, file);
            fprintf(file, "\tcmp ebx,0\n");
            fprintf(file, "\tsetne bl\n");
            fprintf(file, "\tadd eax,ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_OR:
            loadVar("eax", arg1, file);
            fprintf(file, "\tcmp eax,0\n");
            fprintf(file, "\tsetne al\n");
            loadVar("ebx", arg2, file);
            fprintf(file, "\tcmp ebx,0\n");
            fprintf(file, "\tsetne bl\n");
            fprintf(file, "\tor eax,ebx\n");
            storeVar("eax", result, file);
            break;
        case OP_LEA:
            leaVar("eax", arg1, file);
            storeVar("eax", result, file);
            break;
        case OP_POINTER_SET:
            loadVar("eax", result, file);
            loadVar("ebx", arg1, file);
            fprintf(file, "\tmov [ebx],eax\n");
            break;
        case OP_POINTER_GET:
            loadVar("eax", arg1, file);
            fprintf(file, "\tmov eax,[eax]\n");
            storeVar("eax", result, file);
            break;
        case OP_JUMP:
            fprintf(file, "\tjmp %s\n", jump_target->label_name.c_str());
            break;
        case OP_JUMP_TRUE:
            loadVar("eax", arg1, file);
            fprintf(file, "\tcmp eax,0\n");
            fprintf(file, "\tjne %s\n", jump_target->label_name.c_str());
            break;
        case OP_JUMP_FALSE:
            loadVar("eax", arg1, file);
            fprintf(file, "\tcmp eax,0\n");
            fprintf(file, "\tje %s\n", jump_target->label_name.c_str());
            break;
        case OP_JUMP_NOT_EQUAL:
            loadVar("eax", arg1, file);
            loadVar("ebx", arg2, file);
            fprintf(file, "\tcmp eax,ebx\n");
            fprintf(file, "\tjne %s\n", jump_target->label_name.c_str());
            break;
        case OP_ARG:
            loadVar("eax", arg1, file);
            fprintf(file, "\tpush eax\n");
            break;
        case OP_FUN:
            fprintf(file, "\tcall %s\n", function->getName().c_str());
            fprintf(file, "\tadd esp,%d\n", function->getParameterList().size() * 4);
            break;
        case OP_FUN_CALL:
            fprintf(file, "\tcall %s\n", function->getName().c_str());
            fprintf(file, "\tadd esp,%d\n", function->getParameterList().size() * 4);
            storeVar("eax", result, file);
            break;
        case OP_RETURN:
            fprintf(file, "\tjmp %s\n", jump_target->label_name.c_str());
            break;
        case OP_RETURN_VALUE:
            loadVar("eax", arg1, file);
            fprintf(file, "\tjmp %s\n", jump_target->label_name.c_str());
            break;
    }
    fclose(file);
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

void IntermediateCode::markFirst() {
    int length = inter_code.size();
    //首尾两条指令标记为首指令
//    inter_code[0]->setFirst();          //OP_ENTRY
//    inter_code[length - 1]->setFirst(); //OP_EXIT
//    inter_code[1]->setFirst();          //第一条指令
    for (int i = 0; i < length; ++i) {
        if (i == 0 || i == 1 || i == length - 1){
            //entry exit 和 第一条指令设为首指令
            inter_code[i]->setFirst();
            continue;
        }

        //跳转指令和紧跟跳转指令后面的指令为首指令
        if (inter_code[i]->isJump() || inter_code[i]->isJumpCondition()){
            inter_code[i]->setFirst();
            inter_code[i+1]->setFirst();
        }
    }
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

