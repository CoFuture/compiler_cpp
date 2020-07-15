cmake_minimum_required(VERSION 3.15)
project(compiler_cpp)

set(CMAKE_CXX_STANDARD 11)

add_executable(compiler_cpp main.cpp Token.cpp Token.h Lexer.cpp Lexer.h SyntacticParser.cpp SyntacticParser.h SymbolTable.cpp SymbolTable.h IntermediateCode.cpp IntermediateCode.h GenerateCode.cpp GenerateCode.h VarElement.cpp VarElement.h FunElement.cpp FunElement.h DataFlowGraph.cpp DataFlowGraph.h ConstPropagation.cpp ConstPropagation.h LiveAnalyse.cpp LiveAnalyse.h Set.cpp Set.h RegisterAssign.cpp RegisterAssign.h)