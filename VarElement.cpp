//
// Created by 95792 on 2020/6/1.
//

#include "VarElement.h"
/****变量元素*****/
//(指针)变量的创建
VarElement::VarElement(vector<int>& sp, Tag t, string n, bool isPtr) {
    isArray = false;
    array_length = 0;
    isConstant = false;
    isLeft = true;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    scopePath = sp;
    type = t;
    name = n;
    isPointer = isPtr;
}

//(数组)变量的创建
VarElement::VarElement(vector<int> &sp, Tag t, string n, int len) {
    isConstant = false;
    isLeft = true;
    isPointer = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    isArray = true;
    array_length = len;
    scopePath = sp;
    type = t;
    name = n;
}

//常量对象的创建
VarElement::VarElement(Token *token) {
    isArray = false;
    array_length = 0;
    isPointer = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    isConstant = true;
    isLeft = false;
    switch (token->tag) {
        case CONSTANT_NUM:
            type = KW_INT;
            name = "[consInt]";
            intValue = ((Num*)token)->num_value;
            break;
        case CONSTANT_CHAR:
            type = KW_CHAR;
            name = "[consChar]";
            charValue = ((Character*)token)->character_value;
            break;
        case CONSTANT_STRING:
            type = KW_CHAR;
            stringValue = ((Str*)token)->string_value;
            //采用字符数组形式存储字符串
            isArray = true;
            isLeft = true;
            array_length = stringValue.size() + 1;
            //todo size设置
            break;
        default:
            cout << "In create constant var element type error" << endl;
            break;
    }
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

bool VarElement::isBasicType() {
    return type == KW_INT || type == KW_CHAR;
}

//设置局部变量偏移
void VarElement::setOffset(int off) {
    offset = off;
}




