//
// Created by 95792 on 2020/6/1.
//

#include "VarElement.h"
#include "SymbolTable.h"

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
}

/****变量元素*****/
//创建空void特殊变量
VarElement::VarElement() {
    isArray = false;
    array_length = 0;
    isPointer = false;
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
    //void变量的特征
    type = KW_VOID;
    isLeft = false;
    name = "[void]";
}

//VarElement::VarElement(vector<int> &sp, Tag t, bool b) {
//
//}

//特殊变量 0 1 4的创建
VarElement::VarElement(int num) {
    isArray = false;
    array_length = 0;
    isPointer = false;
    isConstant = false;
    initialed = false;
    initData = nullptr;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    offset = 0;
    //0 1 4变量的特征
//    type = KW_INT;
    isLeft = false;
    isConstant = true;
    intValue = num;
//    size = 4;
    setType(KW_INT);
    name = "[consInt]";
}

//(指针)变量的创建
VarElement::VarElement(vector<int>& sp, Tag t, string n, bool isPtr, VarElement* init) {
    isArray = false;
    array_length = 0;
    isConstant = false;
    isLeft = true;
    initialed = false;
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
    initData = init;
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

//拷贝一个临时变量
VarElement::VarElement(vector<int> &sp, VarElement *var) {
    isArray = false;
    array_length = 0;
    isConstant = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;
    size = 0;
    offset = 0;

    scopePath = sp;
    type = var->getVarType();
    name = "[temp]";
    isPointer = var->getIsPointer() || var->getIsArray();
    isLeft = false;
}


//临时变量的创建
VarElement::VarElement(vector<int> &sp, Tag t, bool isPtr) {
    isConstant = false;
    isLeft = true;
    isPointer = false;
    initialed = false;
    initData = nullptr;
    intValue = 0;
    charValue = -1;
    stringValue = "";
    pointerValue = nullptr;

    offset = 0;
    isArray = false;
    array_length = 0;

    scopePath = sp;
    type = t;
    name = "[temp]";
    if (isPtr){
        isPointer = true;
    } else{
        if (t == KW_INT)
            size = 4;
        else if (t == KW_CHAR)
            size = 1;
        else
            size = 0;
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

bool VarElement::getIsArray() {
    return isArray;
}

bool VarElement::getIsConstant() {
    return isConstant;
}

bool VarElement::getIsLeft() {
    return isLeft;
}

bool VarElement::getIsVoid() {
    return type == KW_VOID;
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

void VarElement::setType(Tag t) {
    type = t;
    if (type == KW_INT)
        size = 4;
    else if (type == KW_CHAR)
        size = 1;
    else if (type == KW_VOID){
        //TODO 语法报错 void赋值
        type = KW_INT;
        size = 4;
    }
}

void VarElement::setName(string n) {
    this->name = n;
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






















