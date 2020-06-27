//
// Created by 95792 on 2020/4/9.
//

#ifndef COMPILER_CPP_COMMON_H
#define COMPILER_CPP_COMMON_H
#include <string>
#include <iostream>
#include <ext/hash_map>
#include <list>

using namespace __gnu_cxx;
using namespace std;
//词法记号标记
enum Tag{
    //特殊类型err表示错误，end表示文件结束
    ERR,
    FILE_END,
    //标识符ID
    IDENTIFIER,
    //常量
    CONSTANT_NUM,
    CONSTANT_CHAR,
    CONSTANT_STRING,
    //函数和IO关键字
    KW_MAIN,// main
    KW_SCANF,// scanf
    KW_PRINTF,// printf
    //数据类型关键词
    KW_INT,
    KW_CHAR,
    KW_FLOAT,
    KW_DOUBLE,
    KW_BOOL,
    KW_CONST,
    KW_VOID,
    KW_EXTERN,
    //控制结构关键词
    KW_IF,
    KW_ELSE,
    KW_SWITCH,
    KW_CASE,
    KW_DEFAULT,
    KW_WHILE,
    KW_FOR,
    KW_DO,
    KW_BREAK,
    KW_CONTINUE,
    KW_RETURN,
    //界限符号-运算符号 + - * /等
    DELIMITER_ADD,// +
    DELIMITER_SUB,// -
    DELIMITER_MUL,// *
    DELIMITER_DIV,// /
    DELIMITER_MOD,// %
    DELIMITER_INC,// ++
    DELIMITER_DEC,// --
    DELIMITER_NOT,// !
    DELIMITER_LEA,// &
    DELIMITER_AND,// &&
    DELIMITER_OR,// ||
    DELIMITER_ASSIGN,// =
    DELIMITER_GREATER,// >
    DELIMITER_GREATER_EQUAL,// >=
    DELIMITER_LESS,// <
    DELIMITER_LESS_EQUAL,// <=
    DELIMITER_EQUAL,// ==
    DELIMITER_NOT_EQUAL,// !=
    //界限符号-特殊的
    DELIMITER_COMMA,// ,
    DELIMITER_COLON,// :
    DELIMITER_SEMICOLON,// ;
    DELIMITER_LEFT_BRACE,// {
    DELIMITER_RIGHT_BRACE,// }
    DELIMITER_LEFT_PAREN,// (
    DELIMITER_RIGHT_PAREN,// )
    DELIMITER_LEFT_BRACKET,// [
    DELIMITER_RIGHT_BRACKET// ]
};



//四元式操作符号
enum Operator{
    OP_NOP, //空操作符
    OP_LABEL,   //标签指令
    OP_FUN_ENTRY, OP_FUN_EXIT, //函数入口和函数出口
    OP_DECLARE,  //变量或者函数声明
    OP_ASSIGN,  //赋值=
    OP_ADD, OP_SUB, OP_MUL, OP_DIV, // + - * / 运算
    OP_MOD,  //取模运算
    OP_NEGATIVE,  //取负运算
    OP_GREATER, OP_GREATER_EQUAL, OP_LESS, OP_LESS_EQUAL, OP_EQUAL, OP_NOT_EQUAL,  // > >= < <= == != 比较判断符号
    OP_INC, OP_DEC, //++ --
    OP_NOT, OP_AND, OP_OR,  // ! && || 与或非运算符
    OP_LEA, OP_POINTER_SET, OP_POINTER_GET, // 取地址，指针赋值， 指针寻值
    OP_JUMP, //跳转
    OP_JUMP_TRUE, OP_JUMP_FALSE, // TRUE条件跳转 FALSE条件跳转
    OP_JUMP_NOT_EQUAL, //不相等跳转
    OP_ARG, OP_FUN, OP_FUN_CALL,  //函数参数， 函数调用, 函数调用并赋值
    OP_RETURN, OP_RETURN_VALUE  // 无返回值return 有返回值return
};

//类的初始化声明
//class Scanner;
//class Lexer;
//class Token;
//class SymbolTable;

#endif //COMPILER_CPP_COMMON_H
