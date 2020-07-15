//
// Created by 95792 on 2020/6/1.
//

#include "VarElement.h"
#include "SymbolTable.h"
#include "GenerateCode.h"

/*****定义的字符串输出，将tag枚举值转化为对应的字符串*****/
const char * varTagToString[] = {
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

//变量创建初始化
void VarElement::create_init() {
    scopePath.push_back(-1);
    isArray = false;
    isConstant = false;
    isLeft = true;
    isPointer = false;
    pointerValue = nullptr;
    initialed = false;
    initData = nullptr;
    size = 0;
    offset = 0;
    //优化部分信息初始化
    list_index = -1;
    live = false;
    register_id = -1;
    in_memory = false;
}

/****变量元素*****/
//创建空void特殊变量
VarElement::VarElement() {
    create_init();
    name = "[void]";
    setLeft(false);
    //void变量的特征
    type = KW_VOID;
    intValue = 0;
    //不是基本变量类型
    isPointer = true;
    //不是常量
    isConstant = false;
}

//VarElement::VarElement(vector<int> &sp, Tag t, bool b) {
//
//}

//特殊变量 0 1 4的创建
VarElement::VarElement(int num) {
    create_init();
    name = "[consInt]";
    //0 1 4变量的特征
    setLeft(false);
    isConstant = true;
    intValue = num;
    setType(KW_INT);
}

//(指针)变量的创建
VarElement::VarElement(vector<int>& sp, Tag t, string n, bool isPtr, VarElement* init) {
    create_init();

    scopePath = sp;
    setType(t);
    setIsPointer(isPtr);
    name = n;
    initData = init;
//    if (isPtr)
//        setPointerValue(init);
}

//(数组)变量的创建
VarElement::VarElement(vector<int> &sp, Tag t, string n, int len) {
    create_init();

    scopePath = sp;
    setType(t);
    name = n;
    setArray(len);
}

//常量对象的创建
VarElement::VarElement(Token *token) {
    create_init();

    isConstant = true;
    setLeft(false);
    switch (token->tag) {
        case CONSTANT_NUM:
            setType(KW_INT);
            name = "[consInt]";
            intValue = ((Num*)token)->num_value;
            break;
        case CONSTANT_CHAR:
            setType(KW_CHAR);
            name = "[consChar]";
            charValue = ((Character*)token)->character_value;
            break;
        case CONSTANT_STRING:
            setType(KW_CHAR);
            //todo 名字的重新产生 单独 .h文件
            name = GenerateCode::genLabel();
            stringValue = ((Str*)token)->string_value;
            //采用字符数组形式存储字符串
            setArray((stringValue.size())+1);
            break;
        default:
            cout << "In create constant var element type error" << endl;
            break;
    }
}

//拷贝一个临时变量
VarElement::VarElement(vector<int> &sp, VarElement *var) {
    create_init();

    scopePath = sp;
    setType(var->getVarType());
    setIsPointer(var->getIsPointer() || var->getIsArray());
    name = "[temp]";
    setLeft(false);
}


//临时变量的创建
VarElement::VarElement(vector<int> &sp, Tag t, bool isPtr) {
    create_init();

    scopePath = sp;
    setType(t);
    setIsPointer(isPtr);
    setLeft(false);
    name = "[temp]";
}

VarElement::~VarElement() = default;

string VarElement::getVarName() {
    return name;
}

//获取变量类型
Tag VarElement::getVarType() {
    return type;
}

//获取变量作用域
vector<int> &VarElement::getScopePath() {
    return scopePath;
}

//获取字符常量值
string VarElement::getStrConstantValue() {
    return stringValue;
}

VarElement *VarElement::getPointer() {
    return pointerValue;
}

//判断是基本类型——不是指针，不是数组
bool VarElement::isBasicType() {
    return !isPointer && !isArray;
}

bool VarElement::getIsPointer() {
    return isPointer;
}

bool VarElement::getIsPointed() {
    return pointerValue != nullptr;
}

VarElement *VarElement::getInitData() {
    return initData;
}

bool VarElement::getIsArray() {
    return isArray;
}

bool VarElement::getIsConstant() {
    return isConstant && isBasicType();
}

bool VarElement::getIsLeft() {
    return isLeft;
}

bool VarElement::getIsVoid() {
    return type == KW_VOID;
}

bool VarElement::getIsInited() {
    return initialed;
}

int VarElement::getVarSize() {
    return size;
}

//设置是否是指针
void VarElement::setIsPointer(bool isPtr) {
    isPointer = isPtr;
    //指针变量4个字节
    if (isPointer)
        size = 4;
}

//设置局部变量偏移
void VarElement::setOffset(int off) {
    this->offset = off;
}

void VarElement::setPointerValue(VarElement *var) {
    this->pointerValue = var;
}

void VarElement::setLeft(bool flag) {
    this->isLeft = flag;
}

//设定变量初始化
bool VarElement::setInit() {
    //取出初始化数据
    VarElement* init = initData;
    if (!init)
        return false;
    //默认尚未初始化
    initialed = false;
    //初始化变量类型检查
    if (!GenerateCode::typeCheck(this, init))
        cout << "Var init type check failed" << endl;
    else if (init->getIsConstant()){
        //常量进行初始化
        initialed = true;
        if (init->getIsArray())
            charPointerValue = init->getVarName();
        else{
            //基本类型
            intValue = init->intValue;
            charValue = init->charValue;
        }
    } else {
        //初始化的值不是常量
        if (scopePath.size() == 1)
            cout << "global var init value must be constant" << endl;
        else
            return true;
    }
    return false;
}

void VarElement::setType(Tag t) {
    type = t;
    //void类型特殊处理
    if (type == KW_VOID){
        cout << "error! Type of var cannot be VOID" << endl;
        type = KW_INT;
    }

    if (type == KW_INT)
        size = 4;
    else if (type == KW_CHAR)
        size = 1;
}


//设置数组，参数为数组的长度
void VarElement::setArray(int l) {
    if (l <= 0){
        //todo 语义错误 数组长度小于等于0
        return;
    } else {
        isArray = true;
        isLeft = false;
        array_length = l;
        //变量大小 = 变量类型大小 * 数组长度
        size = size * array_length;
    }
}

void VarElement::generateName() {
    //todo 随机生成变量名字
}

VarElement *VarElement::getStep(VarElement *var) {
    //根据var的类型确定步长
    //先判断是否是基本类型
    if (var->isBasicType())
        return SymbolTable::num_one;
    //非基本类型则为pointer
    if (var->getVarType() == KW_INT)
        return SymbolTable::num_four;
    else if (var->getVarType() == KW_CHAR)
        return SymbolTable::num_one;
    else
        return nullptr;
}

//输出显示var的相关信息
void VarElement::showInformation() {
    //输出类型
    cout << varTagToString[type] << " ";
    //输出指针相关信息
    if (isPointer)
        cout << " " << "*" << " ";
    //输出名字
    cout << name << " ";
    //输出数组
    if (isArray)
        cout << "[" << array_length << "]" << " ";
    //输出初始化相关信息
    if (initialed){
        cout << "=";
        if (type == KW_INT)
            cout << intValue << " ";
        else if (type == KW_CHAR){
            if (isPointer)
                cout << charPointerValue << " ";
            else
                cout << charValue << " ";
        }
    }
    //输出大小相关信息
    cout << " " << "size= " << size << " ";
    cout << "scope=\"";
    for (int i = 0; i < scopePath.size(); ++i) {
        cout << scopePath[i] << "/";
    }
    cout << "\"" << " ";

    if (name[0] == '[')
        cout << "address=<" << name << "> ";
    else{
        //输出局部或全局变量的偏移信息
        if (offset > 0)
            cout << "address=[ebp +" << offset << "]" << endl;
        else if (offset < 0)
            cout << "address=[ebp " << offset << "]" << endl;
        else
            cout << "Value=" << intValue << endl;
    }
    //输出存储位置
    showRegisterInfo();
}

void VarElement::showInterValue() {
    if (isConstant){
        if (type == KW_INT)
            cout << intValue << " ";
        else if (type == KW_CHAR){
            if (isArray)
                cout << name << " ";       //字符数组，字符串
            else
                cout << charValue << " ";  //普通字符值
        }
    } else
        cout << name << " ";
}

int VarElement::getConstantValue() {
    return intValue;
}

void VarElement::showRegisterInfo() {
    cout<< "VarElement: " << name << " registerId = " << register_id << " InMemory: " << in_memory << endl;
}

int VarElement::getOffset() {
    return offset;
}

string VarElement::getCharPointerValue() {
    return charPointerValue;
}

string VarElement::getRawString() {
    stringstream ss;
    int len = stringValue.length();
    for (int i = 0,chpass = 0; i < len; ++i) {
        if (stringValue[i] == 10 || stringValue[i] == 9 || stringValue[i] == '\"' || stringValue[i] == '\0'){
            if (chpass == 0){
                if (i != 0)
                    ss << ",";
                ss << (int)stringValue[i];
            } else
                ss << "\"," << (int)stringValue[i];
            chpass = 0;
        } else {
            if (chpass == 0){
                if (i != 0)
                    ss << ",";
                ss << "\"" << stringValue[i];
            } else
                ss << stringValue[i];
            if (i == len - 1)
                ss << "\"";
            chpass = 1;
        }
    }
    ss << ",0";
    return ss.str();
}



































