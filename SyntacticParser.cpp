//
// Created by 95792 on 2020/4/17.
//
#include "SyntacticParser.h"

//TODO 引用类型？ 指针类型？
SyntacticParser::SyntacticParser(Lexer &lexer1, SymbolTable &symbolTable1):lexer(lexer1),symbolTable(symbolTable1) {
    currentToken = nullptr;
}

void SyntacticParser::getNextToken() {
    currentToken = lexer.getToken();
}

bool SyntacticParser::matchToken(Tag t) {
    return currentToken->tag == t;
}

//TODO 自设计的错误恢复算法
void SyntacticParser::errorRecovery() {
    //TODO 错误修复
    cout << "Recovery used" << endl;
}

void SyntacticParser::startSyntacticAnalyze() {
    //提前读入一个token
    getNextToken();
    //进入program函数
    program();
}

void SyntacticParser::showCurrentToken() {
    cout << currentToken->standardOutput() << endl;
}

/******判断first集合的函数******/
bool SyntacticParser::matchTypeFirstSet() {
    return matchToken(KW_INT) || matchToken(KW_CHAR) || matchToken(KW_VOID) || matchToken(KW_BOOL);
}

bool SyntacticParser::matchControlStatementSet() {
    return matchToken(KW_WHILE) || matchToken(KW_FOR) || matchToken(KW_DO) || matchToken(KW_IF) || matchToken(KW_SWITCH) || matchToken(KW_CONTINUE) || matchToken(KW_BREAK) || matchToken(KW_RETURN);
}

bool SyntacticParser::matchExpressionFirstSet() {
    //<元素表达式>的first集合
    bool temp1 = matchToken(IDENTIFIER) || matchToken(DELIMITER_LEFT_PAREN) || matchToken(CONSTANT_NUM) || matchToken(CONSTANT_CHAR) || matchToken(CONSTANT_STRING);
    //level操作符的first集合
    bool temp2 = matchToken(DELIMITER_NOT) || matchToken(DELIMITER_SUB) || matchToken(DELIMITER_LEA) || matchToken(DELIMITER_MUL) || matchToken(DELIMITER_INC) || matchToken(DELIMITER_DEC);
    //<表达式>为空的情况 选择后继follow集合{;}
    bool temp3 = matchToken(DELIMITER_SEMICOLON);
    return temp1 || temp2 || temp3;
}

/****以下是递归下降中的子程序——对应文法中的非终结符****/
/*****匹配到非终结符后getNextToken，否则进行错误处理*****/
//TODO scanf和printf语句的支持
//<程序> -> <程序片段><程序> | 空
void SyntacticParser::program() {
    //产生式为空的情况 考虑follow集合 这里只有文件结束这一可能
    if (matchToken(FILE_END))
        return;
    else{
        programSegment();
        program();
    }
}

//<程序片段> -> <显式标志Extern><类型><声明定义>
void SyntacticParser::programSegment() {
    //暂时不考虑extern
//    bool extern_type = matchAndGetNextToken(KW_EXTERN);
    //<类型>
    Tag temp = type();
    //<声明定义>
    decOrDef(temp);
}

//<类型> -> KW_INT | KW_CHAR | KW_BOOL | KW_VOID
Tag SyntacticParser::type() {
    Tag temp = KW_INT;
    if (matchToken(KW_INT) || matchToken(KW_CHAR) || matchToken(KW_BOOL) || matchToken(KW_VOID)){
        temp = currentToken->tag;
        //终结符匹配成功,获取下一个字符
        getNextToken();
    } else{
        cout << "In function type(), Tag type error" << currentToken->tag << endl;
    }
    return temp;
}

//<声明定义> -> IDENTIFIER <声明定义尾部> | 乘法符号"*" IDENTIFIER <变量初始化> <多变量声明/定义尾部>
void SyntacticParser::decOrDef(Tag t) {
    if (matchToken(IDENTIFIER) || matchToken(KW_MAIN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        decOrDefTail();
    }
    //指针变量声明定义
    else if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(IDENTIFIER))
        {
            //终结符匹配成功,获取下一个字符
            getNextToken();
            varInit();
            multiVarDecOrDefTail();
        } else{
            cout << "In function decOrDef(), identifier unmatched" <<endl;
        }
    } else{
        cout << "In function decOrDef(), token unmatched" <<endl;
    }
}

//<声明定义尾部> -> <数组尾部声明/定义> <多变量声明/定义尾部> | 左小括号"(" <参数列表> 右小括号")" <函数声明/定义尾部>
void SyntacticParser::decOrDefTail() {
    if(matchToken(DELIMITER_LEFT_PAREN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        funParameterList();
        if (matchToken(DELIMITER_RIGHT_PAREN)){
            //终结符匹配成功,获取下一个字符
            getNextToken();
            funDecOrDefTail();
        } else{
            cout<< "In decOrDefTail ) match Error" << endl;
        }
    } else{
        //数组尾部定义声明
        arrayDecOrDef();
        multiVarDecOrDefTail();
    }
}

//<变量初始化> -> 等号(=) <表达式>
void SyntacticParser::varInit() {
    if (matchToken(DELIMITER_ASSIGN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        expression();
    } else{
        cout<< "In VarInit match Error" << endl;
    }
}

//<单变量声明/定义> -> IDENTIFIER <数组尾部声明/定义> | 乘法符号(*) IDENTIFIER <变量初始化>
void SyntacticParser::singleVarDecOrDef() {
    if (matchToken(IDENTIFIER)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //数组尾部声明定义
        arrayDecOrDef();
    }
    else if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if(matchToken(IDENTIFIER))
        {
            //终结符匹配成功,获取下一个字符
            getNextToken();
            varInit();
        } else{
            cout << "In singleVarDecOrDef match error1" << endl;
        }
    } else {
        cout << "In singleVarDecOrDef match error2" << endl;
    }
}

//<多变量声明/定义尾部> -> 逗号(,) <单变量声明/定义> <多变量声明/定义尾部> | 分号(;)
void SyntacticParser::multiVarDecOrDefTail() {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<单变量声明/定义>
        singleVarDecOrDef();
        //<多变量声明/定义尾部>
        multiVarDecOrDefTail();
    }else if (matchToken(DELIMITER_SEMICOLON)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        return;
    }
    else{
        cout << "In multiVarDecOrDefTail match error" << endl;
    }
}

//<数组尾部声明/定义> -> 左中括号([) 数字常量 右中括号(]) | <变量初始化>
void SyntacticParser::arrayDecOrDef() {
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(CONSTANT_NUM)){
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_RIGHT_BRACKET))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In arrayDecOrDef right bracket match error" << endl;
        } else{
            cout << "In arrayDecOrDef constant num match error" << endl;
        }
    } else{
        //变量初始化
        varInit();
    }
}

/***函数***/
//<函数声明/定义尾部> -> <函数块> | 分号";"
void SyntacticParser::funDecOrDefTail() {
    if (matchToken(DELIMITER_SEMICOLON)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
    } else{
        //<函数块>
        funBlock();
    }
}

//<函数块> -> 左大括号"{" <函数子块> 右大括号"}"
void SyntacticParser::funBlock() {
    if (matchToken(DELIMITER_LEFT_BRACE)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<函数子块>
        funSubBlock();
        if (matchToken(DELIMITER_RIGHT_BRACE))
        {
            //终结符匹配成功,获取下一个字符
            getNextToken();
            return;
        }
        else
            cout << "In funBlock } unmatched" << endl;
    } else
        cout << "In funBlock { unmatched" << endl;
}

//<函数参数列表> -> <单参数> <多参数尾部> | 空
void SyntacticParser::funParameterList() {
    //参数列表为空的情况
    if (matchToken(DELIMITER_RIGHT_PAREN)){
        return;
    }
    else{
        //<单参数>
        singleParameter();
        //<多参数尾部>
        multiParameterTail();
    }
}

//<单参数> -> <类型> <单参数尾部>
void SyntacticParser::singleParameter() {
    //<类型>
    Tag temp = type();
    //<单参数尾部>
    singleParameterTail();
}

//<单参数尾部> -> 乘法符号"*" IDENTIFIER | IDENTIFIER <数组参数尾部>
void SyntacticParser::singleParameterTail() {
    if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(IDENTIFIER))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In singleParameterTail ID match error" << endl;
    } else if (matchToken(IDENTIFIER)){
        cout << "In singleParameterTail ID match error" << endl;
        //<数组参数尾部>
        arrayParameterTail();
    } else{
        cout << "In singleParameterTail * match error" << endl;
    }
}

//<数组参数尾部> -> 左中括号([) 数字常量 右中括号(]) | 空
void SyntacticParser::arrayParameterTail() {
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(CONSTANT_NUM)){
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_RIGHT_BRACKET))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In arrayParameterTail ] match error" << endl;
        } else
            cout << "In arrayParameterTail num match error" << endl;
    } else
        cout << "In arrayParameterTail [ unmatched error || null generated" << endl;
}

//<多参数尾部> -> 逗号"," <单参数> <多参数尾部> | 空
void SyntacticParser::multiParameterTail() {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        singleParameter();
        multiParameterTail();
    } else
        cout << "In multiParameterTail comma, unmatched || null generated" << endl;
}

//<函数子块> -> <局部变量声明/定义> <函数子块> | <控制语句和表达式> <函数子块> | 空
void SyntacticParser::funSubBlock() {
    //<局部变量声明/定义>
    if (matchTypeFirstSet()){
        //<局部变量声明/定义>
        localVarDecOrDef();
        //<函数子块>
        funSubBlock();
    }
    //<控制语句和表达式>
    else if (matchExpressionFirstSet() || matchControlStatementSet()){
        //<控制语句和表达式>
        statement();
        //<函数子块>
        funSubBlock();
    } else
        cout << "In funSubBlock unmatched || null generated" << endl;
}

//<局部变量声明/定义> -> <类型><单变量声明/定义><多变量声明/定义尾部>
void SyntacticParser::localVarDecOrDef() {
    //<类型>
    Tag temp = type();
    //<单变量声明/定义>
    singleVarDecOrDef();
    //<多变量声明/定义尾部>
    multiVarDecOrDefTail();
}

//<控制语句和表达式> -> <表达式> 分号";" | <控制语句>
void SyntacticParser::statement() {
    if (matchControlStatementSet())
        controlStatement();
    else if (matchExpressionFirstSet()){
        expression();
        if (matchToken(DELIMITER_SEMICOLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In statement ; unmatched" << endl;
    } else
        cout << "In statement unknown unmatched" << endl;
}

/***表达式***/
//<表达式> -> <level9表达式> | 空
void SyntacticParser::expression() {
    if (matchExpressionFirstSet()){
        level9Expression();
    } else
        cout << "In expression null expression" << endl;
}

//<level9表达式> -> <level8表达式><level9表达式尾部>
void SyntacticParser::level9Expression() {
    //<level8表达式>
    level8Expression();
    //<level9表达式尾部>
    level9ExpressionTail();
}

//<level9表达式尾部> -> 赋值 = <level8表达式><level9表达式尾部> | 空
void SyntacticParser::level9ExpressionTail() {
    //TODO some problem?
    if (matchToken(DELIMITER_ASSIGN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level8表达式>
        level8Expression();
        //<level9表达式尾部>
        level9ExpressionTail();
    } else{
        cout << "In level9 expression null generated" << endl;
        return;
    }
}

//<level8表达式> -> <level7表达式><level8表达式尾部>
void SyntacticParser::level8Expression() {
    //<level7表达式>
    level7Expression();
    //<level8表达式尾部>
    level8ExpressionTail();
}

//<level8表达式尾部> -> 逻辑或|| <level7表达式><level8表达式尾部> | 空
void SyntacticParser::level8ExpressionTail() {
    if (matchToken(DELIMITER_OR)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level7表达式>
        level7Expression();
        //<level8表达式尾部>
        level7ExpressionTail();
    }
}

//<level7表达式> -> <level6表达式><level7表达式尾部>
void SyntacticParser::level7Expression() {
    //<level6表达式>
    level6Expression();
    //<level7表达式尾部>
    level7ExpressionTail();
}

//<level7表达式尾部> -> 逻辑与&& <level6表达式><level7表达式尾部> | 空
void SyntacticParser::level7ExpressionTail() {
    if (matchToken(DELIMITER_AND)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level6表达式>
        level6Expression();
        //<level7表达式尾部>
        level7ExpressionTail();
    } else
    {
        cout << "In level7 expression tail null generated" << endl;
        return;
    }
}

//<level6表达式> -> <level5表达式><level6表达式尾部>
void SyntacticParser::level6Expression() {
    //<level5表达式>
    level5Expression();
    //<level6表达式尾部>
    level6ExpressionTail();
}

//<level6表达式尾部> -> <比较判断符号><level5表达式><level6表达式尾部> | 空
void SyntacticParser::level6ExpressionTail() {
    if (matchToken(DELIMITER_GREATER) || matchToken(DELIMITER_GREATER_EQUAL) || matchToken(DELIMITER_LESS) || matchToken(DELIMITER_LESS_EQUAL) || matchToken(DELIMITER_EQUAL) || matchToken(DELIMITER_NOT_EQUAL)){
        //<比较判断符号>
        Tag temp = level6Operation();
        //<level5表达式>
        level5Expression();
        //<level6表达式尾部>
        level6ExpressionTail();
    } else{
        cout << "In level6Expression opt unmatched / NULL" << endl;
        return;
    }
}

//<比较判断符号> level6操作符号
Tag SyntacticParser::level6Operation() {
    Tag operation = currentToken->tag;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return operation;
}

//<level5表达式> -> <level4表达式><level5表达式尾部>
void SyntacticParser::level5Expression() {
    //<level4表达式>
    level4Expression();
    //<level5表达式尾部>
    level5ExpressionTail();
}

//<level5表达式尾部> -> 加减操作+/- <level4表达式><level5表达式尾部> | 空
void SyntacticParser::level5ExpressionTail() {
    if (matchToken(DELIMITER_SUB) || matchToken(DELIMITER_ADD)){
        Tag operation = level5Operation();
        //<level4表达式>
        level4Expression();
        //<level5表达式尾部>
        level5ExpressionTail();
    } else{
        cout << "In level5Expression opt unmatched / NULL" << endl;
        return;
    }
}

//<加减操作> level5操作
Tag SyntacticParser::level5Operation() {
    Tag operation = currentToken->tag;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return operation;
}

//<level4表达式> -> <因子表达式><level4表达式尾部>
void SyntacticParser::level4Expression() {
    //<因子表达式>
    factorExpression();
    //<level4表达式尾部>
    level4ExpressionTail();
}

//<level4表达式尾部> -> 乘除求余*/% <因子表达式><level4表达式尾部> | 空
void SyntacticParser::level4ExpressionTail() {
    if (matchToken(DELIMITER_MUL) || matchToken(DELIMITER_DIV) || matchToken(DELIMITER_MOD)){
        Tag operation = level4Operation();
        //<因子表达式>
        factorExpression();
        //<level4表达式尾部>
        level4ExpressionTail();
    } else{
        cout << "In level4Expression opt unmatched / NULL" << endl;
        return;
    }
}

//<乘除求余*/%> level4操作
Tag SyntacticParser::level4Operation() {
    Tag operation = currentToken->tag;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return operation;
}

//<因子表达式> -> <level3操作><因子表达式> | <值表达式>
void SyntacticParser::factorExpression() {
    if (matchToken(DELIMITER_NOT) || matchToken(DELIMITER_SUB) || matchToken(DELIMITER_LEA) || matchToken(DELIMITER_MUL) || matchToken(DELIMITER_INC) || matchToken(DELIMITER_DEC)){
        Tag operation = level3Operation();
        //<因子表达式>
        factorExpression();
    } else{
        //TODO 值表达式的first集合
        valueExpression();
    }
}

//TODO 操作函数operation简化
//<level3操作> -> ! - & * ++ --
Tag SyntacticParser::level3Operation() {
    Tag operation = currentToken->tag;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return operation;
}

//<值表达式> -> <元素表达式><level2操作>
void SyntacticParser::valueExpression() {
    //<元素表达式>
    elementExpression();
    if (matchToken(DELIMITER_INC) || matchToken(DELIMITER_DEC)){
        Tag operation = level2Operation();
    } else
        cout << "In value Expression opt unmatched" << endl;
}

//<level2操作> -> ++ --(右侧)
Tag SyntacticParser::level2Operation() {
    Tag operation = currentToken->tag;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return operation;
}

//<元素表达式> -> IDENTIFIER <元素表达式尾部> | ( <表达式> ) | <常量>
void SyntacticParser::elementExpression() {
    if (matchToken(IDENTIFIER)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<元素表达式尾部>
        elementExpressionTail();
    } else if (matchToken(DELIMITER_LEFT_PAREN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<表达式>
        expression();
        if (matchToken(DELIMITER_RIGHT_PAREN))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In element Expression ) unmatched" << endl;
    } else if (matchToken(CONSTANT_NUM) || matchToken(CONSTANT_CHAR) || matchToken(CONSTANT_STRING)){
        constant();
    } else
        cout << "In element Expression unknown unmatched" << endl;
}

//<元素表达式尾部> -> [ <表达式> ] | ( <函数实参列表> ) | 空
void SyntacticParser::elementExpressionTail() {
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<表达式>
        expression();
        if (matchToken(DELIMITER_RIGHT_BRACKET))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In elementExpressionTail ] unmatched" << endl;
    } else if (matchToken(DELIMITER_LEFT_PAREN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<函数实参列表>
        funRealParameterList();
        if (matchToken(DELIMITER_RIGHT_PAREN))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In elementExpressionTail ) unmatched" << endl;
    } else{
        cout << "In elementExpressionTail null generated" << endl;
        return;
    }
}

//<常量> -> 数字 字符 字符串
VarElement* SyntacticParser::constant() {
    //TODO implement
    VarElement *v = nullptr;
//    v = new VarElement(currentToken);
    //终结符匹配成功,获取下一个字符
    getNextToken();
    return v;
}

//<函数实参列表> -> <实参><多个实参尾部> | 空
void SyntacticParser::funRealParameterList() {
    //函数实参列表为空的情况
    if (matchToken(DELIMITER_RIGHT_PAREN))
        return;
    else{
        //<实参>
        singleRealParameter();
        //<多个实参尾部>
        multiRealParameterTail();
    }
}

//<实参> -> <表达式>
void SyntacticParser::singleRealParameter() {
    expression();
}

//<多个实参尾部> -> 逗号"," <多个实参尾部> | 空
void SyntacticParser::multiRealParameterTail() {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<多个实参尾部>
        multiRealParameterTail();
    } else{
        cout << "In multiRealParameterTail NULL generated" << endl;
        return;
    }
}

/***控制语句***/
//<控制语句> -> <while> | <for> | <do-while> | <if> | <switch> | break 分号";" | continue 分号";" | return <表达式> 分号";"
void SyntacticParser::controlStatement() {
    switch (currentToken->tag){
        //<控制语句>
        case KW_WHILE:
            whileStatement();
            break;
        case KW_FOR:
            forStatement();
            break;
        case KW_DO :
            doWhileStatement();
            break;
        case KW_IF:
            ifStatement();
            break;
        case KW_SWITCH:
            switchStatement();
            break;
        case KW_CONTINUE:
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_SEMICOLON))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In Statement kw_continue ; unmatched" << endl;
            break;
        case KW_BREAK:
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_SEMICOLON))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In Statement kw_break ; unmatched" << endl;
        case KW_RETURN:
            //终结符匹配成功,获取下一个字符
            getNextToken();
            //<表达式>
            expression();
            //semicolon;
            if (matchToken(DELIMITER_SEMICOLON))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In Statement kw_return ; unmatched" << endl;
            break;
        default:
            cout << "In controlStatement unmatched" << endl;
    }
}

//TODO 函数块{} 或者直接单行操作
//<while> -> KW_WHILE 左小括号( <表达式> 右小括号) <函数块>
void SyntacticParser::whileStatement() {
    //终结符KW_WHILE匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In while statement ( unmatched" << endl;
    /***<表达式>****/
    expression();
    /***<表达式>****/
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In while statement ( unmatched" << endl;
    //<函数块>
    funBlock();
}

//<for> -> KW_FOR 左小括号( <初始条件> <循环判断条件> <循环操作> 右小括号) <函数块>
void SyntacticParser::forStatement() {
    //终结符KW_FOR匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forStatement ( unmatched" << endl;
    //<初始条件>
    forInitStatement();
    //<循环判断条件>
    forJudgeStatement();
    //<循环操作>
    forOperationStatement();
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forStatement ) unmatched" << endl;
    //<函数块>
    funBlock();
}

//<初始条件> -> <局部变量声明/定义> | <表达式> 分号";"
void SyntacticParser::forInitStatement() {
    if (matchTypeFirstSet())
        localVarDecOrDef();
    else if (matchExpressionFirstSet()){
        expression();
        if (matchToken(DELIMITER_SEMICOLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In forInitStatement ; unmatched" << endl;
    } else
        cout << "In forInitStatement unmatched" << endl;
}

//<循环判断条件> -> <表达式> 分号";"
void SyntacticParser::forJudgeStatement() {
    expression();
    if (matchToken(DELIMITER_SEMICOLON))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forJudgeStatement ; unmatched" << endl;
}

//<循环操作> -> <表达式>
void SyntacticParser::forOperationStatement() {
    expression();
}

//<do-while> -> KW_DO <函数块> KW_WHILE 左小括号( <表达式> 右小括号) 分号";"
void SyntacticParser::doWhileStatement() {
    //终结符KW_DO匹配成功,获取下一个字符
    getNextToken();
    //<函数块>
    funBlock();
    if (matchToken(KW_WHILE))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "in do-while statement kw_while unmatched" << endl;
    //左小括号(
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "in do-while statement ( unmatched" << endl;
    //<表达式>
    expression();
    //右小括号)
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "in do-while statement ) unmatched" << endl;
    //分号;
    if (matchToken(DELIMITER_SEMICOLON))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "in do-while statement ; unmatched" << endl;
}

//<if> -> KW_IF 左小括号( <表达式> 右小括号) <函数块> <else>
void SyntacticParser::ifStatement() {
    //终结符KW_IF匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In ifStatement ( unmatched" << endl;
    //<表达式>
    expression();
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In ifStatement ) unmatched" << endl;
    //<函数块>
    funBlock();
    //<else>
    ifElseStatement();
}

//<else> -> KW_ELSE <函数块> | 空
void SyntacticParser::ifElseStatement() {
    //KW_ELSE
    if (matchToken(KW_ELSE)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        funBlock();
    } else{
        cout << "In ifElseStatement null generated" << endl;
        return;
    }
}

//<switch> -> KW_SWITCH 左小括号( <表达式> 右小括号) 左大括号{ <switch-case> 右大括号}
void SyntacticParser::switchStatement() {
    //终结符KW_SWITCH匹配成功,获取下一个字符
    getNextToken();
    //左小括号(
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement ( unmatched" << endl;
    //<表达式>
    expression();
    //右小括号)
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement ) unmatched" << endl;
    //左大括号{
    if (matchToken(DELIMITER_LEFT_BRACE))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement { unmatched" << endl;
    //<switch-case>
    switchCaseStatement();
    //右大括号}
    if (matchToken(DELIMITER_RIGHT_BRACE))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement } unmatched" << endl;
}

//<switch-case> -> KW_CASE <常量> 冒号":" <函数子块> <switch-case> | KW_DEFAULT 冒号":" <函数子块>
void SyntacticParser::switchCaseStatement() {
    if (matchToken(KW_CASE)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<常量>
        constant();
        if (matchToken(DELIMITER_COLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In switchStatement case : unmatched" << endl;
        //<函数子块>
        funSubBlock();
        //<switch-case>
        switchCaseStatement();
    } else if (matchToken(KW_DEFAULT)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(DELIMITER_COLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In switchStatement default : unmatched" << endl;
        //<函数子块>
        funSubBlock();
    } else
        cout << "In switchCaseStatement unmatched" << endl;
}



