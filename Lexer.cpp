//
// Created by 95792 on 2020/4/9.
//

#include "Lexer.h"

/***************************************************/
/****************扫描器函数具体实现******************/
/***************************************************/

//扫描器的构造函数，需要输入文件名字来作为参数
Scanner::Scanner(const char *name){
    file = fopen(name, "r");
    if (!file){
        printf("File %s open failed !\n", name);
    }
    file_name = name;
    //初始化扫描器参数
    line_length = 0;
    read_position = -1;
    pre_char = 0;
    row_number = 1;
    col_number = 0;
}

Scanner::~Scanner() {
    if (file){
        printf("File have not been scanned yet! \n");
        fclose(file);
    }
}

//void Scanner::showChar(char ch) {
//    if (ch == -1)
//        printf("File EOF!");
//    else if (ch == '\n')
//        printf("\\n");
//    else if (ch == '\t')
//        printf("\\t");
//    else if (ch == ' ')
//        printf("blank\n");
//    else
//        printf("%c",ch);
//    printf("%d\n", ch);
//}

char Scanner::scan() {
    if (!file){
        printf("Scanner does not found file\n");
        return -1;
    }

    if (read_position == line_length - 1){
        line_length = fread(buffer, 1, BUFFER_LENGTH, file);
        //缓存区读不到数据了
        if (line_length == 0){
            //字符-1代表文件读取结束
            line_length = 1;
            buffer[0] = -1;
        }
        //将读取位置初始化
        read_position = -1;
    }
    //读取位置向后移动
    read_position ++;
    char current_char = buffer[read_position];
    if (pre_char == '\n'){
        row_number ++;
        col_number = 0;
    }
    //读取到文件结束标志，关闭文件，释放file变量
    if (current_char == -1){
        fclose(file);
        file = nullptr;
    } else if (current_char != '\n'){
        col_number ++;
    }
    pre_char = current_char;
    //TODO compiler 输出参数 showChar
    return current_char;
}

//获取文件名字
const char* Scanner::getFileName() {
    return file_name;
}

//获取行号
int Scanner::getRowNumber() {
    return row_number;
}

//获取列号
int Scanner::getColumnNumber() {
    return col_number;
}


/***************************************************/
/****************关键词列表具体实现******************/
/***************************************************/
Keywords::Keywords() {
    //函数和IO关键词
    keywords["main"] = KW_MAIN;
    keywords["scanf"] = KW_SCANF;
    keywords["printf"] = KW_PRINTF;
    //数据类型关键词
    keywords["int"] = KW_INT;
    keywords["char"] = KW_CHAR;
    keywords["bool"] = KW_BOOL;
    keywords["float"] = KW_FLOAT;
    keywords["double"] = KW_DOUBLE;
    keywords["const"] = KW_CONST;
    keywords["void"] = KW_VOID;
    keywords["extern"] = KW_EXTERN;
    //控制结构关键词
    keywords["if"] = KW_IF;
    keywords["else"] = KW_ELSE;
    keywords["switch"] = KW_SWITCH;
    keywords["case"] = KW_CASE;
    keywords["default"] = KW_DEFAULT;
    keywords["while"] = KW_WHILE;
    keywords["do"] = KW_DO;
    keywords["for"] = KW_FOR;
    keywords["break"] = KW_BREAK;
    keywords["continue"] = KW_CONTINUE;
    keywords["return"] = KW_RETURN;
}

Tag Keywords::getTag(string tokenName) {
    //如果在hash表中有对应的值，则返回
    if (keywords.find(tokenName) != keywords.end())
        return keywords[tokenName];
    //若查询不到对应的值，则将tokenName的视为identifier
    else
        return IDENTIFIER;
}

/***************************************************/
/****************词法解析器的函数具体实现**************/
/***************************************************/
Keywords Lexer::keywords;

Lexer::Lexer(Scanner &sc):scanner(sc){
    token = nullptr;
    character = scanner.scan();
}

Lexer::~Lexer() {
    if (!token)
    {
        delete token;
    }
}

bool Lexer::scan(char need) {
    character = scanner.scan();
    //TODO 检查该函数need的必要性
    if(need){
        if (character != need)
            return false;
        else{
            character = scanner.scan();
            return true;
        }
    }
    return true;
}

Token* Lexer::getToken() {
    Token *final = nullptr;
    //初始化读入扫描
    while (character != -1)
    {
        /*****每次循环结束后要保证character为下一个token的首字符******/
        Token *temp = nullptr;
        //过滤掉空格、tab和回车
        while (character == ' ' || character == '\n' || character == '\t')
            scan();

        string tokenName;
        //识别标识符和关键字 (_|a-z|A-Z)(a-z|A-Z|0-9)*
        if (character == '_' || (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'))
        {
            do{
                tokenName.push_back(character);
                scan();
            }while ((character >= '0' && character <= '9') || character == '_' ||
                    (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z'));

            //获取对应的tag 判断是keyword还是id
            Tag tag = keywords.getTag(tokenName);
            //这里有个坑需要注意 new出来的是一段空间的首地址，需要用指针变量来存放。
            if (tag == IDENTIFIER)
                temp = new Id(tokenName);
            else
                temp = new Token(tag);
        }
        //识别数字类型
        else if (character >= '0' && character <= '9'){
            int num_value = 0;
            //识别十进制，非0开头的数字
            if (character != '0'){
                do{
                    num_value = num_value * 10 + (character - '0');
                    scan();
                }while (character >= '0' && character <= '9');
            }
            else{
                //以0开头，有四种情况，16进制和8进制和2进制，以及0本身
                //扫描下一个字符
                scan();
                if (character == 'x' || character == 'X'){
                    scan();
                    if ((character >= 0 && character <= 9) || (character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F')){
                        do{
                            num_value = num_value * 16;
                            //这里要注意16进制中 a-f 字符的特殊性
                            if (character >= '0' && character <= '9')
                                num_value = num_value + character - '0';
                            else if (character >= 'a' && character <= 'f')
                                num_value = num_value + character - 'a';
                            else
                                num_value = num_value + character - 'A';
                            scan();
                        }while ((character >= '0' && character <= '9') || (character >= 'a' && character <= 'f') || (character >= 'A' && character <= 'F'));
                    } else{
                        //0x后接的不是16进制的情况
                        printf("Character After 0x is not a HEX number\n");
                        temp = new Token(ERR);
                    }
                }
                //二进制数字
                else if (character == 'b' || character == 'B'){
                    scan();
                    if (character == '0' || character == '1'){
                        do{
                            num_value = num_value * 2 + character - '0';
                            scan();
                        }while (character == '0' || character == '1');
                    } else{
                        printf("Character After 0b is not a HEX number\n");
                        temp = new Token(ERR);
                    }
                }
                //八进制数字
                else if (character >= '0' && character <= '7'){
                    do{
                        num_value = num_value * 8 + character - '0';
                        scan();
                    }while (character >= '0' && character <= '7');
                }
            }
            //temp没有被初始化的情况（）
            if (!temp)
                temp = new Num(num_value);
        }
        //识别字符串
        else if (character == '"'){
            //TODO 字符串的识别
            string str_value;
            scan();
            while (character != '"'){
                //考虑异常情况
                if (character == '\n' || character == -1){
                    temp = new Token(ERR);
                    cout << "String token err" << endl;
                    break;
                }
                //转义字符的处理
                if (character == '\\'){
                    scan();
                    if (character == '\\')
                        str_value.push_back('\\');
                    else if (character == 't')
                        str_value.push_back('\t');
                    else if (character == '0')
                        str_value.push_back('\0');
                    else if (character == '"')
                        str_value.push_back('\"');
                    else if (character == 'n')
                        str_value.push_back('\n');
                    else if (character == '\n')
                        cout << "String token goto next line" << endl;
                    else
                        str_value.push_back(character);
                    //指向转义后的下一个字符
                    scan();
                } else{
                    //非转义的正常字符
                    str_value.push_back(character);
                    scan();
                }

            }
            if (!temp){
                temp = new Str(str_value);
                //保证character指向下一个token的开头字符
                scan();
            }
        }
        //识别字符
        else if (character == '\''){
            //TODO 字符的识别
            char char_value = 0;
            scan();
            if (character == '\n' || character == -1)
            {
                temp = new Token(ERR);
                cout << "Char Token get error" << endl;
            }

            //转义字符的处理
            if (character == '\\'){
                scan();
                if (character == '\\')
                    char_value = '\\';
                else if (character == 't')
                    char_value = '\t';
                else if (character == '0')
                    char_value = '\0';
                else if (character == '\'')
                    char_value = '\'';
                else if (character == 'n')
                    char_value = '\n';
                else if (character == -1 || character == '\n'){
                    temp = new Token(ERR);
                    cout << "Char Token get error" << endl;
                }
                else
                    char_value = character;

                //匹配'\''
                scan();
            }
            //由于空字符由'\0'定义 故''是非法的
            else if (character == '\'')
            {
                temp = new Token(ERR);
                cout << "Empty Character Error" << endl;
                //读取下一个字符
                scan();
            }
            //非转义、非空的正常字符
            else{
                char_value = character;
                //读取下一个字符 匹配'\''
                scan();
            }

            if (!temp){
                //右侧'\''的读取
                if (character == '\''){
                    temp = new Character(char_value);
                    //读取下一个字符
                    scan();
                }
                else{
                    temp = new Token(ERR);
                    cout << "Character Length Error" << endl;
                    scan();
                }

            }
        }
        //识别界符号
        else{
            switch (character){
                case '+':
                    scan();
                    if (character == '+')
                        temp = new Token(DELIMITER_INC);
                    else
                        temp = new Token(DELIMITER_ADD);
                    break;
                case '-':
                    scan();
                    if (character == '-')
                        temp = new Token(DELIMITER_DEC);
                    else
                        temp = new Token(DELIMITER_SUB);
                    break;
                case '*':
                    scan();
                    temp = new Token(DELIMITER_MUL);
                    break;
                case '/':
                    scan();
                    //TODO 注释部分的解析
                    temp = new Token(DELIMITER_DIV);
                    break;
                case '%':
                    scan();
                    temp = new Token(DELIMITER_MOD);
                    break;
                case '>':
                    scan();
                    if (character == '=')
                        temp = new Token(DELIMITER_GREATER_EQUAL);
                    else
                        temp = new Token(DELIMITER_GREATER);
                    break;
                case '<':
                    scan();
                    if (character == '=')
                        temp = new Token(DELIMITER_LESS_EQUAL);
                    else
                        temp = new Token(DELIMITER_LESS);
                    break;
                case '=':
                    scan();
                    if (character == '=')
                        temp = new Token(DELIMITER_EQUAL);
                    else
                        temp = new Token(DELIMITER_ASSIGN);
                    break;
                case '&':
                    scan();
                    if (character == '&')
                        temp = new Token(DELIMITER_AND);
                    else
                        temp = new Token(DELIMITER_LEA);
                    break;
                case '|':
                    scan();
                    if (character == '|')
                        temp = new Token(DELIMITER_OR);
                    else{
                        temp = new Token(ERR);
                        printf(" | delimiter error\n");
                    }
                    break;
                case '!':
                    scan();
                    if (character == '=')
                        temp = new Token(DELIMITER_NOT_EQUAL);
                    else
                        temp = new Token(DELIMITER_NOT);
                    break;
                case ',':
                    scan();
                    temp = new Token(DELIMITER_COMMA);
                    break;
                case ':':
                    scan();
                    temp = new Token(DELIMITER_COLON);
                    break;
                case ';':
                    scan();
                    temp = new Token(DELIMITER_SEMICOLON);
                    break;
                case '{':
                    scan();
                    temp = new Token(DELIMITER_LEFT_BRACE);
                    break;
                case '}':
                    scan();
                    temp = new Token(DELIMITER_RIGHT_BRACE);
                    break;
                case '[':
                    scan();
                    temp = new Token(DELIMITER_LEFT_BRACKET);
                    break;
                case ']':
                    scan();
                    temp = new Token(DELIMITER_RIGHT_BRACKET);
                    break;
                case '(':
                    scan();
                    temp = new Token(DELIMITER_LEFT_PAREN);
                    break;
                case ')':
                    scan();
                    temp = new Token(DELIMITER_RIGHT_PAREN);
                    break;
                case -1:
                    scan();
                    //创建文件结束token
                    temp = new Token(FILE_END);
                    printf("File open failed or File to End\n");
                    break;
                default:
                    //位置的情况 按错误处理
                    scan();
                    temp = new Token(ERR);
            }
        }

        //释放原对象中的token
        delete token;
        token = temp;
        if (token && token->tag != ERR)
            return token;
        else{
            cout << "token null or error" << endl;
            continue;
        }
    }
    if (character == -1)
        final = new Token(FILE_END);
    return final;
}