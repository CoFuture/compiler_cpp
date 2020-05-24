//
// Created by 95792 on 2020/4/9.
//

#ifndef COMPILER_CPP_LEXER_H
#define COMPILER_CPP_LEXER_H

#include "common.h"
#include "Token.h"
#include <ext/hash_map>
//为了使用__gnu_cxx中的hash表
using namespace __gnu_cxx;

/***************************************************/
/****************关键词类****************************/
/***************************************************/
class Keywords{
    //hash函数这一部分使用的是系统定义的字符串hash函数
    struct hash_string{
        size_t operator()(const string& str) const {
            return __stl_hash_string(str.c_str());
        }
    };
    //hash表 index为string类型 value为定义的Tag类型
    hash_map<string, Tag, hash_string> keywords;
public:
    Keywords();
    //查询hash表,返回string对应的hash值，判断是关键字还是ID
    Tag getTag(string tokenName);
};



/***************************************************/
/****************扫描器类****************************/
/***************************************************/
//扫描器类
class Scanner{
    //文件名字和文件指针
    const char* file_name;
    FILE* file;
    //扫描器的初识参数
    static const int BUFFER_LENGTH = 80;
    char buffer[BUFFER_LENGTH];
    int line_length;
    int read_position;
    char pre_char;
    //行号和列号
    int row_number;
    int col_number;

public:
    //构造函数与析构函数
    Scanner(const char* fileName);
    ~Scanner();
    //定义的扫描函数
    char scan();
    //显示读取的字符
//    void showChar(char ch);
    const char* getFileName();
    int getRowNumber();
    int getColumnNumber();
};

/***************************************************/
/****************词法分析器类****************************/
/***************************************************/

class Lexer {
    //关键词的hash列表
    static Keywords keywords;
    //扫描器
    Scanner &scanner;
    //读入的字符
    char character;
    //字符的扫描与匹配
    bool scan(char need = 0);
    //记录扫描的词法记号
    Token *token;

public:
    Lexer(Scanner &sc);
    ~Lexer();
    /****用来识别出的token*****/
    Token* getToken();
};

#endif //COMPILER_CPP_LEXER_H
