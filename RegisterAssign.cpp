//
// Created by 95792 on 2020/7/8.
//

#include "RegisterAssign.h"
#include "VarElement.h"
#include "IntermediateCode.h"
#include "LiveAnalyse.h"

Node::Node(VarElement *v, Set &e) {
    this->var = v;
    this->degree = 0;
    this->nodeColor = -1;
    //排斥集合默认为空集
    excludedColor = e;
}

void Node::addLink(Node *node) {
    auto position = lower_bound(links.begin(), links.end(), node);
    if (position == links.end() || *position != node){
        //当前冲突没有找到要求的节点，插入
        links.insert(position, node);
        degree ++;
    }
}

void Node::addExcludedColor(int color) {
    //degree = -1为节点已经着色（失败）标志
    if (degree == -1)
        return;
    //添加排斥颜色
    excludedColor.set(color);
    //添加排斥色，说明冲突节点中有一个已经着色，度数减一
//    degree --;
}

void Node::paint(Set &colorBox) {
    //可用颜色集合
    Set availableColor = colorBox - excludedColor;
    int count = availableColor.count;
    if (count > 0){
        //存在可用颜色
        for (int i = 0; i < count; ++i) {
            if (availableColor.get(i)){
                nodeColor = i;
                var->register_id = nodeColor;
                degree--;
                //为相邻节点添加排斥颜色
                for (auto & link : links) {
                    link->addExcludedColor(nodeColor);
                }
                return;
            }
        }
    } else
        //无可用颜色，着色失败
        degree = -1;
}

ConflictGraph::ConflictGraph(list<InterInstruction *> &optCode, vector<VarElement *> &paraVar, LiveAnalyse *live,
                             FunElement *f) {
    this->function = f;
    this->optimizedCode = optCode;
    this->liveAnalyse = live;
    //初始集合 全集和空集的初始化
    U.init(REGISTER_COUNT, true);
    E.init(REGISTER_COUNT, false);

    //将函数参数变量放入列表中
    for (auto & i : paraVar) {
        varList.push_back(i);
    }
    //将局部变量添加到列表中
    for (auto & code : optimizedCode) {
        Operator opt = code->getOperator();
        if (opt == OP_DECLARE){
            VarElement* arg1 = code->getArg1();
            varList.push_back(arg1);
        }
        //取地址指令比较特殊，变量在内存中分配
        if (opt == OP_LEA){
            VarElement* arg1 = code->getArg1();
            if (arg1)
                arg1->in_memory = true;
        }
    }
    //活跃变量分析时的空集合
    Set& liveE = liveAnalyse->getE();
    Set mask = liveE;
    for (auto & i : varList) {
        mask.set(i->list_index);
    }
    //构建冲突图
    for (int i = 0; i < varList.size(); ++i) {
        Node* node;
        if (varList[i]->getIsArray() || varList[i]->in_memory)
            node = new Node(varList[i], U);
        else
            node = new Node(varList[i], E);

        //修改变量索引，将节点添加到节点向量中
        varList[i]->list_index = i;
        nodes.push_back(node);
    }

    Set buffer = liveE;
    list<InterInstruction*>::reverse_iterator iter;
    for (iter = optimizedCode.rbegin(); iter != optimizedCode.rend(); ++iter) {
        Set& liveOut = (*iter)->liveInfo.out;
        if (liveOut != buffer){
            //存在新的冲突关系
            buffer = liveOut;
            //使用掩码过滤冲突变量集合
            vector<VarElement*> conflictVar = liveAnalyse->getCoVar(liveOut & mask);
            for (int i = 0; i < (int)conflictVar.size() - 1; i++) {
                for (int j = i + 1; j < conflictVar.size(); j++) {
                    nodes[conflictVar[i]->list_index]->addLink(nodes[conflictVar[j]->list_index]);
                    nodes[conflictVar[j]->list_index]->addLink(nodes[conflictVar[i]->list_index]);
                }
            }
        }
    }
}

void ConflictGraph::registerAssign() {
    Set& colorBox = U;
    int nodeCount = nodes.size();
    for (int i = 0; i < nodeCount; ++i) {
        Node* node = pickNode();
        node->paint(colorBox);
    }
}

Node *ConflictGraph::pickNode() {
    //堆排序选择最大节点
    make_heap(nodes.begin(), nodes.end(), nodeCompare());
    Node* node = nodes.front();
    return node;
}
