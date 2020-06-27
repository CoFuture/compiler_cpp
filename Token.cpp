//
// Created by 95792 on 2020/4/11.
//

#include "Token.h"
#include <sstream>

/*****定义的字符串输出，将tag枚举值转化为对应的字符串*****/
const char * token_string[] = {
        //特殊符号，错误和文件结束
        "error",
        "file_end",
        //变量id
        "identifier",
        //常量 数字，字符，字符串
        "constant_num",
        "constant_char",
        "constant_string",
        //关键保留字
        "main",
        "scanf",
        "printf",
        "int",
        "char",
        "float",
        "double",
        "bool",
        "const",
        "void",
        "extern",
        //控制结构关键字
        "if",
        "else",
        "switch",
        "case",
        "default",
        "while",
        "for",
        "do",
        "break",
        "continue",
        "return",
        //界限符号-运算符和判断符
        "+",
        "-",
        "*",
        "/",
        "%",
        "++",
        "--",
        "!",
        "&",
        "&&",
        "||",
        "=",
        ">",
        ">=",
        "<",
        "<=",
        "==",
        "!=",
        //界限符号特殊的
        ",",
        ":",
        ";",
        "{",
        "}",
        "(",
        ")",
        "[",
        "]"
};

Token::Token(Tag t){
    tag = t;
}

Token::~Token() = default;

string Token::standardOutput() {
    //返回tag对应的string值
    return token_string[tag];
}

string Token::getName() {
    return token_string[tag];
}

/****标识符函数的实现****/

Id::Id(string s):Token(IDENTIFIER){
    id_name = s;
}

string Id::standardOutput() {
    return Token::standardOutput() + ":" + id_name;
}

//获取变量名称
string Id::getName() {
    return id_name;
}

/***********数字************/
Num::Num(int n):Token(CONSTANT_NUM){
    num_value = n;
}

string Num::standardOutput() {
    //这里使用了输入输出流来方便将int类型值与字符串合并输出
    stringstream outputValue;
    outputValue << num_value;
    return Token::standardOutput() + ":" + outputValue.str();
}

/***********字符类型***********/
Character::Character(char c):Token(CONSTANT_CHAR){
    character_value = c;
}

string Character::standardOutput() {
    stringstream outputValue;
    outputValue << character_value;
    return Token::standardOutput() + ":" + outputValue.str();
}

/*******字符串*******/
Str::Str(string s):Token(CONSTANT_STRING){
    string_value = s;
}

string Str::standardOutput() {
    return Token::standardOutput() + ":" + string_value;
}

