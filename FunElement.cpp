//
// Created by 95792 on 2020/6/1.
//

#include "FunElement.h"
#include "GenerateCode.h"
#include "SymbolTable.h"
#include "DataFlowGraph.h"
#include "ConstPropagation.h"
#include "LiveAnalyse.h"
#include "RegisterAssign.h"

/*****定义的字符串输出，将tag枚举值转化为对应的字符串*****/
const char * funTagToString[] = {
        //特殊符号，错误和文件结束
        "error",
        "file_end",
        //变量id
        "identifier",
        //常量 数字，字符，字符串
        "constant_num",
        "constant_char",
        "constant_string",
        //关键保留字
        "main",
        "scanf",
        "printf",
        "int",
        "char",
        "float",
        "double",
        "bool",
        "const",
        "void",
        "extern",
        //控制结构关键字
        "if",
        "else",
        "switch",
        "case",
        "default",
        "while",
        "for",
        "do",
        "break",
        "continue",
        "return",
        //界限符号-运算符和判断符
        "+",
        "-",
        "*",
        "/",
        "%",
        "++",
        "--",
        "!",
        "&",
        "&&",
        "||",
        "=",
        ">",
        ">=",
        "<",
        "<=",
        "==",
        "!=",
        //界限符号特殊的
        ",",
        ":",
        ";",
        "{",
        "}",
        "(",
        ")",
        "[",
        "]"
};

/****函数元素*****/
FunElement::FunElement(string n, Tag t, vector<VarElement *>& l) {
    //默认处于声明状态
    isDeclare = true;
    name = n;
    returnType = t;
    parameterList = l;
    currentEspPosition = 0;
    maxEspDepth = 0;
    //函数局部变量的栈帧偏移初始化，为返回地址和栈顶开辟位置，所以从offset从8开始分配
    int tempOffset = 8;
    for (auto & i : parameterList) {
        i->setOffset(tempOffset);
        tempOffset += 4;
    }
}

FunElement::~FunElement() = default;

Tag FunElement::getReturnType() {
    return returnType;
}


//进入函数局部作用域
void FunElement::funScopeEnter() {
    scopeEsp.push_back(0);
}

//退出函数局部作用域
void FunElement::funScopeExit() {
    if (currentEspPosition > maxEspDepth)
        maxEspDepth = currentEspPosition;
    //当前栈指针位置减去作用域末元素
    currentEspPosition = currentEspPosition - scopeEsp.back();
    scopeEsp.pop_back();
}

//计算函数内变量的栈帧偏移
void FunElement::locateVar(VarElement *var) {
    int size = var->getVarSize();
    //栈帧内 4字节 对齐
    size += (4 - size%4) % 4;
    scopeEsp.back() += size;
    currentEspPosition += size;
    var->setOffset(-currentEspPosition);
}

//函数声明与定义匹配
bool FunElement::funMatched(FunElement *f) {
    //名字匹配
    if (name!= f->name)
        return false;
    //参数列表匹配
    if (parameterList.size() != f->parameterList.size())
        return false;
    int paraListLength = parameterList.size();
    for (int i = 0; i < paraListLength; ++i) {
        //类型兼容性检查 generateCode typeCheck
        if (GenerateCode::typeCheck(parameterList[i],f->parameterList[i])){
            if (parameterList[i]->getVarType() != f->parameterList[i]->getVarType()){
                //todo 语义错误 函数声明冲突
            }
        } else
            return false;
    }
    //返回值检查
    if (returnType != f->returnType){
        //todo 语义错误 函数返回值冲突
    }
    //经过检查无问题，返回正确值
    return true;
}

//函数参数匹配
bool FunElement::parameterMatched(vector<VarElement *> &p) {
    //参数列表大小检查
    if (parameterList.size() != p.size())
        return false;
    //逐项检查
    int length =parameterList.size();
    for (int i = 0; i < length; ++i) {
        //类型检查 generateCode
        if (!GenerateCode::typeCheck(parameterList[i], p[i]))
            return false;
    }
    return true;
}

bool FunElement::getIsDeclare() {
    return isDeclare;
}

void FunElement::setIsDeclare(bool isDec) {
    isDeclare = isDec;
}

string FunElement::getName() {
    return name;
}

void FunElement::copyParameterList(vector<VarElement *> p) {
    parameterList = p;
}

vector<VarElement *> FunElement::getParameterList() {
    return parameterList;
}

void FunElement::setFunReturnPoint(InterInstruction *point) {
    returnPoint = point;
}

InterInstruction *FunElement::getFunReturnPoint() {
    return returnPoint;
}

void FunElement::addInterInstruction(InterInstruction *i) {
    interIC.addInstruction(i);
}

void FunElement::optimize(SymbolTable *table) {
    //创建数据流图
    flowGraph = new DataFlowGraph(interIC);
    //输出数据流图有关信息
//    flowGraph->showInformation();
//    auto* constPropagation = new ConstPropagation(table, flowGraph, parameterList);
//    //进行常量传播
//    constPropagation->optimize();

    //变量活跃性分析，死代码消除
    LiveAnalyse liveAnalyse(flowGraph, table, parameterList);
    liveAnalyse.removeDeadCode();

    //将优化后的中间代码保存
    flowGraph->toInterCode(optimized_code);

    //todo 寄存器分配，局部变量地址重新计算
    ConflictGraph conflictGraph(optimized_code, parameterList, &liveAnalyse, this);
    conflictGraph.registerAssign();
}

bool FunElement::isBasicReturnType() {
    return returnType == KW_INT || returnType == KW_CHAR;
}

void FunElement::showInformation() {
    cout << funTagToString[returnType] << " ";
    cout << name << " ";
    //输出参数列表
    cout << "(";
    for (int i = 0; i < parameterList.size(); ++i) {
        cout << "<" << parameterList[i]->getVarName() << ">";
        if (i != parameterList.size()-1)
            cout << ",";
    }
    cout << ")" << endl;
    cout << "MaxStackDepth = " << maxEspDepth << endl;
}

void FunElement::showInterCode() {
    cout << "------function" << name << "InterCodeStart------" << endl;
    interIC.showIntermediateCode();
    cout << "------function" << name << "InterCodeEnd------" << endl;
}

void FunElement::showOptimizedCode() {
    cout << "----------" << name.c_str() << "\t optimized  start" << "-----------" << endl;
    for (auto & code : optimized_code) {
        code->tostring();
    }
    cout << "----------" << name.c_str() << "\t optimized  end" << "-----------" << endl;
}

int FunElement::getMaxEsp() {
    return maxEspDepth;
}

list<InterInstruction *> &FunElement::getOptimizedCode() {
    return optimized_code;
}





