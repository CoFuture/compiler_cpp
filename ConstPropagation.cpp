//
// Created by 95792 on 2020/6/28.
//

#include "ConstPropagation.h"
#include "DataFlowGraph.h"
#include "SymbolTable.h"

/***基本运算规则
 * 1、UNDEFINED ∩ v = v
 * 2、UNCERTAIN ∩ v = UNCERTAIN
 * 3、v ∩ v = v
 * 4、c1 ∩ c2 = UNCERTAIN
 * ***/
double ConstPropagation::join(double left, double right) {
    //基本的运算规则
    if (left == UNCERTAIN || right == UNCERTAIN)
        return UNCERTAIN;
    else if (left == UNDEFINED)
        return right;
    else if (right == UNDEFINED)
        return left;
    else if (left == right)
        return left;
    else
        return UNCERTAIN;
}

//集合交汇运算 B.in= & B.prev[i].out
void ConstPropagation::join(Block *block) {
    //B.prev集合
    list<Block *> &preSet = block->precursor;
    //B.in集合
    vector<double> &inSet = block->inValues;

    //记录block的前驱个数
    int precursorCount = preSet.size();

    //for之前的判断是优化计算 ，可以删除
    //唯一前驱，取出前驱集合初始化
//    if (precursorCount == 1) {
//        inSet = preSet.front()->outValues;
//        return;
//    }
//
//    //没有前驱，直接初始化in集合
//    if (precursorCount == 0) {
//        inSet = initValues;
//        return;
//    }

    //多个前驱，处理in集合每个元素
    for (int i = 0; i < inSet.size(); ++i) {
        //记录交并结果
        double value = UNDEFINED;
        //处理每一个前驱基本块prev[j]
        for (auto &j : preSet) {
            //取出prev[j].out的每个元素与结果交并
            value = join(value, j->outValues[i]);
        }
        //更新in[i]
        inSet[i] = value;
    }
}

//指令的传递函数
//out = f(in)
void ConstPropagation::transmit(InterInstruction *instruction, vector<double> &in, vector<double> &out) {

    //获取四元式中的基本信息
    Operator opt = instruction->getOperator();
    VarElement* result = instruction->getResult();
    VarElement* arg1 = instruction->getArg1();
    VarElement* arg2 = instruction->getArg2();

//    if (arg1 && arg1->getVarName()[0] == '[')
//        return;
//
//    if (arg2 && arg2->getVarName()[0] == '[')
//        return;
//
//    if (result && result->getVarName()[0] == '[')
//        return;

    //默认直接传播
    out = in;
    if (instruction->isExpression()){
        //基本的运算表达式
        double temp;
        if (opt == OP_ASSIGN || opt == OP_NEGATIVE || opt == OP_NOT){
            //一元运算 = - ！的处理
            if (arg1->getIsConstant()){
                //常量的处理，常值，非字符串
                temp = arg1->getConstantValue();
            } else
                temp = in[arg1->list_index];

            if (temp != UNDEFINED && temp != UNCERTAIN){
                if (opt == OP_NEGATIVE)
                    temp = - temp;
                else if (opt == OP_NOT)
                    temp = ! temp;
            }
        } else if (opt >= OP_ADD && opt <= OP_OR){
            //双目运算的处理
            double left_temp;
            double right_temp;
            if (arg1->getIsConstant()){
                left_temp = arg1->getConstantValue();
            } else
                left_temp = in[arg1->list_index];

            if (arg2->getIsConstant()){
                right_temp = arg2->getConstantValue();
            } else
                right_temp = in[arg2->list_index];

            //有一个为UNCERTAIN则为 UNCERTAIN
            //有一个为UNDEFINED则为 UNDEFINED
            if (left_temp == UNCERTAIN || right_temp == UNCERTAIN)
                temp = UNCERTAIN;
            else if (left_temp == UNDEFINED || right_temp == UNDEFINED)
                temp = UNDEFINED;
            else{
                //todo 窄转换？ 全int
                int left = left_temp;
                int right = right_temp;
                if(opt == OP_ADD)
                    temp = left + right;
                else if(opt == OP_SUB)
                    temp = left - right;
                else if(opt == OP_MUL)
                    temp = left * right;
                else if(opt == OP_DIV){
                    if(!right)
                        temp = UNCERTAIN;
                    else
                        temp = left / right;
                }//除数为0,特殊处理！
                else if(opt == OP_MOD){
                    if(!right)
                        temp = UNCERTAIN;
                    else
                        temp=left % right;
                }
                else if(opt == OP_GREATER)
                    temp = left > right;
                else if(opt == OP_GREATER_EQUAL)
                    temp = left >= right;
                else if(opt == OP_LESS)
                    temp = left < right;
                else if(opt == OP_LESS_EQUAL)
                    temp = left <= right;
                else if(opt == OP_EQUAL)
                    temp = left == right;
                else if(opt == OP_NOT_EQUAL)
                    temp = left != right;
                else if(opt == OP_AND)
                    temp = left && right;
                else if(opt == OP_OR)
                    temp = left || right;
            }
        }
        else if (opt == OP_POINTER_GET)
            temp = UNCERTAIN;

        out[result->list_index] = temp;
    }
    //非基本表达式的处理
    else if (opt == OP_POINTER_SET || opt == OP_ARG && !arg1->isBasicType()){
        //破坏运算*x=y 或者 arg x没有影响，arg p->p是指针，破坏
        for (double & i : out) {
            i = UNCERTAIN;
        }
    } else if (opt == OP_FUN){
        //保守认为函数调用,破坏了全局变量,破坏了传递
        for (auto & globalVar : globalVars) {
            out[globalVar->list_index] = UNCERTAIN;
        }
    } else if (opt == OP_FUN_CALL){
        //保守认为函数调用,破坏了全局变量,保守认为函数返回值失去常量性质
        for (auto & globalVar : globalVars) {
            out[globalVar->list_index] = UNCERTAIN;
            out[result->list_index] = UNCERTAIN;
        }
    }

    //in out 拷贝到指令中
    instruction->inValues = in;
    instruction->outValues = out;
}

/*
	基本块传递函数:B.out=fB(B.in);
	out集合变化返回true
*/
bool ConstPropagation::transmit(Block *block) {
    vector<double> inSet = block->inValues;     //输入集合
    vector<double> outSet = block->outValues;   //输出集合，默认没有指令，直接传递

    //处理基本块的每一个指令
    for (auto inst : block->instructions) {
        //取出指令, 单指令传递
        transmit(inst, inSet, outSet);
        //下条指令的in为当前指令的out
        inSet = outSet;
    }

    //判断out集合是否变化
    bool flag = false;

    for (int i = 0; i < outSet.size(); ++i) {
        if (block->outValues[i] != outSet[i]) {
            flag = true;//检测到变化
            break;
        }
    }

    block->outValues = outSet;//设定out集合
    return flag;
}

//常量传播分析
void ConstPropagation::startAnalyse() {
    //初始化边界集合
    dataFlowGraph->blocks[0]->outValues = boundValues;
    for (int i=1; i < dataFlowGraph->blocks.size(); ++i) {
        //初始化，in和 out
        dataFlowGraph->blocks[i]->outValues = initValues;
        dataFlowGraph->blocks[i]->inValues = initValues;
    }
    bool outChanged = true;
    while (outChanged){
        //out集合发生变化
        outChanged = false;
        for (int i = 1; i < dataFlowGraph->blocks.size(); ++i) {
            //块交汇原酸
            join(dataFlowGraph->blocks[i]);
            //比较前后out集合是否有变化
            if (transmit(dataFlowGraph->blocks[i]))
                outChanged = true;
        }
    }
}

//常量合并和代数化简
void ConstPropagation::simplifyAlgebra() {
    //外层循环，遍历所有的指令块
    for (auto & block : dataFlowGraph->blocks) {
        for (auto inst : block->instructions) {
            //遍历可达块的所有指令
            //获取运算符
            Operator opt = inst->getOperator();
            //处理每一个可能常量传播的指令
            if (inst->isExpression()) {
                //存储临时值结果
                double rs;
                VarElement *result = inst->getResult();     //结果变量
                VarElement *arg1 = inst->getArg1();         //参数1
                VarElement *arg2 = inst->getArg2();         //参数2
                rs = inst->outValues[result->list_index];   //结果变量常量传播结果
                if (rs != UNDEFINED && rs != UNCERTAIN) {//结果为常量，替换为result=c指令

                    auto *newVar = new VarElement((int) rs);//计算的表达式结果
                    table->addVariable(newVar);//添加到符号表
                    inst->replace(OP_ASSIGN, result, newVar);//替换新的操作符与常量操作数
                } else if (opt >= OP_ADD && opt <= OP_OR &&
                           !(opt == OP_ASSIGN || opt == OP_NEGATIVE || opt == OP_NOT)) {
                    //常量传播不能处理的情况由代数化简完成
                    double lp, rp;//左右操作数
                    if (arg1->list_index == -1) {//参数不在值列表，必是常量
                        if (arg1->isBasicType())
                            lp = arg1->getConstantValue();
                    } else
                        lp = inst->inValues[arg1->list_index];//左操作数
                    if (arg2->list_index == -1) {//参数不在值列表，必是常量
                        if (arg2->isBasicType())
                            rp = arg2->getConstantValue();
                    } else
                        rp = inst->inValues[arg2->list_index];//右操作数
                    int left, right;//记录操作数值
                    bool dol = false, dor = false;//处理哪一个操作数
                    if (lp != UNDEFINED && lp != UNCERTAIN) {
                        left = lp;
                        dol = true;
                    } else if (rp != UNDEFINED && rp != UNCERTAIN) {
                        right = rp;
                        dor = true;
                    } else
                        continue;//都不是常量不进行处理！

                    VarElement *newArg1 = nullptr;//记录有效的操作数
                    VarElement *newArg2 = nullptr;//可选的操作数
                    Operator newOp = OP_ASSIGN;//化简成功后默认为赋值运算
                    if (opt == OP_ADD) {//z=0+y z=x+0
                        if (dol && left == 0) {
                            /*if(arg1->index==-1)delete arg1;*/
                            newArg1 = arg2;
                        }
                        if (dor && right == 0) {
                            newArg1 = arg1;
                            /*if(arg2->index==-1)delete arg2;*/
                        }
                    } else if (opt == OP_SUB) {//z=0-y z=x-0
                        if (dol && left == 0) {
                            /*if(arg1->index==-1)delete arg1;*/
                            newOp = OP_NEGATIVE;
                            newArg1 = arg2;
                        }
                        if (dor && right == 0) {
                            newArg1 = arg1;
                            /*if(arg2->index==-1)delete arg2;*/
                        }
                    } else if (opt == OP_MUL) {//z=0*y z=x*0 z=1*y z=x*1
                        if (dol && left == 0 || dor && right == 0) {
                            newArg1 = new VarElement(0);
                            // if(arg1->index==-1)delete arg1;
                            // if(arg2->index==-1)delete arg2;
                        }
                        if (dol && left == 1) {
                            /*if(arg1->index==-1)delete arg1;*/
                            newArg1 = arg2;
                        }
                        if (dor && right == 1) {
                            newArg1 = arg1;
                            /*if(arg2->index==-1)delete arg2;*/
                        }
                    } else if (opt == OP_DIV) {
                        //z=0/y z=x/1
                        if (dol && left == 0) {
                            /*if(arg1->index==-1)delete arg1;*/
                            newArg1 = SymbolTable::num_zero;
                        }
                        if (dor && right == 1) {
                            newArg1 = arg1;
                            /*if(arg2->index==-1)delete arg2;*/
                        }
                    } else if (opt == OP_MOD) {
                        //z=0%y z=x%1
                        if (dol && left == 0 || dor && right == 1) {
                            newArg1 = SymbolTable::num_zero;
                            // if(arg1->index==-1)delete arg1;
                            // if(arg2->index==-1)delete arg2;
                        }
                    } else if (opt == OP_AND) {//z=0&&y z=x&&0 z=1&&y z=x&&1
                        if (dol && left == 0 || dor && right == 0) {
                            newArg1 = SymbolTable::num_zero;
                            // if(arg1->index==-1)delete arg1;
                            // if(arg2->index==-1)delete arg2;
                        }
                        if (dol && left != 0) {
                            //z=y!=0
                            // if(arg1->index==-1)delete arg1;
                            newOp = OP_NOT_EQUAL;
                            newArg1 = arg2;
                            newArg2 = SymbolTable::num_zero;
                        }
                        if (dor && right != 0) {//z=x!=0
                            newOp = OP_NOT_EQUAL;
                            newArg1 = arg1;
                            newArg2 = SymbolTable::num_zero;
                            // if(arg2->index==-1)delete arg2;
                        }
                    } else if (opt == OP_OR) {
                        //z=0||y z=x||0 z=1||y z=x||1
                        if (dol && left != 0 || dor && right != 0) {
                            newArg1 = SymbolTable::num_one;
                            // if(arg1->index==-1)delete arg1;
                            // if(arg2->index==-1)delete arg2;
                        }
                        if (dol && left == 0) {
                            //z=y!=0
                            // if(arg1->index==-1)delete arg1;
                            newOp = OP_NOT_EQUAL;
                            newArg1 = arg2;
                            newArg2 = SymbolTable::num_zero;
                        }
                        if (dor && right == 0) {
                            //z=x!=0
                            newOp = OP_NOT_EQUAL;
                            newArg1 = arg1;
                            newArg2 = SymbolTable::num_zero;
                            // if(arg2->index==-1)delete arg2;
                        }
                    }
                    if (newArg1) {//代数化简成功！
                        inst->replace(newOp, result, newArg1, newArg2);//更换新的指令
                    } else {//没法化简，正常传播
                        if (dol) {//传播左操作数
                            newArg1 = new VarElement(left);
                            table->addVariable(newArg1);
                            newArg2 = arg2;
                        } else if (dor) {//传播右操作数
                            newArg2 = new VarElement(right);
                            table->addVariable(newArg2);
                            newArg1 = arg1;
                        }
                        inst->replace(opt, result, newArg1, newArg2);//更换传播的新的指令
                    }
                }
            } else if (opt == OP_ARG || opt == OP_RETURN_VALUE) {
                //将参数或者返回值取出
                VarElement *arg1 = inst->getArg1();
                if (arg1->list_index != -1) {//不是常数
                    double rs = inst->outValues[arg1->list_index];//结果变量常量传播结果
                    if (rs != UNDEFINED && rs != UNCERTAIN) {
                        //为常量
                        auto *newVar = new VarElement((int) rs);//计算的表达式结果
                        //添加到符号表
                        table->addVariable(newVar);
                        //替换新的操作符与常量操作数
                        inst->setArg1(newVar);
                    }
                }
            }
        }
    }
}

void ConstPropagation::optimizeConditionJump() {
//dfg->toString();
    for (int j = 0; j < dataFlowGraph->blocks.size(); ++j) {
        /*if(!dfg->reachable(dfg->blocks[j])){//不可达
            dfg->release(dfg->blocks[j]);//不可达块消除后继
            //dfg->blocks[j]->toString();
            continue;//不进行优化
        }*/
        //dfg->blocks[j]->toString();
        for (auto i = dataFlowGraph->blocks[j]->instructions.begin(), k = i;
             i != dataFlowGraph->blocks[j]->instructions.end(); i = k) {//遍历可达块的所有指令
            ++k;//记录下一个迭代器，防止遍历时发生指令删除
            InterInstruction *inst = *i;
            if (inst->isJumpCondition()) {//处理每一个可能常量传播的条件跳转指令，无条件跳转指令不用关心
                Operator op = inst->getOperator();//获取运算符
                InterInstruction *tar = inst->getJumpTarget();//目标位置
                VarElement *arg1 = inst->getArg1();//参数1
                double cond;//操作数

                //将jump的condition取出
                if (arg1->getIsConstant()) {//参数不在值列表，必是常量
                    cond = arg1->getConstantValue();
                } else
                    cond = inst->inValues[arg1->list_index];//条件值

                //非常量条件不做处理
                if (cond == UNCERTAIN || cond == UNDEFINED)
                    continue;

                //处理true和false两种跳转
                if (op == OP_JUMP_TRUE) {
                    if (cond == 0) {
                        //condition为false，删除跳转指令，解除基本块关联
                        inst->block->instructions.remove(inst);                     //删除跳转指令
                        if (dataFlowGraph->blocks[j + 1] != tar->block)             //目标块不是紧跟块
                            dataFlowGraph->deleteLink(inst->block, tar->block);     //解除基本块关联
                    } else {
                        //条件始终为真，改为无条件跳转指令
                        inst->replace(OP_JUMP, tar);
                        if (dataFlowGraph->blocks[j + 1] != tar->block)//目标块不是紧跟块
                            dataFlowGraph->deleteLink(inst->block, dataFlowGraph->blocks[j + 1]);//从DFG内递归解除指令所在块到紧跟块的关联
                    }
                } else if (op == OP_JUMP_FALSE) {
                    if (cond == 0) {
                        //condition为false，改为无条件跳转指令
                        inst->replace(OP_JUMP, tar);//无条件跳转指令
                        if (dataFlowGraph->blocks[j + 1] != tar->block)//目标块不是紧跟块
                            dataFlowGraph->deleteLink(inst->block, dataFlowGraph->blocks[j + 1]);//从DFG内递归解除指令所在块到紧跟块的关联
                    } else {
                        //condition为true，删除跳转指令，解除基本块关联
                        inst->block->instructions.remove(inst);                 //删除跳转指令
                        if (dataFlowGraph->blocks[j + 1] != tar->block)         //目标块不是紧跟块
                            dataFlowGraph->deleteLink(inst->block, tar->block); //解除基本块关联
                    }
                }
            }
        }
    }
}

ConstPropagation::ConstPropagation(SymbolTable *t, DataFlowGraph *dfg, vector<VarElement *> &paraVars):dataFlowGraph(dfg), table(t) {
    //全局变量
    vector<VarElement*> global = t->getGlobalVariables();
    int index = 0;
    for (auto var : global) {
        var->list_index = index ++;
        vars.push_back(var);//将被赋值的变量加入值集合
        //不存在浮点变量，因此可以用特殊浮点数区分UNDEF与NAC
        double value = UNDEFINED;//默认变量都是UNDEF
        if(!var->isBasicType())
            //非基本类型
            value = UNCERTAIN;
        else if(var->getIsInited())
            //初始化的基本类型变量为const
            value = var->getConstantValue();
        //添加边界值
        boundValues.push_back(value);
    }
    //参数变量 初始值为UNCERTAIN
    for (auto var : paraVars) {
        //获取变量
        //记录变量在列表中的位置
        var->list_index = index++;
        //将变量添加到集合中
        vars.push_back(var);
        //添加边界值
        boundValues.push_back(UNCERTAIN);
    }
    //局部变量
    for (auto & code : dataFlowGraph->interCodeList) {
        //局部变量声明
        if (code->isDeclare()){
            //获取声明的局部变量
            VarElement* var = code->getArg1();
            var->list_index = index++;
            vars.push_back(var);
            //默认变量初始值均为UNDEFINED
            double value = UNDEFINED;
            if (!var->isBasicType())
                value = UNCERTAIN;
            else if (var->getIsInited())
                //已经初始化的，获取初始化值
                value = var->getConstantValue();
            boundValues.push_back(value);
        }
//        else {
//            VarElement* result = code->getResult();
//            VarElement* arg1 = code->getArg1();
//            VarElement* arg2 = code->getArg2();
//            if (result && result->getVarName()[0] == '['){
//                result->list_index = index ++;
//                vars.push_back(result);
//                double value = UNDEFINED;
//                if (!result->isBasicType())
//                    value = UNCERTAIN;
//                else if (result->getIsInited())
//                    //已经初始化的，获取初始化值
//                    value = result->getConstantValue();
//                boundValues.push_back(value);
//            }
//            if (arg1 && arg1->getVarName()[0] == '['){
//                arg1->list_index = index ++;
//                vars.push_back(arg1);
//                double value = UNDEFINED;
//                if (!arg1->isBasicType())
//                    value = UNCERTAIN;
//                else if (arg1->getIsInited())
//                    //已经初始化的，获取初始化值
//                    value = arg1->getConstantValue();
//                boundValues.push_back(value);
//            }
//            if (arg2 && arg2->getVarName()[0] == '['){
//                arg2->list_index = index ++;
//                vars.push_back(arg2);
//                double value = UNDEFINED;
//                if (!arg2->isBasicType())
//                    value = UNCERTAIN;
//                else if (arg2->getIsInited())
//                    //已经初始化的，获取初始化值
//                    value = arg2->getConstantValue();
//                boundValues.push_back(value);
//            }
//        }
    }

    //初始值的初始化
    while (index--)
        initValues.push_back(UNDEFINED);
}

void ConstPropagation::optimize() {
    //常量传播分析
    startAnalyse();
    //代数化简
    simplifyAlgebra();
    //不可达代码消除
    optimizeConditionJump();
}
