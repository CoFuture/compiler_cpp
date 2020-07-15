//
// Created by 95792 on 2020/7/4.
//

#ifndef COMPILER_CPP_SET_H
#define COMPILER_CPP_SET_H

#include "common.h"

class Set {
    //集合中的元素
    vector<unsigned int> setList;
public:
    int count;

    //构造与初始化
    Set();                          //无参的集合构造
    Set(int size,bool val);         //构造一个容纳size个元素的集合，并初始化
    void init(int size,bool val);
    void p();                       //调试输出函数

    //集合基本运算
    Set operator &(Set val);//交集运算
    Set operator |(Set val);//并集运算
    Set operator -(Set val);//差集运算
    Set operator ^(Set val);//异或运算
    Set operator ~();//补集运算
    bool operator ==(Set& val);//比较运算
    bool operator !=(Set& val);//比较运算
    bool get(int i);//索引运算
    void set(int i);//置位运算
    void reset(int i);//复位运算
    int max();//返回最高位的1的索引

    //判断集合是否为空集
    bool isEmptySet();

};

/*
	活跃变量的数据流信息
*/
struct LiveInfo
{
    Set in;//输入集合
    Set out;//输出集合
    Set use;//使用变量集合——变量的使用先与定值
    Set def;//定值变量集合——变量的定值先与使用
};


#endif //COMPILER_CPP_SET_H
