//
// Created by 95792 on 2020/4/11.
//

#ifndef COMPILER_CPP_TOKEN_H
#define COMPILER_CPP_TOKEN_H
#include "common.h"

class Token {
public:
    Tag tag;
    Token(Tag t);
    virtual string standardOutput();
    virtual ~Token();
    //获取identifier的name
    virtual string getName();
};

/******************以下为继承自token的子类**********************/
/****************标识符***********************/
class Id:public Token{
public:
    string id_name;
    Id(string s);
    string standardOutput() override;
    string getName() override ;
};

/****************数字常量***********************/
class Num:public Token{
public:
    int num_value;
    Num(int v);
    string standardOutput() override;
};

/****************字符常量***********************/
class Character:public Token{
public:
    char character_value;
    Character(char c);
    string standardOutput() override;
};

/****************字符串常量***********************/
class Str:public Token{
public:
    string string_value;
    Str(string s);
    string standardOutput() override;
};


#endif //COMPILER_CPP_TOKEN_H
