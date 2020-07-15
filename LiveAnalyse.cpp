//
// Created by 95792 on 2020/7/4.
//

#include "LiveAnalyse.h"
#include "SymbolTable.h"
#include "IntermediateCode.h"
#include "VarElement.h"
#include "DataFlowGraph.h"

bool LiveAnalyse::transmit(Block *block) {
    Set temp = block->liveInfo.out;
    for (auto i = block->instructions.rbegin(); i != block->instructions.rend(); ++i) {
        InterInstruction *instruction = *i;
        if (instruction->isDead)
            continue;
        //初始化指令的in 和 out集合
        Set &in = instruction->liveInfo.in;
        Set &out = instruction->liveInfo.out;
        out = temp;
        //传递函数
        in = instruction->liveInfo.use | (out - instruction->liveInfo.def);
        temp = in;
    }
    //检查活跃变量集合是否变化
    bool flag = (temp != block->liveInfo.in);
    block->liveInfo.in = temp;
    return flag;
}

LiveAnalyse::LiveAnalyse(DataFlowGraph *graph, SymbolTable *t, vector<VarElement *> &paraVar) : dataFlowGraph(graph),
                                                                                                table(t) {
    //获取符号表全局变量保存到varList中
    varList = table->getGlobalVariables();
    int globalVarCount = varList.size();
    //将函数的参数变量添加到变量表中
    for (auto &var : paraVar) {
        varList.push_back(var);
    }
    //将上一步优化后的中间代码保存到本地
    dataFlowGraph->toInterCode(optimizedCode);

    //获取中间代码中声明的变量
    for (auto &instruction : optimizedCode) {
        Operator opt = instruction->getOperator();
        if (opt == OP_DECLARE) {
            VarElement *var_dec = instruction->getArg1();
            varList.push_back(var_dec);
        }
    }

    //初始化集合
    U.init(varList.size(), true);
    E.init(varList.size(), false);
    G = E;

    //全局变量的设置
    for (int i = 0; i < globalVarCount; ++i) {
        G.set(i);
    }

    for (int i = 0; i < varList.size(); ++i) {
        varList[i]->list_index = i;
    }

    for (auto &instruction : optimizedCode) {
        instruction->liveInfo.use = E;
        instruction->liveInfo.def = E;
        VarElement *result = instruction->getResult();
        Operator opt = instruction->getOperator();
        VarElement *arg1 = instruction->getArg1();
        VarElement *arg2 = instruction->getArg2();
        if (opt >= OP_ASSIGN && opt <= OP_LEA) {
            instruction->liveInfo.use.set(arg1->list_index);
            if (arg2) {
                instruction->liveInfo.use.set(arg2->list_index);
            }
            if (result != arg1 && result != arg2) {
                instruction->liveInfo.def.set(result->list_index);
            }
        } else if (opt == OP_POINTER_SET)
            instruction->liveInfo.use.set(result->list_index);
        else if (opt == OP_POINTER_GET)
            instruction->liveInfo.use = U;
        else if (opt == OP_RETURN_VALUE)
            instruction->liveInfo.use.set(arg1->list_index);
        else if (opt == OP_ARG) {
            if (arg1->isBasicType())
                instruction->liveInfo.use.set(arg1->list_index);
            else
                instruction->liveInfo.use = U;
        } else if (opt == OP_FUN_CALL || opt == OP_FUN) {
            instruction->liveInfo.use = G;
            if (result && (result->getScopePath().size() > 1))
                instruction->liveInfo.def.set(result->list_index);
        } else if (opt == OP_JUMP_TRUE || opt == OP_JUMP_FALSE)
            instruction->liveInfo.use.set(arg1->list_index);
    }
}

void LiveAnalyse::startAnalyse() {
    int blockSize = dataFlowGraph->blocks.size();
    dataFlowGraph->blocks[blockSize - 1]->liveInfo.in = E;
    for (int i = 0; i < blockSize - 1; ++i) {
        dataFlowGraph->blocks[i]->liveInfo.in = E;
    }
    bool blockChanged = true;
    while (blockChanged) {
        blockChanged = false;
        for (int i = blockSize - 2; i >= 0; --i) {
            if (!dataFlowGraph->blocks[i]->reachable)
                continue;
            else {
                Set temp = E;
                list<Block *>::iterator j;
                for (j = dataFlowGraph->blocks[i]->successor.begin();
                     j != dataFlowGraph->blocks[i]->successor.end(); ++j) {
                    temp = temp | (*j)->liveInfo.in;
                }
                dataFlowGraph->blocks[i]->liveInfo.out = temp;
                if (transmit(dataFlowGraph->blocks[i]))
                    blockChanged = true;
            }
        }
    }
}

void LiveAnalyse::removeDeadCode(int stop) {
    if (stop) {
        for (auto &code:optimizedCode) {
            Operator opt = code->getOperator();
            if (code->isDead || opt == OP_DECLARE)
                continue;
            VarElement *result = code->getResult();
            VarElement *arg1 = code->getArg1();
            VarElement *arg2 = code->getArg2();
            if (result)
                result->live = true;
            if (arg1)
                arg1->live = true;
            if (arg2)
                arg2->live = true;
        }
        for (auto &code:optimizedCode) {
            Operator opt = code->getOperator();
            if (opt == OP_FUN) {
                VarElement *arg1 = code->getArg1();
                if (arg1 && !arg1->live)
                    code->isDead = true;
            }
        }
        return;
    }

    stop = true;
    //开始活跃性分析
    startAnalyse();
    for (auto &code : optimizedCode) {
        if (code->isDead)
            continue;
        VarElement* result = code->getResult();
        Operator opt = code->getOperator();
        VarElement* arg1 = code->getArg1();
        VarElement* arg2 = code->getArg2();

        if (opt >= OP_ASSIGN && opt <= OP_LEA || opt == OP_POINTER_GET){
            //全局变量不处理
            if (result->getScopePath().size() == 1)
                continue;
            if (!code->liveInfo.out.get(result->list_index) || (opt == OP_ASSIGN && result == arg1)){
                code->isDead = true;
                stop = false;
            }
        } else if (opt == OP_FUN_CALL){
            if (!code->liveInfo.out.get(result->list_index))
                //转为普通的函数调用，无返回值
                code->toSimpleCall();
        }
    }

    //递归
    removeDeadCode(stop);
}

Set &LiveAnalyse::getE() {
    return E;
}

//根据提供的live out集合提取优化后的变量集合——冲突变量
vector<VarElement *> LiveAnalyse::getCoVar(Set liveOut) {
    vector<VarElement*> conflictVar;
    for (int i = 0; i < varList.size(); ++i) {
        if (liveOut.get(i)){
            //保存活跃的变量
            conflictVar.push_back(varList[i]);
        }
    }
    return conflictVar;
}
