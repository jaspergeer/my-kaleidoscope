#include <string>
#include <map>
#include <iostream>

#include "ast.h"
#include "error.h"
#include "parser.h"

// Lexer

static std::string IdentifierStr;
static double NumVal;

static int gettok() {
    static int lastChar = ' ';

    while (isspace(lastChar))
        lastChar = getchar();

    // identifiers and keywords
    if (isalpha(lastChar)) {
        IdentifierStr = lastChar;
        while (isalnum((lastChar = getchar())))
            IdentifierStr += lastChar;

        if (IdentifierStr == "def")
            return tok_def;
        if (IdentifierStr == "extern")
            return tok_extern;
        return tok_identifier;
    }

    // numbers
    if (isdigit(lastChar) || lastChar == '.') {
        std::string numStr;
        do {
            numStr += lastChar;
            lastChar = getchar();
        } while (isdigit(lastChar) || lastChar == '.');

        NumVal = strtod(numStr.c_str(), 0);
        return tok_number;
    }

    // comments
    if (lastChar == '#') {
        do
            lastChar = getchar();
        while (lastChar != EOF && lastChar != '\n' && lastChar != '\r');

        if (lastChar != EOF)
            return gettok();
    }

    if (lastChar == EOF)
        return tok_eof;

    int thisChar = lastChar;
    lastChar = getchar();
    return thisChar;
}

// Parser

int CurTok;

int getNextToken() {
    return CurTok = gettok();
}

static int getTokPrecedence() {
    if (!isascii(CurTok))
        return -1;
    
    switch(CurTok) {
        case '<':
            return 10;
        case '+':
            return 20;
        case '-':
            return 20;
        case '*':
            return 40;
    }
    return -1;
}

std::unique_ptr<ExprAST> logErrorE(const char *str) {
    logErrorE(str);
    return nullptr;
}

std::unique_ptr<PrototypeAST> logErrorP(const char *str) {
    logErrorE(str);
    return nullptr;
}

/// numberexpr ::= number
std::unique_ptr<ExprAST> parseNumberExpr() {
    auto result = std::make_unique<NumberExprAST>(NumVal);
    getNextToken();
    return std::move(result);
}

std::unique_ptr<ExprAST> parseExpression();

/// parenexpr ::= '(' expression ')'
std::unique_ptr<ExprAST> parseParenExpr() {
    getNextToken();
    auto v = parseExpression();
    if (!v)
        return nullptr;
    if (CurTok != ')')
        return logErrorE("expected ')'");
    getNextToken();
    return v;
}

/// identifierexpr
///   ::= identifier
///   ::= identifier '(' expression* ')'
std::unique_ptr<ExprAST> parseIdentifierExpr() {
    std::string idName = IdentifierStr;

    getNextToken();

    if (CurTok != '(')
        return std::make_unique<VariableExprAST>(idName);
    
    getNextToken();
    std::vector<std::unique_ptr<ExprAST>> args;
    if (CurTok != ')') {
        while(1) {
            if (auto arg = parseExpression())
                args.push_back(std::move(arg));
            else
                return nullptr;
            
            if (CurTok == ')')
                break;
            if (CurTok != ',')
                return logErrorE("Expected ')' or ',' in argument list");
            
            getNextToken();
        }
    }

    getNextToken();
    
    return std::make_unique<CallExprAST>(idName, std::move(args));
}

/// primary
///   ::= identifierexpr
///   ::= numberexpr
///   ::= parenexpr
std::unique_ptr<ExprAST> parsePrimary() {
    switch(CurTok) {
        default:
            return logErrorE("unknown token when expecting an expression");
        case tok_identifier:
            return parseIdentifierExpr();
        case tok_number:
            return parseNumberExpr();
        case '(':
            return parseParenExpr();
    }
}

/// binoprhs
///   ::= ('+' primary)*
static std::unique_ptr<ExprAST> parseBinOpRHS(int exprPrec, std::unique_ptr<ExprAST> lhs) {
    while (1) {
        int tokPrec = getTokPrecedence();
        if (tokPrec < exprPrec)
            return lhs;

        int binOp = CurTok;
        getNextToken();

        auto rhs = parsePrimary();
        if (!rhs)
            return nullptr;

        int nextPrec = getTokPrecedence();
        if (tokPrec < nextPrec) {
            rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_unique<BinaryExprAST>(binOp, std::move(lhs), std::move(rhs));
    }
}

/// expression
///   ::= primary binoprhs
std::unique_ptr<ExprAST> parseExpression() {
    auto lhs = parsePrimary();
    if (!lhs)
        return nullptr;
    
    return parseBinOpRHS(0, std::move(lhs));
}


/// prototype
///   ::= id '(' id* ')'
std::unique_ptr<PrototypeAST> parsePrototype() {
    if (CurTok != tok_identifier)
        return logErrorP("Expected function name in prototype");

    std::string fnName = IdentifierStr;
    getNextToken();

    if (CurTok != '(')
        return logErrorP("Expected '(' in prototype");

    std::vector<std::string> argNames;
    while (getNextToken() == tok_identifier) {
        argNames.push_back(IdentifierStr);

        // TODO
    }
    if (CurTok != ')')
        return logErrorP("Expected ')' in prototype");

    getNextToken();

    return std::make_unique<PrototypeAST>(fnName, std::move(argNames));
}

/// definition ::= 'def' prototype expression
std::unique_ptr<FunctionAST> parseDefinition() {
    getNextToken();
    auto proto = parsePrototype();
    if (!proto)
        return nullptr;

    if (auto e = parseExpression())
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    return nullptr;
}

/// external ::= 'extern' prototype
std::unique_ptr<PrototypeAST> parseExtern() {
    getNextToken();
    return parsePrototype();
}

/// toplevelexpr ::= expression
std::unique_ptr<FunctionAST> parseTopLevelExpr() {
    if (auto e = parseExpression()) {
        auto proto = std::make_unique<PrototypeAST>("__anon_expr", std::vector<std::string>());
        return std::make_unique<FunctionAST>(std::move(proto), std::move(e));
    }
    return nullptr;
}