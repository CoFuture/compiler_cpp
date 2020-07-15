//
// Created by 95792 on 2020/7/4.
//

#include "Set.h"

bool Set::isEmptySet() {
    return setList.empty();
}

/*
	构造函数
*/
Set::Set() {
    count = 0;
}

Set::Set(int size, bool val) {
    init(size, val);
}

/*
	构造一个容纳size个元素的集合，并初始化
*/
void Set::init(int size, bool val) {
    count = size;//记录大小
    size = size / 32 + (size % 32 != 0);//计算需要多少个32位位图
    unsigned int v = val ? 0xffffffff : 0;
    for (int i = 0; i < size; i++)setList.push_back(v);//初始化位图
}

/*
	交集运算
*/
Set Set::operator&(Set val) {
    Set ret(count, 0);
    for (unsigned int i = 0; i < setList.size(); i++) {
        ret.setList[i] = setList[i] & val.setList[i];
    }
    return ret;
}

/*
	并集运算
*/
Set Set::operator|(Set val) {
    Set ret(count, 0);
    for (unsigned int i = 0; i < setList.size(); i++) {
        ret.setList[i] = setList[i] | val.setList[i];
    }
    return ret;
}

/*
	差集运算
*/
Set Set::operator-(Set val) {
    Set ret(count, 0);
    for (unsigned int i = 0; i < setList.size(); i++) {
        ret.setList[i] = setList[i] & ~val.setList[i];
    }
    return ret;
}

/*
	异或运算
*/
Set Set::operator^(Set val) {
    Set ret(count, 0);
    for (unsigned int i = 0; i < setList.size(); i++) {
        ret.setList[i] = setList[i] ^ val.setList[i];
    }
    return ret;
}

/*
	补集运算
*/
Set Set::operator~() {
    Set ret(count, 0);
    for (unsigned int i = 0; i < setList.size(); i++) {
        ret.setList[i] = ~setList[i];
    }
    return ret;
}

/*
	比较运算
*/
bool Set::operator==(Set &val) {
    if (count != val.count)
        return false;
    for (unsigned int i = 0; i < setList.size(); i++) {
        if (setList[i] != val.setList[i])
            return false;
    }
    return true;
}

/*
	比较运算
*/
bool Set::operator!=(Set &val) {
    if (count != val.count)
        return true;
    for (unsigned int i = 0; i < setList.size(); i++) {
        if (setList[i] != val.setList[i])
            return true;
    }
    return false;
}


/*
	索引运算
*/
bool Set::get(int i) {
    if (i < 0 || i >= count) {
        //printf("集合索引失败！\n");
        return false;
    }
    return !!(setList[i / 32] & (1 << (i % 32)));
}

/*
	置位运算
*/
void Set::set(int i) {
    if (i < 0 || i >= count) {
        //printf("集合索引失败！\n");
        return;
    }
    setList[i / 32] |= (1 << (i % 32));
}

/*
	复位运算
*/
void Set::reset(int i) {
    if (i < 0 || i >= count) {
        //printf("集合索引失败！\n");
        return;
    }
    setList[i / 32] &= ~(1 << (i % 32));
}

/*
	返回最高位的1的索引
*/
int Set::max() {
    for (int i = setList.size() - 1; i >= 0; --i) {
        unsigned int n = 0x80000000;
        int index = 31;
        while (n) {
            if (setList[i] & n)break;
            --index;
            n >>= 1;
        }
        if (index >= 0) {//出现1
            return i * 32 + index;
        }
    }
    return -1;
}

/*
	调试输出函数
*/
void Set::p() {
    int num = count;//计数器
    // if(bmList.size()==0){
    // 	while(num--)printf("- ");
    // }
    for (unsigned int i : setList) {
        unsigned int n = 0x1;
        while (n) {
            printf("%d ", (i & n) != 0);
            if (!--num)break;
            n <<= 1;
        }
        if (!--num)break;
    }
    fflush(stdout);
}