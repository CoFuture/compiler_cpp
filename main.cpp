#include <iostream>
#include "SyntacticParser.h"
#include "GenerateCode.h"


int main() {
    std::cout << "Hello, World!" << std::endl;
    string filePath = R"(F:\CodeFiles\CLion Projects\compiler-cpp\sourcefile\test.txt)";
    const char *fileName = filePath.c_str();
    Scanner scanner(fileName);
    Lexer lexer(scanner);
    SymbolTable symbolTable;
    GenerateCode codeGenerator(symbolTable);
    SyntacticParser syntacticParser(lexer, symbolTable, codeGenerator);
    /***词法分析调试***/
//    Token *t = lexer.getToken();
//    while (t->tag != FILE_END)
//    {
//        cout << t->standardOutput() << endl;
//        t = lexer.getToken();
//    }
//    cout << t->standardOutput() << endl;
    /***语法分析调试***/
    syntacticParser.startSyntacticAnalyze();
    symbolTable.showInformation();
    symbolTable.showInterCode();

    symbolTable.optimize();
    symbolTable.showOptimizedCode();
    symbolTable.showRegisterId();
    symbolTable.genX86Code();
    return 0;
}

