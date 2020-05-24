//
// Created by 95792 on 2020/4/17.
//

#ifndef COMPILER_CPP_SYNTACTIC_PARSER_H
#define COMPILER_CPP_SYNTACTIC_PARSER_H

#include "common.h"
#include "Lexer.h"
#include "SymbolTable.h"
/*****以下是文法的定义******/
//<程序> -> <程序片段><程序> | 空
//<程序片段> -> <显式标志Extern><类型><声明定义>
//<类型> -> KW_INT | KW_CHAR | KW_BOOL | KW_VOID
/***提取左公因式前***/
//<声明定义> -> <变量声明/定义> | <函数声明/定义>
/***提取左公因式前***/
//<变量声明/定义> -> <单变量声明/定义><多变量声明/定义尾部>
//<单变量声明/定义> -> IDENTIFIER <数组尾部声明/定义> | 乘法符号(*) IDENTIFIER <变量初始化>
//<数组尾部声明/定义> -> 左中括号([) 数字常量 右中括号(]) | <变量初始化>
//<变量初始化> -> 等号(=) <表达式>
//<多变量声明/定义尾部> -> 逗号(,) <单变量声明/定义> <多变量声明/定义尾部> | 分号(;)
//<函数声明/定义> -> IDENTIFIER 左小括号"(" <参数列表> 右小括号")" <函数声明/定义尾部>
//<函数声明/定义尾部> -> <函数块> | 分号";"
/***提取左公因式 IDENTIFIER 后***/
//<声明定义> -> IDENTIFIER <声明定义尾部> | 乘法符号"*" IDENTIFIER <变量初始化> <多变量声明/定义尾部>
//<声明定义尾部> -> <数组尾部声明/定义> <多变量声明/定义尾部> | 左小括号"(" <参数列表> 右小括号")" <函数声明/定义尾部>
//<函数声明/定义尾部> -> <函数块> | 分号";"
/***提取左公因式 IDENTIFIER 后***/
/***函数参数部分***/
//<参数列表> -> <单参数> <多参数尾部>
//<单参数> -> <类型> <单参数尾部>
//<单参数尾部> -> 乘法符号"*" IDENTIFIER | IDENTIFIER <数组参数尾部>
//<数组参数尾部> -> 左中括号([) 数字常量 右中括号(]) | 空
//<多参数尾部> -> 逗号"," <单参数> <多参数尾部> | 空
/***函数块***/
//<函数块> -> 左大括号"{" <函数子块> 右大括号"}"
//<函数子块> -> <局部变量声明/定义> <函数子块> | <控制语句和表达式> <函数子块> | 空
//<局部变量声明/定义> -> <类型><单变量声明/定义><多变量声明/定义尾部>
//<控制语句和表达式> -> <表达式> 分号";" | <控制语句>
/*******表达式构造*******/
/******** 根据运算符的优先级构造文法 0~9依次
 * level0:[] ()
 * level1:()
 * level2:++ -- (后置)
 * level3:! -(负号) &(取地址) * ++ -- (前置)
 * level4:* / %
 * level5:+ -
 * level6:> < >= <= == !=
 * level7:&&
 * level8:||
 * level9:=
 * ******/
//<表达式> -> <level9表达式> | 空
//<level9表达式> -> <level8表达式><level9表达式尾部>
//<level9表达式尾部> -> 赋值 = <level8表达式><level9表达式尾部> | 空
//<level8表达式> -> <level7表达式><level8表达式尾部>
//<level8表达式尾部> -> 逻辑或|| <level7表达式><level8表达式尾部> | 空
//<level7表达式> -> <level6表达式><level7表达式尾部>
//<level7表达式尾部> -> 逻辑与&& <level6表达式><level7表达式尾部> | 空
//<level6表达式> -> <level5表达式><level6表达式尾部>
//<level6表达式尾部> -> <比较判断符号> <level5表达式><level6表达式尾部> | 空
//<比较判断符号> -> > | < | >= | <= | == | !=
//<level5表达式> -> <level4表达式><level5表达式尾部>
//<level5表达式尾部> -> 加减操作+/- <level4表达式><level5表达式尾部> | 空
//<level4表达式> -> <因子表达式><level4表达式尾部>
//<level4表达式尾部> -> 乘除求余*/% <因子表达式><level4表达式尾部> | 空
/*****这一部分比较特殊 level3的在操作符左侧 属于因子表达式*****/
//<因子表达式> -> <level3操作><因子表达式> | <值表达式>
//<level3操作> -> ! - & * ++ --
/*****这一部分比较特殊 level2的在操作符右侧 属于值表达式*****/
//<值表达式> -> <元素表达式><level2操作>
//<level2操作> -> ++ --(右侧)
/*****元素表达式是基本操作单元，包括变量，数组，函数调用，括号表达式和常量*****/
/***原始的元素表达式***/
//<元素表达式> -> IDENTIFIER | IDENTIFIER [ <表达式> ] | IDENTIFIER ( <函数实参列表> ) | ( <表达式> ) | <常量>
/***提取左公因式的元素表达式***/
//<元素表达式> -> IDENTIFIER <元素表达式尾部> | ( <表达式> ) | <常量>
//<常量> -> 数字 字符 字符串
//<元素表达式尾部> -> [ <表达式> ] | ( <函数实参列表> ) | 空
//<函数实参列表> -> <实参><多个实参尾部> | 空
//<多个实参尾部> -> 逗号"," <多个实参尾部> | 空
//<实参> -> <表达式>
/*******控制语句构造*******/
//<控制语句> -> <while> | <for> | <do-while> | <if> | <switch> | break 分号";" | continue 分号";" | return <表达式> 分号";"
//<while> -> KW_WHILE 左小括号( <表达式> 右小括号) <函数块>
//<for> -> KW_FOR 左小括号( <初始条件> <循环判断条件> <循环操作> 右小括号) <函数块>
//<初始条件> -> <局部变量声明/定义> | <表达式> 分号";"
//<循环判断条件> -> <表达式> 分号";"
//<循环操作> -> <表达式>
//<do-while> -> KW_DO <函数块> KW_WHILE 左小括号( <表达式> 右小括号) 分号";"
//TODO else-if complement
//<if> -> KW_IF 左小括号( <表达式> 右小括号) <函数块> <else>
//<else> -> KW_ELSE <函数块> | 空
//<switch> -> KW_SWITCH 左小括号( <表达式> 右小括号) 左大括号{ <switch-case> 右大括号}
//<switch-case> -> KW_CASE <常量> 冒号":" <函数子块> <switch-case> | KW_DEFAULT 冒号":" <函数子块>
/**这一部分实现一个语法分析器**/
class SyntacticParser {
private:
    //语法制导的编译器，parser驱动lexer
    Lexer &lexer;
    //记录当前分析的token
    Token *currentToken;
    //符号表
    SymbolTable &symbolTable;

    void getNextToken();
    bool matchToken(Tag t);
    void errorRecovery();
    //TODO follow集合相关问题
    /****为了简化判断条件写的函数****/
    //在<statement>的first集中
    bool matchTypeFirstSet();
    bool matchExpressionFirstSet();
    bool matchControlStatementSet();

    /****非终结符的函数****/
    //非终结符的具体函数
    void program();//程序
    void programSegment();//程序片段
    Tag type();//类型

    void decOrDef(Tag t);//声明定义 def
    void decOrDefTail();//声明定义尾部 idtail
    void varInit();//变量初始化 init
    void singleVarDecOrDef();//单变量声明/定义
    void multiVarDecOrDefTail();//多变量声明/定义尾部 deflist
    void arrayDecOrDef();//数组声明/定义 varrdef
    /*****函数文法定义****/
    void funDecOrDefTail();//函数声明/定义尾部 funtail
    void funBlock();//函数块 block
    void funParameterList();//函数参数列表 para
    void singleParameter();//单参数 type paradata
    void singleParameterTail();//单参数尾部 paradata
    void arrayParameterTail();//数组参数尾部 paradatatail
    void multiParameterTail();//多参数尾部 paralist

    void funSubBlock();//函数子块 subprogram
    void localVarDecOrDef();//局部变量声明/定义 localdef
    void statement();//控制语句和表达式 statement

    /****表达式构造****/
    void expression();//表达式 altexpr  expr|空
    void level9Expression();//level9表达式 assexpr
    void level9ExpressionTail();//level9表达式尾部 asstail
    void level8Expression();//level8表达式 orexpr
    void level8ExpressionTail();//level8 tail ortail
    void level7Expression();//level7表达式 andexpr
    void level7ExpressionTail();//level7 tail andtail
    void level6Expression();//level6表达式 cmpexpr
    void level6ExpressionTail();//level6 tail cmptail
    Tag level6Operation();//比较判断符号 GT GE LT LE EQU NEQU
    void level5Expression();//level5表达式 aloexpr
    void level5ExpressionTail();//level5 tail alotail
    Tag level5Operation();//加减运算符 + -
    void level4Expression();//level4表达式 cmpexpr
    void level4ExpressionTail();//level4 tail cmptail
    Tag level4Operation();//乘除求余 * / %
    void factorExpression();//因子表达式
    Tag level3Operation();// ! - & * ++ --
    void valueExpression();//值表达式
    Tag level2Operation();// ++ -- 后置
    void elementExpression();//表达式元素的表达式
    void elementExpressionTail();//元素表达式尾部
    VarElement* constant();//常量
    void funRealParameterList();//函数实参列表
    void singleRealParameter();//（单个）实参 arg
    void multiRealParameterTail();//多个实参尾部

    /****控制语句****/
    void controlStatement();//控制语句
    void whileStatement();//while语句
    void forStatement();// for
    void forInitStatement();// for初始条件
    void forJudgeStatement();// for循环判断条件
    void forOperationStatement();// for循环操作
    void doWhileStatement();// do while
    void ifStatement();// if
    void ifElseStatement();// else
    void switchStatement();// switch
    void switchCaseStatement();// switch case

public:
    //语法分析构造函数
    SyntacticParser(Lexer &lexer1, SymbolTable &symbolTable1);
    //开始语法分析
    void startSyntacticAnalyze();
    //输出分析的token
    void showCurrentToken();
};


#endif //COMPILER_CPP_SYNTACTIC_PARSER_H
