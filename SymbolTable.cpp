//
// Created by 95792 on 2020/4/21.
//

#include "SymbolTable.h"
#include "GenerateCode.h"

VarElement* SymbolTable::var_void = nullptr;
VarElement* SymbolTable::num_zero = nullptr;
VarElement* SymbolTable::num_one = nullptr;
VarElement* SymbolTable::num_four = nullptr;

SymbolTable::SymbolTable() {
    //特殊变量的初始化
    var_void = new VarElement();
    num_zero = new VarElement(0);
    num_one = new VarElement(1);
    num_four = new VarElement(4);

    //作用域初始化
    scopeId = 0;
    scopePath.push_back(0);
    //当前分析函数指针为空
    currentFunction = nullptr;
    //对应的代码生成器为空
    generateCode = nullptr;
}

SymbolTable::~SymbolTable() = default;

void SymbolTable::setGenerateCoder(GenerateCode *g) {
    this->generateCode = g;
}

void SymbolTable::addVariable(VarElement *var) {
    //检查同名变量列表中是否有该变量
    string varName = var->getVarName();
    if (variableTable.find(varName) != variableTable.end()) {
        //将同名变量列表保存在varList引用变量中
        vector<VarElement*>& varList = *variableTable[varName];
        //优先考虑常量元素，常量元素无作用域
        if (varName == "[consInt]" || varName == "[consChar]")
            variableTable[varName]->push_back(var);
        else{
            //变量元素作用域检查，通过作用域尾部相同来判断
            int i;
            for (i = 0; i < varList.size(); i++) {
                if (varList[i]->getScopePath().back() == var->getScopePath().back())
                    break;
            }
            //无作用域冲突
            if (i == varList.size())
                varList.push_back(var);
            else{
                //出现相同作用域的冲突，变量重定义
                cout << "var redefine error" << endl;
                return;
            }
        }
    } else {
        //未找到同名变量列表，创建变量列表并将元素添加到列表中
        variableTable[varName] = new vector<VarElement*>;
        variableTable[varName]->push_back(var);
        //记录变量的添加顺序
        varList.push_back(varName);
    }

    //TODO 中间代码生成部分
    if (generateCode){
        int initFlag = generateCode->genVarInitial(var);
        if (currentFunction && initFlag)
            currentFunction->locateVar(var);
    }
}

void SymbolTable::addString(VarElement *var) {
//    hash_map<string, VarElement*, hash_string>::iterator consItr, consEnd = constantTable.end();
//    for (consItr = constantTable.begin(); consItr != consEnd ; ++consItr) {
//        VarElement* temp = consItr->second;
//        if(temp->getStrConstantValue() == var->getStrConstantValue()){
//            delete var;
//            var = temp;
//            return;
//        }
//    }
    //字符常量不存在的情况，添加
    constantTable[var->getVarName()] = var;
}

//根据变量名获取变量
VarElement *SymbolTable::getVariable(string name, vector<int>& sp) {
    VarElement* selectVar = nullptr;
    if (variableTable.find(name) != variableTable.end()){
        //找到同名变量列表
        vector<VarElement*> &varList = *variableTable[name];
        //变量作用域最长路径匹配原则
        int scopeLength = sp.size();
        int currentMatchedLength = 0;
        for (auto & i : varList) {
            int tempLength = i->getScopePath().size();
            //由于scopeID的唯一性 只需要匹配最后一个scope id就可以
            if(tempLength <= scopeLength && (i->getScopePath()[tempLength - 1] == sp[tempLength - 1])){
                if(tempLength > currentMatchedLength){
                    currentMatchedLength = tempLength;
                    selectVar = i;
                }
            }
        }
    } else
        cout << "In getVariable same-name variableList not found" << endl;
    if (!selectVar)
        cout << "Var not defined" << endl;
    return selectVar;
}

//获取全局变量并返回
vector<VarElement *> SymbolTable::getGlobalVariables() {
    vector<VarElement*> globalVars;
    for (int i = 0; i < varList.size(); ++i) {
        string var_name = varList[i];
        if (var_name[0] == '[')
            continue;
        vector<VarElement*>& list = *variableTable[var_name];
        for (int j = 0; j < list.size(); ++j) {
            if (list[j]->getScopePath().size() == 1){
                //作用域长度为1，全局变量
                globalVars.push_back(list[j]);
                break;
            }
        }
    }
    return globalVars;
}

//函数声明
void SymbolTable::declareFunction(FunElement *fun) {
    fun->setIsDeclare(true);
    //查询函数表
    string funName = fun->getName();
    if (functionTable.find(funName) == functionTable.end()){
        //函数变量尚未声明/定义
        functionTable[funName] = fun;
        //添加进函数表中
        funList.push_back(funName);
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
        funList.push_back(funName);
    } else {
        //函数已经声明，接着定义（不允许重复定义/重载）
        FunElement* temp = functionTable[funName];
        if (temp->getIsDeclare()){

            if (temp->funMatched(fun)){
                //函数匹配，声明转定义，则isDeclare设置为false，参数列表拷贝
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
    //符号表当前分析函数设置为fun
    currentFunction = fun;
    //函数入口代码生成
    generateCode->genFunctionEntry(currentFunction);
}

//结束函数声明
void SymbolTable::endDefineFunction() {
    //函数出口代码生成
    generateCode->genFunctionExit(currentFunction);
    currentFunction = nullptr;
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

//获取当前分析的函数
FunElement *SymbolTable::getCurrentFunction() {
    return currentFunction;
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

void SymbolTable::addInterInstruction(InterInstruction *i) {
    if(currentFunction)
        currentFunction->addInterInstruction(i);
    else
        delete i;
}

void SymbolTable::optimize() {

}

//输出符号表的相关信息
void SymbolTable::showInformation() {
    cout << "=======Symbol Table Begin=======" << endl;
    cout << "-------variables-------" << endl;
    for (int i = 0; i < varList.size(); ++i) {
        string var_name = varList[i];
        vector<VarElement*>& list = *variableTable[var_name];
//        cout << var_name.c_str() << " ";
        for (int j = 0; j < list.size(); ++j) {
            list[j]->showInformation();
            cout << endl;
        }
    }
    cout << "-------strings-------" << endl;
    hash_map<string, VarElement*, hash_string>::iterator strIt,strEnd=constantTable.end();
    for(strIt=constantTable.begin();strIt!=strEnd;++strIt)
        cout << strIt->second->getVarName() << "=" << strIt->second->getStrConstantValue() << endl;
    cout << "-------functions-------" << endl;
    int funListLength = funList.size();
    if (funListLength == 0)
        cout << "No function defined" << endl;
    else{
        for (int i = 0; i < funList.size(); ++i) {
            functionTable[funList[i]]->showInformation();
        }
    }
    cout << "=======Symbol Table End=======" << endl;
}

void SymbolTable::showInterCode() {
    cout << "=======Intermediate Code Begin=======" << endl;
    int funList_length = funList.size();
    if (funList_length == 0){
        cout << "No function defined" << endl;
    } else {
        //funList中函数不为空的情况
        for (int i = 0; i < funList.size(); ++i) {
            (functionTable[funList[i]])->showInterCode();
        }
    }
    cout << "=======Intermediate Code End=======" << endl;
}

void SymbolTable::showOptimizedCode() {

}











