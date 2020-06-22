//
// Created by 95792 on 2020/6/1.
//

#include "GenerateCode.h"
#include "SymbolTable.h"

GenerateCode::GenerateCode(SymbolTable &table):symbolTable(table) {
    //将符号表与中间代码生成器一一对应
    symbolTable.setGenerateCoder(this);
    //全局标签计数器初始化为0
    label_num = 0;
    //入口和出口的初始化
    entry_stack.push_back(nullptr);
    exit_stack.push_back(nullptr);
}

string GenerateCode::genLabel() {
    label_num++;
    string lb="@Label";
    return lb + to_string(label_num);
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
    //进入函数，函数栈帧准备计数
    f->funScopeEnter();
    //产生函数入口点中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_FUN_ENTRY, f));
    //创建函数的返回点（不分配）
    f->setFunReturnPoint(new InterInstruction);
}

void GenerateCode::genFunctionExit(FunElement *f) {
    //获取函数的返回点
    symbolTable.addInterInstruction(f->getFunReturnPoint());
    //分配函数的返回点出口
    symbolTable.addInterInstruction(new InterInstruction(OP_FUN_EXIT, f));
    //函数栈帧退出
    f->funScopeExit();
}

//空返回值以VarElement* isVoid = true形式返回
void GenerateCode::genReturn(VarElement *var) {
    //若返回值var为null则return
    if (!var)
        return;
    //获取当前分析的函数
    FunElement* cur_fun = symbolTable.getCurrentFunction();
    // 语义检查——函数返回值类型不匹配（非基本类型的转换）
    if (cur_fun->getReturnType() != var->getVarType()){
        if (!var->isBasicType() || !cur_fun->isBasicReturnType())
            //todo 提示语法错误
            return;
    }
    //获取函数返回点位置
    InterInstruction* return_point = cur_fun->getFunReturnPoint();

    if (var->getVarType() == KW_VOID)
        //无函数返回值
        symbolTable.addInterInstruction(new InterInstruction(OP_RETURN, return_point));
    else{
        //有函数返回值
        // return *pointer 的情况
        if (var->getIsPointed())
            var = genPointer(var);
        symbolTable.addInterInstruction(new InterInstruction(OP_RETURN, return_point, var));
    }
}

//指针运算右值赋值
VarElement *GenerateCode::genPointerAssign(VarElement* var) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), var);
    symbolTable.addVariable(temp);
    //处理指针运算的特殊情况 temp = *var
    if(var->getIsPointed()){
        symbolTable.addInterInstruction(new InterInstruction(OP_POINTER_GET, temp, var->getPointer()));
    } else
        //temp = var
        symbolTable.addInterInstruction(new InterInstruction(OP_ASSIGN, temp, var));
    return temp;
}

//l_var = r_var，要求l_var是左值
VarElement *GenerateCode::genAssign(VarElement *l_var, VarElement *r_var) {
    //左值检查
    if (!l_var->getIsLeft()){
        //todo 语义错误 非左值不能赋值
        return r_var;
    }
    //类型检查
    if (!typeCheck(l_var, r_var)){
        //todo 语义错误 类型不匹配
        return r_var;
    }
    //指针类型的特殊处理
    //如果r_var是指针运算结果，则取pointValue
    if (r_var->getIsPointed()){
        r_var = genPointerAssign(r_var);
    }
    //如果l_var是指针运算结果 *l = r
    if (l_var->getIsPointer()){
        symbolTable.addInterInstruction(new InterInstruction(OP_POINTER_SET, l_var->getPointer(), r_var));
    } else
        //l = r
        symbolTable.addInterInstruction(new InterInstruction(OP_ASSIGN, l_var, r_var));
    return l_var;
}

VarElement *GenerateCode::genTwoOperation(Operator o, VarElement *var1, VarElement *var2) {
    if (!var1 || !var2){
        //todo 语义错误 双目运算运算数非空
        return nullptr;
    }

    if (var1->getIsVoid() || var2->getIsVoid()){
        //todo 语义错误 双目运算运算数类型错误
        return nullptr;
    }

    if (o == OP_ASSIGN)
        return genAssign(var1, var2);
    if (var1->getIsPointed())
        var1 = genPointerAssign(var1);
    if (var2->getIsPointed())
        var2 = genPointerAssign(var2);
    //非基本类型也可参与的运算
    if (o == OP_AND)
        return genAnd(var1, var2);
    else if (o == OP_OR)
        return genOr(var1, var2);
    else if (o == OP_ADD)
        return genAdd(var1, var2);
    else if (o == OP_SUB)
        return genSub(var1, var2);
    else if (o == OP_EQUAL)
        return genEqual(var1, var2);
    else if (o == OP_NOT_EQUAL)
        return genNotEqual(var1, var2);
    else{
        //只有基本类型可以参与的运算
        if (!var1->isBasicType() || !var2->isBasicType())
        {
            //todo 语义错误 非基本数值类型
            return var1;
        }
        if (o == OP_GREATER)
            return genGreater(var1, var2);
        if (o == OP_GREATER_EQUAL)
            return genGreaterEqual(var1, var2);
        if (o == OP_LESS)
            return genLess(var1, var2);
        if (o == OP_LESS_EQUAL)
            return genLessEqual(var1, var2);
        if (o == OP_MUL)
            return genMul(var1, var2);
        if (o == OP_DIV)
            return genDiv(var1, var2);
        if (o == OP_MOD)
            return genMod(var1, var2);
        //都不是，返回var1
        return var1;
    }
}

VarElement *GenerateCode::genAdd(VarElement *var1, VarElement *var2) {
    VarElement* temp = nullptr;
    //考虑指针加减——数组a[n] a+1 1+a的情况
    if (!var1->isBasicType() && var2->isBasicType()){
       //a + 1
       temp = new VarElement(symbolTable.getScopePath(), var1);
       //temp + var2 * step
       var2 = genMul(var2, VarElement::getStep(var1));
    }
    else if (var1->isBasicType() && !var2->isBasicType()){
        //1 + a
        temp = new VarElement(symbolTable.getScopePath(), var2);
        //temp + var2 * step(sizeof(type))
        var2 = genMul(var1, VarElement::getStep(var2));
    } else if (var1->isBasicType() && var2->isBasicType()){
        //都是基本类型，普通相加即可，创建临时temp变量
        temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    } else{
        //两个非基本类型 pointer +　pointer
        //todo 语义错误，两个指针相加
        return var1;
    }
    //将temp添加到符号表
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_ADD, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genSub(VarElement *var1, VarElement *var2) {
    VarElement* temp = nullptr;
    //var2为指针非法
    if (!var2->isBasicType()){
        // 1 - pointer情况，非法
        //todo 语义错误
        return var1;
    }

    if (var1->isBasicType())
        //均为基本类型，相减即可
        temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    else{
        //var1非基本类型，var1 - var2 * step
        temp = new VarElement(symbolTable.getScopePath(), var1);
        var2 = genMul(var2, VarElement::getStep(var1));
    }
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_SUB, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genMul(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_MUL, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genDiv(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_DIV, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genAnd(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_AND, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genMod(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_MOD, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genOr(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_OR, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genEqual(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_EQUAL, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genNotEqual(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_NOT_EQUAL, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genGreater(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_GREATER, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genGreaterEqual(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_GREATER_EQUAL, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genLess(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_LESS, temp, var1, var2));
    return temp;
}

VarElement *GenerateCode::genLessEqual(VarElement *var1, VarElement *var2) {
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //将temp添加到符号表中
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_LESS_EQUAL, temp, var1, var2));
    return temp;
}

//单目运算符 * & ++ --（前缀） - ！
VarElement *GenerateCode::genOneOperation(Operator o, VarElement *var) {
    if (!var)
        //由于语法部分有运算的优先级，允许var为空
        return nullptr;
    if (var->getIsVoid())
    {
        //todo 语义错误 不允许void
        return nullptr;
    }

    if (o == OP_MUL)
        return genPointer(var);
    if (o == OP_LEA)
        return genLea(var);
    if (o == OP_INC)
        return genInc(var);
    if (o == OP_DEC)
        return genDec(var);
    // ! 和 - 的操作数是右值
    if (var->getIsPointed())
        var = genPointerAssign(var);
    if (o == OP_SUB)
        return genNegative(var);
    if (o == OP_NOT)
        return genNot(var);

    return var;
}

//前缀++
VarElement *GenerateCode::genInc(VarElement *var) {
    //要求必须是左值
    if (!var->getIsLeft()){
        //todo 语义错误 非左值无法++
        return var;
    }
    // ++ *p
    if (var->getIsPointed()){
        VarElement* temp1 = genPointerAssign(var);//temp1 = *p
        VarElement* temp2 = genAdd(temp1, VarElement::getStep(var));//temp2 = temp1 + 1
        genAssign(var, temp2);//*p = temp2
    } else
        symbolTable.addInterInstruction(new InterInstruction(OP_ADD, var, var, VarElement::getStep(var)));
    return var;
}

//前缀--
VarElement *GenerateCode::genDec(VarElement *var) {
    //要求必须是左值
    if (!var->getIsLeft()){
        //todo 语义错误 非左值无法++
        return var;
    }
    // ++ *p
    if (var->getIsPointed()){
        VarElement* temp1 = genPointerAssign(var);//temp1 = *p
        VarElement* temp2 = genSub(temp1, VarElement::getStep(var));//temp2 = temp1 - 1
        genAssign(var, temp2);//*p = temp2
    } else
        symbolTable.addInterInstruction(new InterInstruction(OP_SUB, var, var, VarElement::getStep(var)));
    return var;
}

//取反运算
VarElement *GenerateCode::genNot(VarElement *var) {
    //创建临时变量
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //添加到符号表
    symbolTable.addVariable(temp);
    //添加中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_NOT, temp, var));
    return temp;
}

//&x取地址运算要求变量是左值
VarElement *GenerateCode::genLea(VarElement *var) {
    if (!var->getIsLeft()){
        //todo 语法错误 非左值不能取地址运算
        return var;
    }
    //判断指针运算的特殊情况
    if (var->getIsPointed()){
        //返回指向该变量的指针变量
        return var->getPointer();
    } else{
        //temp = &var 则 *temp = var
        //无指向该变量的指针，产生临时变量(指针变量)和中间代码
        VarElement* temp = new VarElement(symbolTable.getScopePath(), var->getVarType(), true);
        symbolTable.addVariable(temp);
        //创建中间代码
        InterInstruction* inter_ins = new InterInstruction(OP_LEA, temp, var);
        symbolTable.addInterInstruction(inter_ins);
        return temp;
    }
}

//指针运算 temp = *var
VarElement *GenerateCode::genPointer(VarElement *var) {
    if (var->isBasicType()){
        //todo 语义报错，基本类型不能进行指针运算
        return var;
    }
    //创建运算结果的临时变量
    VarElement* temp = new VarElement(symbolTable.getScopePath(), var->getVarType(), false);
    //指针运算结果可作为左值
    temp->setLeft(true);
    //*var 指向 temp变量
    temp->setPointerValue(var);
    //将temp变量添加到符号表
    symbolTable.addVariable(temp);
    return temp;
}


//取负运算 返回值为生成的临时变量
VarElement *GenerateCode::genNegative(VarElement *var) {
    //操作数需要是基本类型 int char(非指针)
    if (!var->isBasicType()){
        //todo 语义错误 非基本类型
        return var;
    }
    //创建临时变量
    VarElement* temp = new VarElement(symbolTable.getScopePath(), KW_INT, false);
    //添加到符号表
    symbolTable.addVariable(temp);
    //生成中间代码
    symbolTable.addInterInstruction(new InterInstruction(OP_NEGATIVE, temp, var));
    return temp;
}

//后缀单目运算符 ++ -- 操作数要求是左值
VarElement *GenerateCode::genOneOperationRight(Operator o, VarElement *var) {
    //var为空
    if (!var)
        return nullptr;
    //var为void
    if (var->getIsVoid()){
        //todo 语义错误
        return nullptr;
    }
    //var非左值
    if (!var->getIsLeft()){
        //todo 语义错误
        return var;
    }
    if (o == OP_INC)
        return genIncRight(var);
    if (o == OP_DEC)
        return genDecRight(var);
    return var;
}

//后缀自加运算 ++
VarElement *GenerateCode::genIncRight(VarElement *var) {
    //先取运算结果 temp=var
    VarElement* temp = genPointerAssign(var);
    if (var->getIsPointed()){
        //(*p) ++ 的情况, temp1 = *p
        //temp2 = temp1 + 1
        VarElement* temp2 = genAdd(temp, VarElement::getStep(var));
        //*p = temp2
        genAssign(var, temp2);
    } else {
        symbolTable.addInterInstruction(new InterInstruction(OP_ADD, var, var, VarElement::getStep(var)));
    }
    return temp;
}

//后缀自减运算 --
VarElement *GenerateCode::genDecRight(VarElement *var) {
    //先取运算结果 temp=var
    VarElement* temp = genPointerAssign(var);
    if (var->getIsPointed()){
        //(*p) -- 的情况, temp1 = *p
        //temp2 = temp1 + 1
        VarElement* temp2 = genSub(temp, VarElement::getStep(var));
        //*p = temp2
        genAssign(var, temp2);
    } else {
        symbolTable.addInterInstruction(new InterInstruction(OP_SUB, var, var, VarElement::getStep(var)));
    }
    return temp;
}

//数组索引运算 a[i] -> *(a + i)
VarElement *GenerateCode::genArrayIndex(VarElement *array, VarElement *index) {
    //array && index not null
    if (!array || !index)
        return nullptr;
    if (array->getIsVoid() || index->getIsVoid()){
        //todo 语义错误
        return nullptr;
    }
    //array不是指针类型，index是指针类型，语义错误
    if (array->isBasicType() || !index->isBasicType()){
        //todo 语义错误
        return index;
    }
    return genPointer(genAdd(array, index));
}

//函数参数入栈
void GenerateCode::genArgs(VarElement *arg) {
    //考虑参数是否为指针运算结果
    if (arg->getIsPointed())
        arg = genPointerAssign(arg);
    symbolTable.addInterInstruction(new InterInstruction(OP_ARG, arg));
}

//函数调用 参数为函数名和参数，返回值为函数调用返回值
VarElement *GenerateCode::genFunctionCall(FunElement *fun, vector<VarElement *> &args) {
    if (!fun)
        return nullptr;
    //参数入栈, 倒序入栈
    for (int i = (args.size() - 1); i >= 0; i--) {
        genArgs(args[i]);
    }
    //函数返回值类型是void
    if (fun->getReturnType() == KW_VOID){
        symbolTable.addInterInstruction(new InterInstruction(OP_FUN, fun));
        return SymbolTable::var_void;
    } else {
        //函数存在非void返回值
        VarElement* return_value = new VarElement(symbolTable.getScopePath(), fun->getReturnType(), false);
        symbolTable.addVariable(return_value);
        symbolTable.addInterInstruction(new InterInstruction(OP_FUN_CALL, fun, return_value));
        return return_value;
    }
}

//产生目标标签为t_else的jump指令
void GenerateCode::genIfHead(VarElement *condition, InterInstruction *& t_else) {
    t_else = new InterInstruction();
    if (condition){
        if (condition->getIsPointed())
            condition = genPointerAssign(condition);
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP_FALSE, t_else, condition));
    } else {
        //todo 语义错误 条件非空
        return;
    }
}

//产生目标标签为t_else的标签
void GenerateCode::genIfTail(InterInstruction *&t_else) {
    symbolTable.addInterInstruction(t_else);
}

//产生t_exit标签，无条件跳转和t_else
void GenerateCode::genElseHead(InterInstruction *&t_else, InterInstruction *&t_exit) {
    t_exit = new InterInstruction;
    symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, t_exit));
    symbolTable.addInterInstruction(t_else);
}

void GenerateCode::genElseTail(InterInstruction *&t_exit) {
    symbolTable.addInterInstruction(t_exit);
}

//创建exit标签并保存
void GenerateCode::genSwitchHead(InterInstruction *&t_exit) {
    t_exit = new InterInstruction();
    //switch中不支持continue，所以不能返回开头
    pushEntryAndExit(nullptr, t_exit);
}

//添加exit标签，并且进行出口管理
void GenerateCode::genSwitchTail(InterInstruction *&t_exit) {
    symbolTable.addInterInstruction(t_exit);
    popEntryAndExit();
}

//创建case exit标签，生成跳转指令
void GenerateCode::genCaseHead(VarElement *condition, VarElement *label, InterInstruction *&t_case_exit) {
    t_case_exit = new InterInstruction();
    if (condition){
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP_NOT_EQUAL, t_case_exit, condition, label));
    } else {
        //todo 语义错误 condition非空
    }
}

//添加case exit标签
void GenerateCode::genCaseTail(InterInstruction *&t_case_exit) {
    symbolTable.addInterInstruction(t_case_exit);
}

//创建while入口标签，添加while入口，创建exit标签，使用stack进行出入口管理
void GenerateCode::genWhileHead(InterInstruction *&t_while, InterInstruction *&t_while_exit) {
    t_while = new InterInstruction();
    symbolTable.addInterInstruction(t_while);
    t_while_exit = new InterInstruction();
    //入口和出口管理，方便break和continue
    pushEntryAndExit(t_while, t_while_exit);
}

//处理循环条件，目标为exit的Jump false 指令
void GenerateCode::genWhileCondition(VarElement *condition, InterInstruction*& t_while_exit) {
    if (condition){
        //空循环条件用void表示
        if (condition->getIsVoid())
            condition = SymbolTable::num_one;
        else if (condition->getIsPointed())
            condition = genPointerAssign(condition);
        //创建中间代码
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP_FALSE, t_while_exit, condition));
    }
}

//生成目标是入口t_while的跳转指令，生成while出口
void GenerateCode::genWhileTail(InterInstruction *&t_while, InterInstruction *&t_while_exit) {
    symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, t_while));
    symbolTable.addInterInstruction(t_while_exit);
    //入口和出口管理
    popEntryAndExit();
}

//创建入口do 和出口 exit标签，添加入口指令，同时stack进行管理
void GenerateCode::genDoWhileHead(InterInstruction *&t_do, InterInstruction *&t_exit) {
    t_do = new InterInstruction();
    t_exit = new InterInstruction();
    symbolTable.addInterInstruction(t_do);
    pushEntryAndExit(t_do, t_exit);
}

//生成目标为入口的跳转指令，进行stack管理
void GenerateCode::genDoWhileTail(VarElement *condition, InterInstruction *&t_do, InterInstruction *&t_exit) {
    if (condition){
        if (condition->getIsVoid())
            condition = SymbolTable::num_one;
        else if (condition->getIsPointed())
            condition = genPointerAssign(condition);
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP_TRUE, t_do, condition));
        symbolTable.addInterInstruction(t_exit);
        popEntryAndExit();
    } else {
        cout << "do-while condition is null pointer" << endl;
    }
}

//创建入口和出口，并且由stack进行管理
void GenerateCode::genForHead(InterInstruction *&t_for, InterInstruction *&t_exit) {
    t_for = new InterInstruction();
    t_exit = new InterInstruction();
    symbolTable.addInterInstruction(t_for);
}

//创建for循环体入口block，循环结束操作入口step，创建跳转到出口的for循环体入口的治理，使用stack进行出入口管理
void GenerateCode::genForConditionHead(VarElement *condition, InterInstruction *&t_step, InterInstruction *&t_for_block,
                                       InterInstruction *&t_exit) {
    t_step = new InterInstruction();
    t_for_block = new InterInstruction();
    if (condition){
        if (condition->getIsVoid())
            condition = SymbolTable::num_one;
        else if(condition->getIsPointed())
            condition = genPointerAssign(condition);
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP_FALSE, t_exit, condition));
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, t_for_block));
    } else
        cout << "In for-statement condition is null pointer" << endl;
    symbolTable.addInterInstruction(t_step);
    //continue入口step，break出口exit
    pushEntryAndExit(t_step, t_exit);
}

//生成跳转到for入口的指令，添加循环体入口指令
void GenerateCode::genForConditionTail(InterInstruction *&t_for, InterInstruction *&t_for_block) {
    symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, t_for));
    symbolTable.addInterInstruction(t_for_block);
}

//生成跳转到循环结束操作的指令，添加for循环出口
void GenerateCode::genForTail(InterInstruction *&t_step, InterInstruction *& t_exit) {
    symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, t_step));
    symbolTable.addInterInstruction(t_exit);
    //for循环入口和出口弹出
    popEntryAndExit();
}

void GenerateCode::genContinue() {
    //取出当前入口栈的尾部元素
    InterInstruction* entrance = entry_stack.back();
    if (entrance){
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, entrance));
    } else {
        //todo 语义错误 非循环体或switch中使用continue
    }
}

void GenerateCode::genBreak() {
    //取出当前出口栈的尾部元素
    InterInstruction* exit = exit_stack.back();
    if (exit){
        symbolTable.addInterInstruction(new InterInstruction(OP_JUMP, exit));
    } else {
        //todo 语义错误 非循环体或switch中使用break
    }
}


















