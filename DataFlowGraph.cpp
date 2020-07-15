//
// Created by 95792 on 2020/6/28.
//

#include "DataFlowGraph.h"
#include "IntermediateCode.h"

Block::Block(vector<InterInstruction *> &codes) {
    visited = false;
    reachable = true;
    for (auto & code : codes) {
        //将指令的所在块指向自身
        code->block = this;
        //将指令记录到list中
        instructions.push_back(code);
    }
}

void Block::showInformation() {
    //todo implementation
    cout << "block show information" << endl;
}

void DataFlowGraph::createBlocks() {
    //临时列表，用来保存块中指令
    vector<InterInstruction*> tempList;
    tempList.push_back(interCodeList[0]);
    //遍历interCodeList
    for (int i=1; i < interCodeList.size(); ++i) {
        //如果是首指令，则将原block保存，并清空tempList
        if (interCodeList[i]->isFirst()){
            auto* tempBlock = new Block(tempList);
            blocks.push_back(tempBlock);
            //清空原tempList
            tempList.clear();
            tempList.push_back(interCodeList[i]);
        } else
            //不是首指令，添加到tempList中
            tempList.push_back(interCodeList[i]);
    }
    //添加OP_EXIT
    blocks.push_back(new Block(tempList));
}

//链接基本快
void DataFlowGraph::linkBlocks() {
    for (int i = 0; i < blocks.size(); ++i) {
        //获取block尾部的指令
        InterInstruction* last = blocks[i]->instructions.back();
        //如果是跳转的情况
        if (last->isJump() || last->isJumpCondition()){
            //跳转目标基本快
            Block* target = last->getJumpTarget()->block;
            //当前块的后继为目标块
            blocks[i]->successor.push_back(target);
            //目标块的前趋为当前块
            target->precursor.push_back(blocks[i]);
        }
        //非强制跳转，可能顺序执行的情况
        if (!last->isJump()){
            if (i != (blocks.size() - 1)){
                blocks[i]->successor.push_back(blocks[i + 1]);
                blocks[i + 1]->precursor.push_back(blocks[i]);
            } else {
                blocks[i]->precursor.push_back(blocks[i - 1]);
            }
        }
    }
}

//重置访问标志
void DataFlowGraph::resetVisit() {
    for (auto & block : blocks) {
        block->visited = false;
    }
}

//递归函数，逆向检查block能否到达出口
//调用之前需要重置访问标志
bool DataFlowGraph::blockReachable(Block *block) {
    //递归终止条件，block为入口entrance
    if (block == blocks[0])
        return true;
    else if (block->visited)
        return false;

    //将block设定为访问过
    block->visited = true;
    //入口可达标志
    bool entranceReached = false;
    for (auto preBlock : block->precursor){
        entranceReached = blockReachable(preBlock);
        if (entranceReached)
            break;
    }

    return entranceReached;
}

void DataFlowGraph::releaseBlock(Block *block) {
    if (!blockReachable(block)){
        //不可达的块
        list<Block*> deleteList;
        //记录所有后继
        for(auto & item : block->successor){
            deleteList.push_back(item);
        }
        //删除当前块与后继之间的关系
        for(auto & item : deleteList){
            block->successor.remove(item);
            item->precursor.remove(block);
        }
        //递归处理block的后继基本块
        for(auto & item : deleteList){
            releaseBlock(item);
        }
    }
}

DataFlowGraph::DataFlowGraph(IntermediateCode& code) {
    //标识首指令
    code.markFirst();
    //复制中间代码
    interCodeList = code.getIntermediateCode();
    //创建基本块
    createBlocks();
    //链接基本块
    linkBlocks();
}

DataFlowGraph::~DataFlowGraph() {
    for (auto & block : blocks) {
        delete block;
    }

    for (auto & code : optimizedCode) {
        delete code;
    }
}

//删除块之间的联系
void DataFlowGraph::deleteLink(Block *begin, Block *end) {
    //先重置访问标志, 方便之后的release
    resetVisit();
    if (begin){
        begin->successor.remove(end);
        end->precursor.remove(begin);
    }
    releaseBlock(end);
}

//将数据流图导出为中间代码
void DataFlowGraph::toInterCode(list<InterInstruction *> &optimized) {
    //清空原优化指令
    optimized.clear();
    for (auto & block : blocks) {
        resetVisit();
        if (blockReachable(block)){
            list<InterInstruction*> tempInstructions;
            for (auto & instruction : block->instructions) {
                //添加指令
                tempInstructions.push_back(instruction);
            }
            //将块合并
            optimized.splice(optimized.end(), tempInstructions);
        } else
            //记录块不可达
            block->reachable = false;
    }
}

void DataFlowGraph::showInformation() {
    for (auto & block : blocks) {
        block->showInformation();
    }
}








