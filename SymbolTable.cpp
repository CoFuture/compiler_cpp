//
// Created by 95792 on 2020/4/21.
//

#include "SymbolTable.h"
#include <utility>

SymbolTable::SymbolTable() {
    //作用域初始化
    scopeId = 0;
    scopePath.push_back(0);
    //当前分析函数指针为空
    currentFunction = nullptr;
}

//TODO symbol Table 的析构函数
SymbolTable::~SymbolTable() = default;

void SymbolTable::addVariable(VarElement *var) {
    string varName = var->getVarName();
    if (variableTable.find(varName) != variableTable.end()) {
        //找到了同名变量列表
        vector<VarElement *> &varList = *variableTable[varName];
        //优先考虑常量元素
        if (varName == "[consInt]" || varName == "[consChar]")
            variableTable[varName]->push_back(var);
        else{
            //变量元素作用域检查
            int i;
            for (i = 0; i < varList.size(); i++) {
                if (varList[i]->getScopePath().back() == var->getScopePath().back())
                    break;
            }
            //无作用域冲突
            if (i == varList.size())
                varList.push_back(var);
            else{
                cout << "var redefine error" << endl;
                return;
            }
        }
    } else {
        variableTable[varName] = new vector<VarElement*>;
        variableTable[varName]->push_back(var);
    }

    //TODO 中间代码生成部分
}

void SymbolTable::addConstant(VarElement *var) {
    hash_map<string, VarElement*, hash_string>::iterator consItr, consEnd = constantTable.end();
    for (consItr = constantTable.begin(); consItr != consEnd ; ++consItr) {
        VarElement* temp = consItr->second;
        if(temp->getStrConstantValue() == var->getStrConstantValue()){
            delete var;
            var = temp;
            return;
        }
    }
    //字符常量不存在的情况，添加
    constantTable[var->getVarName()] = var;
}

//根据变量名获取变量
VarElement *SymbolTable::getVariable(string name) {
    VarElement* selectVar = nullptr;
    if (variableTable.find(name) != variableTable.end()){
        //找到同名变量列表
        vector<VarElement*> &varList = *variableTable[name];
        //变量作用域最长路径匹配原则
        int scopeLength = scopePath.size();
        int currentMatchedLength = 0;
        for (int i = 0; i < varList.size(); i++) {
            int tempLength = varList[i]->getScopePath().size();
            //由于scopeID的唯一性 只需要匹配最后一个scope id就可以
            if(tempLength <= scopeLength && varList[i]->getScopePath()[tempLength - 1] == scopePath[tempLength - 1]){
                if(tempLength > currentMatchedLength){
                    currentMatchedLength = tempLength;
                    selectVar = varList[i];
                }
            }
        }
    } else
        cout << "In getVariable same name variableList not find" << endl;
    if (!selectVar)
        cout << "Var not defined" << endl;
    return selectVar;
}

//函数声明
void SymbolTable::declareFunction(FunElement *fun) {
    fun->setIsDeclare(true);
    //查询函数表
    string funName = fun->getName();
    if (functionTable.find(funName) == functionTable.end()){
        //函数变量尚未声明/定义
        functionTable[funName] = fun;
    } else{
        //函数变量已经存在,执行函数检查
        FunElement* temp = functionTable[funName];
        if (!fun->funMatched(temp))
            cout << "In declare function , fun declare error" << endl;
        //函数已经定义，删除当前fun
        delete fun;
    }
}

//函数定义
void SymbolTable::defineFunction(FunElement *fun) {
    //查询函数表
    string funName = fun->getName();
    if (functionTable.find(funName) == functionTable.end()){
        //未找到匹配的函数，添加
        functionTable[funName] = fun;
    } else {
        //函数已经声明，接着定义（不允许重复定义/重载）
        FunElement* temp = functionTable[funName];
        if (temp->getIsDeclare()){
            //todo continue xie
            //函数匹配，则设置isDeclare为false 并且拷贝参数列表
            if (temp->funMatched(fun)){
                temp->setIsDeclare(false);
                temp->copyParameterList(fun->getParameterList());
            } else
                cout << "in define function Fun Dec error" << fun->getName() << endl;
        } else {
            //函数是定义状态，报错
            cout << "Function redefined error" << endl;
        }
        //将 fun 指针指向最新的函数
        delete fun;
        fun = temp;
    }
}

//结束函数声明
void SymbolTable::endDefineFunction(FunElement *fun) {

}

//函数调用时使用：根据函数名字和参数列表获取函数
FunElement *SymbolTable::getFunction(string name, vector<VarElement *> &parameters) {
    if (functionTable.find(name) != functionTable.end()){
        //找到了同名函数列表
        FunElement* temp = functionTable[name];
        //参数列表检查
        if (temp->parameterMatched(parameters))
            return temp;
        else{
            cout << "In getFunction Fun call error parameter unmatched" << endl;
            return nullptr;
        }
    } else
        return nullptr;
}

//进入局部作用域
void SymbolTable::scopeEnter() {
    scopeId++;
    scopePath.push_back(scopeId);
    //如果当前处理的是函数元素，调用函数内部作用域管理和栈帧计算函数
    if (currentFunction)
        currentFunction->funScopeEnter();
}

//退出局部作用域
void SymbolTable::scopeExit() {
    scopePath.pop_back();
    //如果当前处理的是函数元素，调用函数内部作用域管理和栈帧计算函数
    if (currentFunction)
        currentFunction->funScopeExit();
}

//获取当前作用域路径
vector<int> &SymbolTable::getScopePath() {
    return scopePath;
}

/****函数元素*****/
FunElement::FunElement(string n, Tag t, vector<VarElement *> &l) {
    //默认处于声明状态
    isDeclare = true;
    name = n;
    returnType = t;
    parameterList = l;
    currentEspPosition = 0;
    maxEspDepth = 0;
    //函数局部变量的栈帧偏移初始化，为返回地址和栈顶开辟位置，所以从offset从8开始分配
    for (int i = 0, tempOff = 8; i < parameterList.size(); i++, tempOff += 4) {
        parameterList[i]->setOffset(tempOff);
    }
}

FunElement::~FunElement() = default;

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

//函数声明与定义匹配
bool FunElement::funMatched(FunElement *f) {
    //名字匹配
    if (name!= f->name)
        return false;
    //返回值检查
    if (returnType != f->returnType)
        return false;
    //参数列表匹配
    if (parameterList.size() != f->parameterList.size())
        return false;
    int paraListLength = parameterList.size();
    for (int i = 0; i < paraListLength; ++i) {
        //todo 类型兼容性检查
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
        //todo 类型检查
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

/****变量元素*****/
//(指针)变量的创建
VarElement::VarElement(vector<int>& sp, Tag t, string n, bool isPtr) {
    isArray = false;
    array_length = 0;
    isConstant = false;
    isLeft = true;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    scopePath = sp;
    type = t;
    name = n;
    isPointer = isPtr;
}

//(数组)变量的创建
VarElement::VarElement(vector<int> &sp, Tag t, string n, int len) {
    isConstant = false;
    isLeft = true;
    isPointer = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    isArray = true;
    array_length = len;
    scopePath = sp;
    type = t;
    name = n;
}

//常量对象的创建
VarElement::VarElement(Token *token) {
    isArray = false;
    array_length = 0;
    isPointer = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    isConstant = true;
    isLeft = false;
    switch (token->tag) {
        case CONSTANT_NUM:
            type = KW_INT;
            name = "[consInt]";
            intValue = ((Num*)token)->num_value;
            break;
        case CONSTANT_CHAR:
            type = KW_CHAR;
            name = "[consChar]";
            charValue = ((Character*)token)->character_value;
            break;
        case CONSTANT_STRING:
            type = KW_CHAR;
            stringValue = ((Str*)token)->string_value;
            //采用字符数组形式存储字符串
            isArray = true;
            isLeft = true;
            array_length = stringValue.size() + 1;
            //todo size设置
            break;
        default:
            cout << "In create constant var element type error" << endl;
            break;
    }
}

VarElement::~VarElement() = default;

string VarElement::getVarName() {
    return name;
}

//获取变量作用域
vector<int> &VarElement::getScopePath() {
    return scopePath;
}

//获取字符常量值
string VarElement::getStrConstantValue() {
    return stringValue;
}

//设置局部变量偏移
void VarElement::setOffset(int off) {
    offset = off;
}


