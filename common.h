//
// Created by 95792 on 2020/4/9.
//

#ifndef COMPILER_CPP_COMMON_H
#define COMPILER_CPP_COMMON_H
#include <string>
#include <iostream>

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

//类的初始化声明
//class Scanner;
//class Lexer;
//class Token;
//class SymbolTable;

#endif //COMPILER_CPP_COMMON_H
