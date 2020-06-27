//
// Created by 95792 on 2020/4/17.
//
#include "SyntacticParser.h"
#include "GenerateCode.h"

//todo 函数块{} 或者单语句 statement

SyntacticParser::SyntacticParser(Lexer &lexer1, SymbolTable &symbolTable1, GenerateCode &generateCode1):lexer(lexer1),symbolTable(symbolTable1),codeGenerator(generateCode1) {
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

bool SyntacticParser::matchControlStatementFirstSet() {
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

//def
//<声明定义> -> IDENTIFIER <声明定义尾部> | 乘法符号"*" IDENTIFIER <变量初始化> <多变量声明/定义尾部>
void SyntacticParser::decOrDef(Tag t) {
    //fun或者var的name
    string name;
    if (matchToken(IDENTIFIER) || matchToken(KW_MAIN)){
        //name写入
        name = currentToken->getName();
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<声明定义尾部>
        decOrDefTail(t, false, name);
    }
    //指针变量声明定义
    else if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(IDENTIFIER))
        {
            //name写入
            name = currentToken->getName();
            //终结符匹配成功,获取下一个字符
            getNextToken();
            //指针变量
            VarElement* init_result = varInit(t, true, name);
            symbolTable.addVariable(init_result);
            //<多变量声明定义尾部>
            multiVarDecOrDefTail(t);
        } else{
            cout << "In function decOrDef(), identifier unmatched" <<endl;
        }
    } else{
        cout << "In function decOrDef(), token unmatched" <<endl;
    }
}

//idtail
//<声明定义尾部> -> <数组尾部声明/定义> <多变量声明/定义尾部> | 左小括号"(" <参数列表> 右小括号")" <函数声明/定义尾部>
void SyntacticParser::decOrDefTail(Tag t, bool isPtr, string name) {
    if(matchToken(DELIMITER_LEFT_PAREN)){
        //作用域管理
        symbolTable.scopeEnter();
        //创建参数列表
        vector<VarElement*> arg_list;
        //终结符匹配成功,获取下一个字符
        getNextToken();
        funParameterList(arg_list);
        if (matchToken(DELIMITER_RIGHT_PAREN)){
            //创建函数变量
            auto* fun = new FunElement(name, t, arg_list);
            //终结符匹配成功,获取下一个字符
            getNextToken();
            funDecOrDefTail(fun);  //<函数声明定义尾部>
        } else{
            cout<< "In decOrDefTail ) match Error" << endl;
        }
    } else{
        //数组尾部定义声明
        auto* temp = arrayDecOrDef(t, false, name); //尾部保存数组长度——常量，非数组
        symbolTable.addVariable(temp);
        //<函数声明/定义尾部>
        multiVarDecOrDefTail(t);
    }
}

//init
//<变量初始化> -> 等号(=) <表达式>
VarElement* SyntacticParser::varInit(Tag t, bool isPtr, string name) {
    VarElement* init_value = nullptr;
    if (matchToken(DELIMITER_ASSIGN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        init_value = expression();
    } else{
        cout<< "In VarInit match Error or init = null" << endl;
    }
    auto* init_result = new VarElement(symbolTable.getScopePath(), t, name, isPtr, init_value);
    return init_result;
}

//defdata
//继承属性：类型tag，返回值：定义的var
//<单变量声明/定义> -> IDENTIFIER <数组尾部声明/定义> | 乘法符号(*) IDENTIFIER <变量初始化>
VarElement* SyntacticParser::singleVarDecOrDef(Tag t) {
    string name;
    if (matchToken(IDENTIFIER)){
        //写入name
        name = currentToken->getName();
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //数组尾部声明定义
        return arrayDecOrDef(t, false, name);
    }
    else if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if(matchToken(IDENTIFIER))
        {
            //写入name
            name = currentToken->getName();
            //终结符匹配成功,获取下一个字符
            getNextToken();
        } else{
            cout << "In singleVarDecOrDef match error1" << endl;
        }
        return varInit(t, true, name);
    } else {
        cout << "In singleVarDecOrDef match error2" << endl;
        return arrayDecOrDef(t, false, name);
    }
}

//deflist
//<多变量声明/定义尾部> -> 逗号(,) <单变量声明/定义> <多变量声明/定义尾部> | 分号(;)
void SyntacticParser::multiVarDecOrDefTail(Tag t) {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<单变量声明/定义>
        VarElement* temp = singleVarDecOrDef(t);
        //将声明/定义的变量添加到符号表
        symbolTable.addVariable(temp);
        //<多变量声明/定义尾部>
        multiVarDecOrDefTail(t);
    }else if (matchToken(DELIMITER_SEMICOLON)){
        //已经是最后一个变量的声明
        //终结符匹配成功,获取下一个字符
        getNextToken();
        return;
    }
    else{
        cout << "In multiVarDecOrDefTail match error" << endl;
    }
}

//varrdef
//继承属性 类型，是否为指针，名称name
//<数组尾部声明/定义> -> 左中括号([) 数字常量 右中括号(]) | <变量初始化>
VarElement* SyntacticParser::arrayDecOrDef(Tag t, bool isPtr, string name) {
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        //设置变量来记录数组长度
        int array_length = 0;
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(CONSTANT_NUM)){
            //记录数组长度
            array_length = ((Num*)currentToken)->num_value;
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_RIGHT_BRACKET))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In arrayDecOrDef right bracket ] match error" << endl;
        } else{
            cout << "In arrayDecOrDef constant num match error" << endl;
        }
        //返回创建的数组变量
        auto* temp = new VarElement(symbolTable.getScopePath(), t, name, array_length);
        return temp;
    } else{
        //变量初始化
        return varInit(t, isPtr, name);
    }
}

/***函数***/
//funtail
//<函数声明/定义尾部> -> <函数块> | 分号";"
void SyntacticParser::funDecOrDefTail(FunElement* fun) {
    if (matchToken(DELIMITER_SEMICOLON)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //将函数添加至符号表
        symbolTable.declareFunction(fun);
    } else{
        //<函数块>, 函数定义
        symbolTable.defineFunction(fun);
        funBlock();
        symbolTable.endDefineFunction();
    }
}

//block
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

//para
//<函数参数列表> -> <单参数> <多参数尾部> | 空
void SyntacticParser::funParameterList(vector<VarElement*>& list) {
    //参数列表为空的情况
    if (matchToken(DELIMITER_RIGHT_PAREN)){
        return;
    }
    else{
        //<单参数>,保存变量，并添加至符号表
        auto* para = singleParameter(); //保存单变量
        symbolTable.addVariable(para);  //将变量添加到符号表
        list.push_back(para);           //将变量添加到参数列表list中
        //<多参数尾部>
        multiParameterTail(list);
    }
}

//type + paradata
//<单参数> -> <类型> <单参数尾部>
VarElement* SyntacticParser::singleParameter() {
    //<类型>
    Tag t = type();
    //<单参数尾部>
    auto* para = singleParameterTail(t);
    return para;
}

//paradata
//<单参数尾部> -> 乘法符号"*" IDENTIFIER | IDENTIFIER <数组参数尾部>
VarElement* SyntacticParser::singleParameterTail(Tag t) {
    string name;
    if (matchToken(DELIMITER_MUL)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(IDENTIFIER)){
            //写入name
            name = currentToken->getName();
            //终结符匹配成功,获取下一个字符
            getNextToken();
        } else
            cout << "In singleParameterTail ID match error" << endl;
        //创建指针变量并返回
        auto* temp = new VarElement(symbolTable.getScopePath(), t, name, true);
        return temp;
    } else if (matchToken(IDENTIFIER)){
        //写入name
        name = currentToken->getName();
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<数组参数尾部>
        return arrayParameterTail(t, name);
    } else{
        cout << "In singleParameterTail * match error" << endl;
        auto* temp = new VarElement(symbolTable.getScopePath(), t, name, false);
        return temp;
    }
}

//paradatatail
//继承属性 name t, 返回值：数组长度常量
//<数组参数尾部> -> 左中括号([) 数字常量 右中括号(]) | 空
VarElement* SyntacticParser::arrayParameterTail(Tag t, string name) {
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        int array_length = 0;   //初始化数组长度
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(CONSTANT_NUM)){
            array_length = ((Num*)currentToken)->num_value;
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_RIGHT_BRACKET))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In arrayParameterTail ] match error" << endl;
            //创建数组长度变量
            auto* var = new VarElement(symbolTable.getScopePath(), t, name, array_length);
            return var;
        } else
            cout << "In arrayParameterTail num match error" << endl;
    } else
        cout << "In arrayParameterTail [ unmatched error || null generated" << endl;
    //创建普通变量并返回
    auto* temp = new VarElement(symbolTable.getScopePath(), t, name, false);
    return temp;
}

//多参数尾部 paralist
//<多参数尾部> -> 逗号"," <单参数> <多参数尾部> | 空
void SyntacticParser::multiParameterTail(vector<VarElement*>& list) {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        auto* para = singleParameter();
        symbolTable.addVariable(para);  //将参数添加到符号表
        list.push_back(para);           //参数添加到参数表list
        multiParameterTail(list);
    } else
        cout << "In multiParameterTail comma, unmatched || null generated" << endl;
}

//函数子块 subprogram
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
    else if (matchExpressionFirstSet() || matchControlStatementFirstSet()){
        //<控制语句和表达式>
        statement();
        //<函数子块>
        funSubBlock();
    } else
        cout << "In funSubBlock unmatched || null generated" << endl;
}

//局部变量声明/定义 localdef
//<局部变量声明/定义> -> <类型><单变量声明/定义><多变量声明/定义尾部>
void SyntacticParser::localVarDecOrDef() {
    //<类型>
    Tag t = type();
    //<单变量声明/定义>
    auto* var = singleVarDecOrDef(t);
    symbolTable.addVariable(var);   //将单变量添加至符号表
    //<多变量声明/定义尾部>
    multiVarDecOrDefTail(t);
}

//控制语句和表达式 statement
//<控制语句和表达式> -> <表达式> 分号";" | <控制语句>
void SyntacticParser::statement() {
    if (matchControlStatementFirstSet())
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
//表达式 altexpr  expr|空
//<表达式> -> <level9表达式> | 空
VarElement* SyntacticParser::expression() {
    if (matchExpressionFirstSet()){
        return level9Expression();
    } else{
        cout << "In expression null generated" << endl;
        //返回void类型变量
        return SymbolTable::var_void;
    }
}

//level9表达式 assexpr
//<level9表达式> -> <level8表达式><level9表达式尾部>
VarElement* SyntacticParser::level9Expression() {
    //<level8表达式>
    auto* left_var = level8Expression();
    //<level9表达式尾部>
    return level9ExpressionTail(left_var);
}

//level9表达式尾部 asstail
//<level9表达式尾部> -> 赋值 = <level8表达式><level9表达式尾部> | 空
VarElement* SyntacticParser::level9ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_ASSIGN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level8表达式>
        auto* tempVar = level8Expression();
        //<level9表达式尾部>
        auto* right_var = level9ExpressionTail(tempVar);
        auto* result = codeGenerator.genTwoOperation(OP_ASSIGN, left_var, right_var);
        return level9ExpressionTail(result);
    } else{
        cout << "In level9 expression null generated" << endl;
        return left_var;
    }
}

//level8表达式 orexpr
//<level8表达式> -> <level7表达式><level8表达式尾部>
VarElement* SyntacticParser::level8Expression() {
    //<level7表达式>
    VarElement* left_var = level7Expression();
    //<level8表达式尾部>
    return level8ExpressionTail(left_var);
}

//level8 tail ortail
//<level8表达式尾部> -> 逻辑或|| <level7表达式><level8表达式尾部> | 空
VarElement* SyntacticParser::level8ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_OR)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level7表达式>
        VarElement* right_var = level7Expression();
        VarElement* result_var = codeGenerator.genTwoOperation(OP_OR, left_var, right_var);
        //<level8表达式尾部>
        return level7ExpressionTail(result_var);
    } else
        return left_var;
}

//level7表达式 andexpr
//<level7表达式> -> <level6表达式><level7表达式尾部>
VarElement* SyntacticParser::level7Expression() {
    //<level6表达式>
    VarElement* left_var = level6Expression();
    //<level7表达式尾部>
    return level7ExpressionTail(left_var);
}

//level7 tail andtail
//<level7表达式尾部> -> 逻辑与&& <level6表达式><level7表达式尾部> | 空
VarElement* SyntacticParser::level7ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_AND)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<level6表达式>
        VarElement* right_var = level6Expression();
        VarElement* result_var = codeGenerator.genTwoOperation(OP_AND, left_var, right_var);
        //<level7表达式尾部>
        return level7ExpressionTail(result_var);
    } else
    {
        cout << "In level7 expression tail null generated" << endl;
        return left_var;
    }
}

//level6表达式 cmpexpr
//<level6表达式> -> <level5表达式><level6表达式尾部>
VarElement* SyntacticParser::level6Expression() {
    //<level5表达式>
    VarElement* left_var = level5Expression();
    //<level6表达式尾部>
    return level6ExpressionTail(left_var);
}

//level6 tail cmptail
//<level6表达式尾部> -> <比较判断符号><level5表达式><level6表达式尾部> | 空
VarElement* SyntacticParser::level6ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_GREATER) || matchToken(DELIMITER_GREATER_EQUAL) || matchToken(DELIMITER_LESS) || matchToken(DELIMITER_LESS_EQUAL) || matchToken(DELIMITER_EQUAL) || matchToken(DELIMITER_NOT_EQUAL)){
        //<比较判断符号>
        Operator opt = level6Operation();
        //<level5表达式>
        VarElement* right_var = level5Expression();
        VarElement* result_var = codeGenerator.genTwoOperation(opt, left_var, right_var);
        //<level6表达式尾部>
        return level6ExpressionTail(result_var);
    } else{
        cout << "In level6Expression opt unmatched / NULL" << endl;
        return left_var;
    }
}

//比较判断符号 GT GE LT LE EQU NEQU
//<比较判断符号> level6操作符号
Operator SyntacticParser::level6Operation() {
    Tag tag = currentToken->tag;
    Operator opt = OP_NOP;
    //终结符匹配成功,获取下一个字符
    getNextToken();

    switch (tag) {
        case DELIMITER_GREATER:
            opt = OP_GREATER;
            break;
        case DELIMITER_GREATER_EQUAL:
            opt = OP_GREATER_EQUAL;
            break;
        case DELIMITER_LESS:
            opt = OP_LESS;
            break;
        case DELIMITER_LESS_EQUAL:
            opt = OP_LESS_EQUAL;
            break;
        case DELIMITER_EQUAL:
            opt = OP_EQUAL;
            break;
        case DELIMITER_NOT_EQUAL:
            opt = OP_NOT_EQUAL;
            break;
    }
    return opt;
}

//level5表达式 aloexpr
//<level5表达式> -> <level4表达式><level5表达式尾部>
VarElement* SyntacticParser::level5Expression() {
    //<level4表达式>
    VarElement* left_var = level4Expression();
    //<level5表达式尾部>
    return level5ExpressionTail(left_var);
}

//level5 tail alotail
//<level5表达式尾部> -> 加减操作+/- <level4表达式><level5表达式尾部> | 空
VarElement* SyntacticParser::level5ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_SUB) || matchToken(DELIMITER_ADD)){
        Operator opt = level5Operation();
        //<level4表达式>
        VarElement* right_var = level4Expression();
        VarElement* result_var = codeGenerator.genTwoOperation(opt, left_var, right_var);
        //<level5表达式尾部>
        return level5ExpressionTail(result_var);
    } else{
        cout << "In level5Expression opt unmatched / NULL" << endl;
        return left_var;
    }
}

//加减运算符 + -
//<加减操作> level5操作
Operator SyntacticParser::level5Operation() {
    Tag tag = currentToken->tag;
    Operator opt = OP_NOP;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    switch (tag) {
        case DELIMITER_ADD:
            opt = OP_ADD;
            break;
        case DELIMITER_SUB:
            opt = OP_SUB;
            break;
    }
    return opt;
}

//level4表达式 itemexpr
//<level4表达式> -> <因子表达式><level4表达式尾部>
VarElement* SyntacticParser::level4Expression() {
    //<因子表达式>
    VarElement* left_var = factorExpression();
    //<level4表达式尾部>
    return level4ExpressionTail(left_var);
}

//level4 tail itemtail
//<level4表达式尾部> -> 乘除求余*/% <因子表达式><level4表达式尾部> | 空
VarElement* SyntacticParser::level4ExpressionTail(VarElement* left_var) {
    if (matchToken(DELIMITER_MUL) || matchToken(DELIMITER_DIV) || matchToken(DELIMITER_MOD)){
        Operator opt = level4Operation();
        //<因子表达式>
        VarElement* right_var = factorExpression();
        VarElement* result_var = codeGenerator.genTwoOperation(opt, left_var, right_var);
        //<level4表达式尾部>
        return level4ExpressionTail(result_var);
    } else{
        cout << "In level4Expression opt unmatched / NULL" << endl;
        return left_var;
    }
}

//乘除求余 * / %
//<乘除求余*/%> level4操作
Operator SyntacticParser::level4Operation() {
    Tag tag = currentToken->tag;
    Operator opt = OP_NOP;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    switch (tag) {
        case DELIMITER_MUL:
            opt = OP_MUL;
            break;
        case DELIMITER_DIV:
            opt = OP_DIV;
            break;
        case DELIMITER_MOD:
            opt = OP_MOD;
            break;
    }
    return opt;
}

//因子表达式 factor
//<因子表达式> -> <level3操作><因子表达式> | <值表达式>
VarElement* SyntacticParser::factorExpression() {
    if (matchToken(DELIMITER_NOT) || matchToken(DELIMITER_SUB) || matchToken(DELIMITER_LEA) || matchToken(DELIMITER_MUL) || matchToken(DELIMITER_INC) || matchToken(DELIMITER_DEC)){
        Operator opt = level3Operation();
        //<因子表达式>
        VarElement* var = factorExpression();
        return codeGenerator.genOneOperation(opt, var);
    } else{
        return valueExpression();
    }
}

//<level3操作> -> ! - & * ++ --
Operator SyntacticParser::level3Operation() {
    Tag tag = currentToken->tag;
    Operator opt = OP_NOP;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    switch (tag) {
        case DELIMITER_NOT:
            opt = OP_NOT;
            break;
        case DELIMITER_SUB:
            opt = OP_NEGATIVE;
            break;
        case DELIMITER_LEA:
            opt = OP_LEA;
            break;
        case DELIMITER_MUL:
            opt = OP_POINTER_GET;
            break;
        case DELIMITER_INC:
            opt = OP_INC;
            break;
        case DELIMITER_DEC:
            opt = OP_DEC;
            break;
    }
    return opt;
}

//值表达式 val
//<值表达式> -> <元素表达式><level2操作>
VarElement* SyntacticParser::valueExpression() {
    //<元素表达式>
    VarElement* var = elementExpression();
    if (matchToken(DELIMITER_INC) || matchToken(DELIMITER_DEC)){
        Operator opt = level2Operation();
        var = codeGenerator.genOneOperationRight(opt, var);
    } else
        cout << "In value Expression opt unmatched" << endl;
    return var;
}

// ++ -- 后置
//<level2操作> -> ++ --(右侧)
Operator SyntacticParser::level2Operation() {
    Tag tag = currentToken->tag;
    Operator opt = OP_NOP;
    //终结符匹配成功,获取下一个字符
    getNextToken();
    switch (tag) {
        case DELIMITER_INC:
            opt = OP_INC;
            break;
        case DELIMITER_DEC:
            opt = OP_DEC;
            break;
    }
    return opt;
}

//表达式元素的表达式 elem
//<元素表达式> -> IDENTIFIER <元素表达式尾部> | ( <表达式> ) | <常量>
VarElement* SyntacticParser::elementExpression() {
    VarElement* var = nullptr;
    string name;
    if (matchToken(IDENTIFIER)){
        //写入name
        name = currentToken->getName();
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<元素表达式尾部>
        var = elementExpressionTail(name);
    } else if (matchToken(DELIMITER_LEFT_PAREN)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<表达式>
        var = expression();
        if (matchToken(DELIMITER_RIGHT_PAREN))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In element Expression ) unmatched" << endl;
    } else if (matchToken(CONSTANT_NUM) || matchToken(CONSTANT_CHAR) || matchToken(CONSTANT_STRING)){
        var = constant();
    } else
        cout << "In element Expression unknown unmatched" << endl;
    return var;
}

//元素表达式尾部 idxpr
//<元素表达式尾部> -> [ <表达式> ] | ( <函数实参列表> ) | 空
VarElement* SyntacticParser::elementExpressionTail(string name) {
    VarElement* var = nullptr;
    if (matchToken(DELIMITER_LEFT_BRACKET)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<表达式>
        VarElement* index = expression();
        if (matchToken(DELIMITER_RIGHT_BRACKET))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In elementExpressionTail ] unmatched" << endl;
        //从符号表中获取数组
        VarElement* array = symbolTable.getVariable(name, symbolTable.getScopePath());
        //产生数组索引运算的中间代码
        var = codeGenerator.genArrayIndex(array, index);

    } else if (matchToken(DELIMITER_LEFT_PAREN)){
        //创建参数列表
        vector<VarElement*> args;
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<函数实参列表>
        funRealParameterList(args);
        if (matchToken(DELIMITER_RIGHT_PAREN))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In elementExpressionTail ) unmatched" << endl;

        //根据函数名称和参数获取函数
        FunElement* fun = symbolTable.getFunction(name, args);
        //产生函数调用中间代码
        var = codeGenerator.genFunctionCall(fun, args);
    } else{
        cout << "In elementExpressionTail null generated" << endl;
        var = symbolTable.getVariable(name, symbolTable.getScopePath());
    }
    return var;
}

//常量 literal
//<常量> -> 数字 字符 字符串
VarElement* SyntacticParser::constant() {
    VarElement* var = nullptr;
    if (matchToken(CONSTANT_NUM) || matchToken(CONSTANT_CHAR) || matchToken(CONSTANT_STRING)){
        var = new VarElement(currentToken);
        //添加至符号表中
        if (matchToken(CONSTANT_STRING))
            symbolTable.addString(var);
        else
            symbolTable.addVariable(var);
        //终结符匹配成功,获取下一个字符
        getNextToken();
    }
    return var;
}

//函数实参列表 realarg
//<函数实参列表> -> <实参><多个实参尾部> | 空
void SyntacticParser::funRealParameterList(vector<VarElement*>& args) {
    //函数实参列表为空的情况
    if (matchToken(DELIMITER_RIGHT_PAREN))
        return;
    else{
        //<实参>
        VarElement* arg = singleRealParameter();
        args.push_back(arg);
        //<多个实参尾部>
        multiRealParameterTail(args);
    }
}

//（单个）实参 arg
//<实参> -> <表达式>
VarElement* SyntacticParser::singleRealParameter() {
    return expression();
}

//多个实参尾部 arglist
//<多个实参尾部> -> 逗号"," <实参> <多个实参尾部> | 空
void SyntacticParser::multiRealParameterTail(vector<VarElement*>& args) {
    if (matchToken(DELIMITER_COMMA)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        VarElement* arg = singleRealParameter();
        args.push_back(arg);
        //<多个实参尾部>
        multiRealParameterTail(args);
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
            //中间代码产生continue语句
            codeGenerator.genContinue();
            //终结符匹配成功,获取下一个字符
            getNextToken();
            if (matchToken(DELIMITER_SEMICOLON))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In Statement kw_continue ; unmatched" << endl;
            break;
        case KW_BREAK:
            //中间代码产生break语句
            codeGenerator.genBreak();
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
            //<表达式>, 并且用变量保存返回值
            auto* return_value = expression();
            //生成中间代码
            codeGenerator.genReturn(return_value);
            //semicolon;
            if (matchToken(DELIMITER_SEMICOLON))
                //终结符匹配成功,获取下一个字符
                getNextToken();
            else
                cout << "In Statement kw_return ; unmatched" << endl;
            break;
    }
}

//<while> -> KW_WHILE 左小括号( <表达式> 右小括号) <函数块>
void SyntacticParser::whileStatement() {
    symbolTable.scopeEnter();           //符号表作用域管理
    //创建入口和出口的中间代码
    InterInstruction* t_while;
    InterInstruction* t_while_exit;
    codeGenerator.genWhileHead(t_while, t_while_exit);  //生成入口和出口的中间代码
    //终结符KW_WHILE匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In while statement ( unmatched" << endl;
    /***<表达式>****/
    //将循环条件保存在变量中
    auto* condition = expression();
    //中间代码生成
    codeGenerator.genWhileCondition(condition, t_while_exit);
    /***<表达式>****/
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In while statement ( unmatched" << endl;
    //<函数块>
    funBlock();
    //产生while尾部的中间代码
    codeGenerator.genWhileTail(t_while, t_while_exit);
    //退出局部作用域
    symbolTable.scopeExit();
}

//<for> -> KW_FOR 左小括号( <初始条件> <循环判断条件> <循环操作> 右小括号) <函数块>
void SyntacticParser::forStatement() {
    //符号表进入局部作用域
    symbolTable.scopeEnter();
    //创建for语句的label标签
    InterInstruction* t_for;
    InterInstruction* t_for_exit;
    InterInstruction* t_step;
    InterInstruction* t_for_block;
    //终结符KW_FOR匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forStatement ( unmatched" << endl;
    //<初始条件>
    forInitStatement();
    //中间代码 产生入口和出口的中间代码
    codeGenerator.genForHead(t_for, t_for_exit);
    //<循环判断条件> 将循环条件保存在变量中
    auto* condition =  forJudgeStatement();
    //中间代码 产生for循环条件开始部分
    codeGenerator.genForConditionHead(condition, t_step, t_for_block, t_for_exit);
    //<循环操作>
    forOperationStatement();
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forStatement ) unmatched" << endl;
    //中间代码 产生循环条件结束部分
    codeGenerator.genForConditionTail(t_for, t_for_block);
    //<函数块>
    funBlock();
    //中间代码 产生for循环尾部
    codeGenerator.genForTail(t_step, t_for_exit);
    //退出局部作用域
    symbolTable.scopeExit();
}

//<初始条件> -> <局部变量声明/定义> | <表达式> 分号";"
void SyntacticParser::forInitStatement() {
    if (matchTypeFirstSet())
        localVarDecOrDef();
    else if (matchExpressionFirstSet()){
        //<表达式>
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
VarElement* SyntacticParser::forJudgeStatement() {
    auto* condition = expression();
    if (matchToken(DELIMITER_SEMICOLON))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In forJudgeStatement ; unmatched" << endl;
    return condition;
}

//<循环操作> -> <表达式>
void SyntacticParser::forOperationStatement() {
    expression();
}

//<do-while> -> KW_DO <函数块> KW_WHILE 左小括号( <表达式> 右小括号) 分号";"
void SyntacticParser::doWhileStatement() {
    //符号表局部作用域管理
    symbolTable.scopeEnter();
    //创建入口和出口标签
    InterInstruction* t_do;
    InterInstruction* t_do_exit;
    codeGenerator.genDoWhileHead(t_do, t_do_exit);
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
    //do while语句局部作用域退出
    symbolTable.scopeExit();
    //<表达式> 使用变量保存循环条件
    auto* condition = expression();
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
    //产生do while语句尾部
    codeGenerator.genDoWhileTail(condition, t_do, t_do_exit);
}

//<if> -> KW_IF 左小括号( <表达式> 右小括号) <函数块> <else>
void SyntacticParser::ifStatement() {
    //进入局部作用域
    symbolTable.scopeEnter();
    //定义for语句的label
    InterInstruction* t_else;
    InterInstruction* t_exit;
    //终结符KW_IF匹配成功,获取下一个字符
    getNextToken();
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In ifStatement ( unmatched" << endl;
    //<表达式>, 并将if条件保存在变量中
    auto* condition = expression();
    codeGenerator.genIfHead(condition, t_else);
    if (matchToken(DELIMITER_RIGHT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In ifStatement ) unmatched" << endl;
    //<函数块>
    funBlock();
    //离开局部作用域
    symbolTable.scopeExit();
    //<else>
    //todo else部分 冗余删除算法？
    if (matchToken(KW_ELSE)){
        //中间代码，创建else标签
        codeGenerator.genElseHead(t_else, t_exit);
        ifElseStatement();
        codeGenerator.genElseTail(t_exit);
    } else
        codeGenerator.genIfTail(t_else);
}

//<else> -> KW_ELSE <函数块> | 空
void SyntacticParser::ifElseStatement() {
    //KW_ELSE
    if (matchToken(KW_ELSE)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //进入局部作用域
        symbolTable.scopeEnter();
        funBlock();
        //退出局部作用域
        symbolTable.scopeExit();
    } else{
        cout << "In ifElseStatement null generated" << endl;
        return;
    }
}

//<switch> -> KW_SWITCH 左小括号( <表达式> 右小括号) 左大括号{ <switch-case> 右大括号}
void SyntacticParser::switchStatement() {
    //进入局部作用域
    symbolTable.scopeEnter();
    //创建出口label
    InterInstruction* t_exit;
    //产生出口的中间代码
    codeGenerator.genSwitchHead(t_exit);
    //终结符KW_SWITCH匹配成功,获取下一个字符
    getNextToken();
    //左小括号(
    if (matchToken(DELIMITER_LEFT_PAREN))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement ( unmatched" << endl;
    //<表达式> 条件保存在变量中
    auto* condition = expression();
    if (condition->getIsPointed())
        condition = codeGenerator.genPointerAssign(condition);
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
    switchCaseStatement(condition);
    //右大括号}
    if (matchToken(DELIMITER_RIGHT_BRACE))
        //终结符匹配成功,获取下一个字符
        getNextToken();
    else
        cout << "In switchStatement } unmatched" << endl;
    //产生switch尾部的中间代码
    codeGenerator.genSwitchTail(t_exit);
    //离开局部作用域
    symbolTable.scopeExit();
}

//<switch-case> -> KW_CASE <常量> 冒号":" <函数子块> <switch-case> | KW_DEFAULT 冒号":" <函数子块>
void SyntacticParser::switchCaseStatement(VarElement* condition) {
    if (matchToken(KW_CASE)){
        //创建case的出口指令
        InterInstruction* t_case_exit;
        //终结符匹配成功,获取下一个字符
        getNextToken();
        //<常量> case的var保存
        auto* case_label = constant();
        //创建case头部中间代码
        codeGenerator.genCaseHead(condition, case_label, t_case_exit);

        if (matchToken(DELIMITER_COLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In switchStatement case : unmatched" << endl;

        //进入局部作用域
        symbolTable.scopeEnter();
        //<函数子块>
        funSubBlock();
        //离开局部作用域
        symbolTable.scopeExit();
        //创建中间代码 case尾部
        codeGenerator.genCaseTail(t_case_exit);
        //<switch-case>
        switchCaseStatement(condition);
    } else if (matchToken(KW_DEFAULT)){
        //终结符匹配成功,获取下一个字符
        getNextToken();
        if (matchToken(DELIMITER_COLON))
            //终结符匹配成功,获取下一个字符
            getNextToken();
        else
            cout << "In switchStatement default : unmatched" << endl;
        //进入局部作用域
        symbolTable.scopeEnter();
        //<函数子块>
        funSubBlock();
        //离开局部作用域
        symbolTable.scopeExit();

    } else
        cout << "In switchCaseStatement unmatched" << endl;
}






