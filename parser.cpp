#include <iostream>
#include <vector>
#include <string>
#include "Lexer.h"
#include <unordered_map>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>

enum NodeType
{
    NODE_PROGRAM,
    NODE_STATEMENT,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_IF_STATEMENT,
    NODE_FOR_LOOP,
    NODE_FUNCTION_DEFINITION,
    NODE_BLOCK,
    NODE_EXPRESSION,
    NODE_LITERAL,
    NODE_IDENTIFIER,
    NODE_PRINT_STATEMENT,
    NODE_UNARY_OP,
    NODE_BINARY_OP,
    NODE_COMMENT,
    NODE_JUMP_STATEMENT,
    NODE_FUNCTION,
    NODE_RETURN_STATEMENT,
    NODE_FUNCTION_NAME,
    NODE_ARRAY_DECLARATION
};

struct Node
{
    NodeType type;
    std::string value;
    std::vector<Node> children;
};
class ASTPrinter
{
public:
    void print(const Node &node, int indent = 0) const
    {
        printIndent(indent);
        std::cout << nodeTypeToString(node.type) << ": " << node.value << "\n";
        for (const Node &child : node.children)
        {
            print(child, indent + 2);
        }
    }

private:
    void printIndent(int indent) const
    {
        for (int i = 0; i < indent; ++i)
        {
            std::cout << ' ';
        }
    }

    std::string nodeTypeToString(NodeType type) const
    {
        switch (type)
        {
        case NODE_PROGRAM:
            return "Program";
        case NODE_DECLARATION:
            return "Declaration";
        case NODE_STATEMENT:
            return "Statement";
        case NODE_EXPRESSION:
            return "Expression";
        case NODE_IDENTIFIER:
            return "Identifier";
        case NODE_LITERAL:
            return "Literal";
        case NODE_PRINT_STATEMENT:
            return "PrintStatement";
        case NODE_IF_STATEMENT:
            return "IfStatement";
        case NODE_FOR_LOOP:
            return "ForLoop";
        case NODE_BLOCK:
            return "Block";
        case NODE_ASSIGNMENT:
            return "Assignment";
        case NODE_BINARY_OP:
            return "BinaryOp";
        case NODE_UNARY_OP:
            return "UnaryOp";
        case NODE_COMMENT:
            return "Comment";
        case NODE_JUMP_STATEMENT:
            return "Jump";
        case NODE_FUNCTION:
            return "Function";
        case NODE_RETURN_STATEMENT:
            return "ReturnStatement";
        case NODE_FUNCTION_DEFINITION:
            return "FUNCTION DEFINITION";
        case NODE_FUNCTION_NAME:
            return "FUNCTION NAME";
        case NODE_ARRAY_DECLARATION:
            return "Array Declaration";
        // case NODE_BINARY_OP:
        //     return "BinaryOp";
        // case NODE_UNARY_OP:
        //     return "UnaryOp";
        default:
            return "Unknown";
        }
    }
};
class Parser
{
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}

    Node parse()
    {
        Node program = {NODE_PROGRAM};
        while (tokens[pos].type != T_EOF)
        {
            program.children.push_back(parseStatement());
        }
        return program;
    }

private:
    std::vector<Token> tokens;
    size_t pos;

    Node parseStatement()
    {
        if (match(T_Int) || match(T_Char) || match(T_Bool))
        {
            return parseDeclaration();
        }
        else if (match(T_Id))
        {
            return parseAssignment();
        }
        else if (match(T_If))
        {
            return parseIfStatement();
        }
        else if (match(T_For))
        {
            return parseForLoop();
        }
        else if (match(T_Print))
        {
            return parsePrintStatement();
        }
        else if (match(T_LC))
        {
            return parseBlock();
        }
        else if (match(T_Return))
        {
            return parseReturnStatement();
        }
        else if (match(T_Comment))
        {
            return parseComment();
        }
        else if (match(T_Break) || match(T_Continue))
        {
            return parseJumpStatement();
        }

        error("Unexpected token in statement");
        return {};
    }

    Node parseDeclaration()
    {
        bool isFuncDef = false;
        Node declaration = {NODE_DECLARATION, tokens[pos++].value};
        declaration.children.push_back(parseIdentifier());
        if (match(T_LB))
        {
            declaration.type = NODE_ARRAY_DECLARATION;
            ++pos; // consume [
            if (!match(T_Decimal))
            {
                error("Expected array size");
            }
            declaration.children.push_back({NODE_LITERAL, tokens[pos++].value});
            if (!match(T_RB))
            {
                error("Expected ']' after array size");
            }
            ++pos;
            // declaration.children.push_back(parseIdentifier());
            if (match(T_Assign))
            {
                ++pos;
                declaration.children.push_back(parseExpression());
            }
        }
        if (match(T_LP))
        {
            ++pos;
            declaration.type = NODE_FUNCTION_DEFINITION;
            declaration.children[0].type = NODE_FUNCTION_NAME;
            while (!match(T_RP))
            {

                if (match(T_Bool) || match(T_Char) || match(T_Int))
                {
                    Node dec = {NODE_DECLARATION, tokens[pos++].value};
                    dec.children.push_back(parseIdentifier());
                    declaration.children.push_back(dec);
                }
                else
                    ++pos; // consume current
            }
            if (!match(T_RP))
            {
                error("expected )");
            }
            else
            {
                ++pos; // consume )
            }

            if (match(T_LC))
            {
                isFuncDef = true;
                declaration.children.push_back(parseBlock());
            }
        }

        if (match(T_Assign))
        {
            ++pos;
            declaration.children.push_back(parseExpression());
        }
        while (match(T_Comma))
        {
            ++pos;
            declaration.children.push_back(parseIdentifier());
            if (match(T_LB))
            {
                ++pos; // consume [
                if (!match(T_Decimal))
                {
                    error("Expected array size");
                }
                declaration.children.push_back({NODE_LITERAL, tokens[pos++].value});
                if (!match(T_RB))
                {
                    error("Expected ']' after array size");
                }
                ++pos;
                // declaration.children.push_back(parseIdentifier());
                if (match(T_Assign))
                {
                    ++pos;
                    declaration.children.push_back(parseExpression());
                }
            }
            if (match(T_Assign))
            {
                ++pos;
                declaration.children.push_back(parseExpression());
            }
        }
        if (!match(T_Semicolon) && !isFuncDef)
        {
            error("Expected semicolon");
        }
        if (!isFuncDef)
        {
            ++pos; // consume semicolon
        }

        return declaration;
    }

    Node parseAssignment()
    {
        Node assignment = {NODE_ASSIGNMENT};
        assignment.children.push_back(parseIdentifier());
        if (match(T_LP))
        {
            assignment.type = NODE_FUNCTION;
            ++pos; // comsume (
            while (!match(T_RP))
            {
                Node dec = parseLogicalOrExpression();
                if (match(T_Comma))
                {
                    ++pos; // consume comma
                    if (match(T_RP))
                    {
                        error("extra comma");
                    }
                }
                assignment.children.push_back(dec);
            }
            ++pos; // consume )
        }

        if (match(T_LB))
        {
            ++pos; // consume [
            if (match(T_Decimal))
            {
                assignment.children.push_back({NODE_LITERAL, tokens[pos++].value});
            }
            else if (match(T_Id))
            {

                assignment.children.push_back({NODE_IDENTIFIER, tokens[pos++].value});
            }
            else
            {
                error("Expected array size");
            }

            if (!match(T_RB))
            {
                error("Expected ']' after array size");
            }
            ++pos;
        }
        if (!match(T_Assign) && assignment.type == NODE_ASSIGNMENT)
        {
            error("Expected '=' in assignment");
        }
        if (assignment.type == NODE_ASSIGNMENT)
        {
            ++pos; // consume '='
            assignment.children.push_back(parseExpression());
        }

        if (!match(T_Semicolon))
        {
            error("Expected semicolon");
        }
        ++pos; // consume semicolon
        return assignment;
    }

    Node parseIfStatement()
    {
        Node ifStatement = {NODE_IF_STATEMENT, tokens[pos++].value};
        if (!match(T_LP))
        {
            error("Expected '(' after 'if'");
        }
        ++pos; // consume '('
        ifStatement.children.push_back(parseExpression());
        if (!match(T_RP))
        {
            error("Expected ')' after condition");
        }
        ++pos; // consume ')'
        ifStatement.children.push_back(parseStatement());
        if (match(T_Else))
        {
            ++pos; // consume 'else'
            if (match(T_If))
            {
                ifStatement.children.push_back(parseIfStatement());
            }
            else
            {
                ifStatement.children.push_back(parseStatement());
            }
        }
        return ifStatement;
    }

    Node parseForLoop()
    {
        Node forLoop = {NODE_FOR_LOOP, tokens[pos++].value};
        if (!match(T_LP))
        {
            error("Expected '(' after 'for'");
        }
        ++pos; // consume '('
        if (match(T_Int) || match(T_Bool) || match(T_Char))
        {
            forLoop.children.push_back(parseDeclaration());
        }
        else if (match(T_Id))
        {
            forLoop.children.push_back(parseAssignment());
        }
        else
        {
            ++pos; // consume ';'
        }
        if (!match(T_Semicolon))
        {
            forLoop.children.push_back(parseExpression());
        }
        if (!match(T_Semicolon))
        {
            error("Expected ';' after first expression");
        }
        ++pos; // consume ';'
        if (!match(T_RP))
        {
            forLoop.children.push_back(parseExpression());
        }
        if (!match(T_RP))
        {
            error("Expected ')' after for conditions");
        }
        ++pos; // consume ')'
        forLoop.children.push_back(parseStatement());
        return forLoop;
    }

    Node parseBlock()
    {
        Node block = {NODE_BLOCK, ""};
        ++pos; // consume '{'
        while (!match(T_RC) && pos < tokens.size())
        {
            block.children.push_back(parseStatement());
        }
        if (!match(T_RC))
        {
            error("Expected '}'");
        }
        ++pos; // consume '}'
        return block;
    }

    Node parseReturnStatement()
    {
        Node ret = {NODE_RETURN_STATEMENT, "return"};
        ++pos; // consume 'return'
        ret.children.push_back(parseExpression());
        if (!match(T_Semicolon))
        {
            error("Expected semicolon");
        }
        ++pos; // consume semicolon
        return ret;
    }

    Node parsePrintStatement()
    {
        Node printStatement = {NODE_PRINT_STATEMENT, tokens[pos++].value};
        if (!match(T_LP))
        {
            error("Expected '(' after 'print'");
        }
        ++pos; // consume '('
        printStatement.children.push_back(parseExpression());
        while (match(T_Comma))
        {
            ++pos;
            printStatement.children.push_back(parseExpression());
        }
        if (!match(T_RP))
        {
            error("Expected ')' after print arguments");
        }
        ++pos; // consume ')'
        if (!match(T_Semicolon))
        {
            error("Expected semicolon");
        }
        ++pos; // consume semicolon
        return printStatement;
    }

    Node parseExpression()
    {
        return parseAssignmentExpression();
    }

    Node parseAssignmentExpression()
    {
        Node left = parseLogicalOrExpression();
        if (match(T_Assign))
        {
            Node assignment = {NODE_ASSIGNMENT, tokens[pos++].value};
            assignment.children.push_back(left);
            assignment.children.push_back(parseAssignmentExpression());
            return assignment;
        }
        return left;
    }

    Node parseLogicalOrExpression()
    {
        Node left = parseLogicalAndExpression();
        while (match(T_LOp_OR))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseLogicalAndExpression());
            left = binary;
        }
        return left;
    }

    Node parseLogicalAndExpression()
    {
        Node left = parseEqualityExpression();
        while (match(T_LOp_AND))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseEqualityExpression());
            left = binary;
        }
        return left;
    }

    Node parseEqualityExpression()
    {
        Node left = parseRelationalExpression();
        while (match(T_ROp_E) || match(T_ROp_NE))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseRelationalExpression());
            left = binary;
        }
        return left;
    }

    Node parseRelationalExpression()
    {
        Node left = parseAdditiveExpression();
        while (match(T_ROp_L) || match(T_ROp_G) || match(T_ROp_LE) || match(T_ROp_GE))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseAdditiveExpression());
            left = binary;
        }
        return left;
    }

    Node parseAdditiveExpression()
    {
        Node left = parseMultiplicativeExpression();
        while (match(T_AOp_PL) || match(T_AOp_MN))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseMultiplicativeExpression());
            left = binary;
        }
        return left;
    }

    Node parseMultiplicativeExpression()
    {
        Node left = parseUnaryExpression();
        while (match(T_AOp_ML) || match(T_AOp_DV) || match(T_AOp_RM))
        {
            Node binary = {NODE_BINARY_OP, tokens[pos++].value};
            binary.children.push_back(left);
            binary.children.push_back(parseUnaryExpression());
            left = binary;
        }
        return left;
    }

    Node parseUnaryExpression()
    {
        if (match(T_LOp_NOT))
        {
            Node unary = {NODE_UNARY_OP, tokens[pos++].value};
            unary.children.push_back(parseUnaryExpression());
            return unary;
        }
        else if (match(T_AOp_PL) || match(T_AOp_MN))
        {
            Node unary = {NODE_UNARY_OP, tokens[pos++].value};
            unary.children.push_back(parseUnaryExpression());
            return unary;
        }
        return parsePrimaryExpression();
    }

    Node parsePrimaryExpression()
    {
        if (match(T_Id))
        {
            Node temp = {NODE_IDENTIFIER, tokens[pos++].value};
            if (match(T_LP))
            {
                temp.type = NODE_FUNCTION;
                ++pos; // consume (
                while (!match(T_RP))
                {
                    Node dec = parseLogicalOrExpression();
                    if (match(T_Comma))
                    {
                        ++pos; // consume comma
                        if (match(T_RP))
                        {
                            error("extra comma");
                        }
                    }
                    temp.children.push_back(dec);
                }
                ++pos; // consume )
            }
            if (match(T_LB))
            {
                ++pos; // consume [
                if (match(T_Decimal))
                {
                    temp.children.push_back({NODE_LITERAL, tokens[pos++].value});
                }
                else if (match(T_Id))
                {

                    temp.children.push_back({NODE_IDENTIFIER, tokens[pos++].value});
                }
                else
                {
                    error("Expected array size");
                }

                if (!match(T_RB))
                {
                    error("Expected ']' after array size");
                }
                ++pos;
            }
            return temp;
        }
        else if (match(T_Decimal) || match(T_Hexadecimal))
        {
            return {NODE_LITERAL, tokens[pos++].value};
        }
        else if (match(T_True) || match(T_False))
        {
            return {NODE_LITERAL, tokens[pos++].value};
        }
        else if (match(T_Character))
        {
            return {NODE_LITERAL, tokens[pos++].value};
        }
        else if (match(T_String))
        {
            return {NODE_LITERAL, tokens[pos++].value};
        }
        else if (match(T_LP))
        {
            ++pos; // consume '('
            Node expr = parseExpression();
            if (!match(T_RP))
            {
                error("Expected ')'");
            }
            ++pos; // consume ')'
            return expr;
        }
        error("Expected primary expression");
        return {};
    }

    Node parseIdentifier()
    {
        if (!match(T_Id))
        {
            error("Expected identifier");
        }
        return {NODE_IDENTIFIER, tokens[pos++].value};
    }

    Node parseComment()
    {
        if (!match(T_Comment))
        {
            error("Expected Comment");
        }
        return {NODE_COMMENT, tokens[pos++].value};
    }

    Node parseJumpStatement()
    {
        Node jump = {NODE_JUMP_STATEMENT, tokens[pos++].value};
        if (!match(T_Semicolon))
        {
            error("expected Semicolon");
        }
        ++pos; // consume ;
        return jump;
    }

    bool match(TokenType type)
    {
        return pos < tokens.size() && tokens[pos].type == type;
    }
    bool matchNext(TokenType type)
    {
        return pos + 1 < tokens.size() && tokens[pos + 1].type == type;
    }
    void error(const std::string &message)
    {
        std::cerr << "Parse error: " << message << " at token " << tokens[pos].value << "\n";
        exit(1);
    }
};

struct Symbol
{
    std::string name;
    std::string type;
    int scopeLevel;
    
};

class SymbolTable
{
private:
    std::vector<std::unordered_map<std::string, Symbol>> table;
    int currentScope;

public:
    SymbolTable() : currentScope(0)
    {
        table.emplace_back();
    }

    void enterScope()
    {
        currentScope++;
        table.emplace_back();
    }

    void exitScope()
    {
        if (currentScope > 0)
        {
            table.pop_back();
            currentScope--;
        }
    }

    bool insert(const std::string &name, const std::string &type)
    {
        if (table[currentScope].find(name) != table[currentScope].end())
        {
            return false;
        }
        table[currentScope][name] = {name, type, currentScope};
        return true;
    }

    Symbol *lookup(const std::string &name)
    {
        for (int i = currentScope; i >= 0; i--)
        {
            if (table[i].find(name) != table[i].end())
            {
                return &table[i][name];
            }
        }
        return nullptr;
    }
};

class SemanticAnalyzer
{
private:
    SymbolTable symbolTable;
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>> functionTable;
    std::vector<std::string> errors;

    void checkNode(const Node &node)
    {
        switch (node.type)
        {

        case NODE_PROGRAM:
            for (const auto &child : node.children)
            {
                checkNode(child);
            }
            break;
        case NODE_DECLARATION:
            checkDeclaration(node);
            break;
        case NODE_FUNCTION_DEFINITION:
            checkFunctionDeclaration(node);
            break;
        case NODE_FUNCTION:
            // checkFunctionDeclaration(node);
            break;
        case NODE_ARRAY_DECLARATION:
            checkArrayDeclaration(node);
            break;
        case NODE_ASSIGNMENT:
            checkAssignment(node);
            break;
        case NODE_IF_STATEMENT:
            checkIfStatement(node);
            break;
        case NODE_FOR_LOOP:
            checkForLoop(node);
            break;
        case NODE_BLOCK:
            symbolTable.enterScope();
            for (const auto &child : node.children)
            {
                checkNode(child);
            }
            symbolTable.exitScope();
            break;
        case NODE_IDENTIFIER:
            checkIdentifier(node);
            break;

        default:
            for (const auto &child : node.children)
            {
                checkNode(child);
            }
            break;
        }
    }
    void checkMainFunction(const Node &root)
    {
        bool mainFunctionFound = false;
        for (const Node &child : root.children)
        {

            if (child.type == NODE_FUNCTION_DEFINITION)
            {
                for (const Node &childmain : child.children)
                {
                    if (childmain.type == NODE_FUNCTION_NAME && childmain.value == "main")
                    {
                        mainFunctionFound = true;
                    }
                }
                // break;
            }
        }

        if (!mainFunctionFound)
        {
            std::cerr << "Semantic error: 'main' function is missing." << std::endl;
            exit(1);
        }
    }
    void checkDeclaration(const Node &node)
    {
        if (!symbolTable.insert(node.children[0].value, node.value))
        {
            reportError("Variable " + node.children[0].value + " redeclared in the same scope");
        }
    }
    void checkFunctionDeclaration(const Node &node)
    {
        std::string funcName = node.children[0].value;
        std::string returnType = node.value;
        std::vector<std::string> paramTypes;
        for (const Node &param : node.children)
        {
            if (param.type == NODE_DECLARATION)
            {
                paramTypes.push_back(param.value);
                symbolTable.enterScope();
                symbolTable.insert(param.children[0].value, paramTypes.back());
                symbolTable.exitScope();
            }
        }
        if (functionTable.find(funcName) != functionTable.end())
        {
            errors.push_back("function " + funcName + " is already declared");
        }
        else
        {
            functionTable[funcName] = {returnType, paramTypes};
            checkNode(node.children.back());
        }
    }
    // void checkFunctionCall(const Node &node)
    // {
    //     std::string funcName = node.children[0].value;
    //     std::string returnType = node.value;
    //     std::vector<std::string> paramTypes;
    //     for (const Node &param : node.children)
    //     {
    //         if (param.type == NODE_DECLARATION)
    //         {
    //             paramTypes.push_back(param.value);
    //             symbolTable.enterScope();
    //             symbolTable.insert(param.children[0].value, paramTypes.back());
    //             symbolTable.exitScope();
    //         }
    //     }
    //     functionTable[funcName] = {returnType, paramTypes};
    //     checkNode(node.children.back());
    // }
    void checkArrayDeclaration(const Node &node)
    {
        int size;
        try
        {
            size = stoi(node.children[1].value);
        }
        catch (...)
        {
            reportError("Index of  " + node.children[0].value + " type mismatch");
        }
        if (size < 1)
        {
            reportError("Size of  " + node.children[0].value + " is invalid");
        }
        else
        {
            for (size_t i = 0; i < size; i++)
            {
                if (!symbolTable.insert("@" + node.children[0].value + to_string(i), node.value))
                {
                    reportError("Variable " + node.children[0].value + " redeclared in the same scope");
                }
            }
        }
    }
    void checkAssignment(const Node &node)
    {
        if (node.children.size() < 3)
        {
            if (symbolTable.lookup(node.children[0].value) == nullptr)
            {
                reportError("Variable " + node.children[0].value + " used before declaration");
            }
        }
        else
        {
            int index;
            try
            {
                stoi(node.children[1].value);
            }
            catch (...)
            {
                reportError("invalid index type for array " + node.children[0].value);
            }
            if (symbolTable.lookup("@" + node.children[0].value + node.children[1].value) == nullptr)
            {
                reportError(node.children[0].value + "[" + node.children[1].value + "]");
            }
        }
    }

    void checkIfStatement(const Node &node)
    {
        checkNode(node.children[0]); // Condition
        checkNode(node.children[1]); // If block
        if (node.children.size() > 2)
        {
            checkNode(node.children[2]); // Else block
        }
    }

    void checkForLoop(const Node &node)
    {
        symbolTable.enterScope();
        checkNode(node.children[0]); // Initialization
        checkNode(node.children[1]); // Condition
        checkNode(node.children[2]); // Increment
        checkNode(node.children[3]); // Loop body
        symbolTable.exitScope();
    }

    void checkIdentifier(const Node &node)
    {

        if (symbolTable.lookup(node.value) == nullptr && symbolTable.lookup("@" + node.value + "0") == nullptr)
        {
            reportError("Variable " + node.value + " used before declaration");
        }
    }

    void reportError(const std::string &error)
    {
        errors.push_back(error);
    }

public:
    void analyze(const Node &root)
    {
        checkMainFunction(root);
        checkNode(root);
        if (!errors.empty())
        {
            for (const auto &error : errors)
            {
                std::cerr << error << "\n";
            }
        }
    }
};

class TypeChecker
{
public:
    void check(const Node &node)
    {
        switch (node.type)
        {
        case NODE_PROGRAM:
            for (const auto &child : node.children)
            {
                check(child);
            }
            break;
        case NODE_DECLARATION:
            checkDeclaration(node);
            break;
        case NODE_ARRAY_DECLARATION:
            checkArrayDeclaration(node);
            break;
        case NODE_ASSIGNMENT:
            checkAssignment(node);
            break;
        case NODE_FUNCTION_DEFINITION:
            checkFunctionDefinition(node);
            break;
        case NODE_FUNCTION:
            checkFunctionCall(node);
            break;
        case NODE_PRINT_STATEMENT:
            checkPrintStatement(node);
            break;
        case NODE_IF_STATEMENT:
            checkIfStatement(node);
            break;
        case NODE_FOR_LOOP:
            checkForLoop(node);
            break;
        case NODE_BLOCK:
            checkBlock(node);
            break;
        case NODE_RETURN_STATEMENT:
            checkReturnStatement(node);
            break;
        default:
            for (const Node &child : node.children)
            {
                check(child);
            }
            break;
        }
    }

private:
    std::unordered_map<std::string, std::string> symbolTable;
    std::unordered_map<std::string, std::pair<std::string, std::vector<std::string>>> functionTable;

    void checkDeclaration(const Node &node)
    {
        std::string varName = node.children[0].value;
        std::string varType = node.value;
        symbolTable[varName] = varType;
        if (node.children.size() > 1)
        {
            std::string exprType = checkExpression(node.children[1]);
            if (varType != exprType)
            {
                error("Type mismatch in declaration for variable " + varName);
            }
        }
    }

    void checkArrayDeclaration(const Node &node)
    {
        std::string varName = node.children[0].value;
        std::string varType = node.value;

        int size = -1;
        try
        {
            size = stoi(node.children[1].value);
        }
        catch (...)
        {
            error("array " + varName + " size cant be this value");
        }

        if (size <= 0)
        {
            error("array " + varName + " size cant be non positive");
        }
        for (size_t i = 0; i < size; i++)
        {
            symbolTable["@" + varName + to_string(i)] = varType;
        }
    }

    void checkAssignment(const Node &node)
    {
        std::string varName = node.children[0].value;
        if (symbolTable.find(varName) == symbolTable.end())
        {
            error("Undeclared variable " + varName);
        }
        std::string varType = symbolTable[varName];
        std::string exprType = checkExpression(node.children[1]);
        if (varType != exprType)
        {
            error("Type mismatch in assignment for variable " + varName);
        }
    }

    void checkFunctionDefinition(const Node &node)
    {
        std::string funcName = node.children[0].value;
        std::string returnType = node.value;
        std::vector<std::string> paramTypes;
        for (const Node &param : node.children)
        {
            if (param.type == NODE_DECLARATION)
            {
                paramTypes.push_back(param.value);

                symbolTable[param.children[0].value] = paramTypes.back();
            }
        }
        if (functionTable.find(funcName) != functionTable.end())
        {
            error("function " + funcName + " is already declared");
        }
        else
        {
            check(node.children.back());
            std::string exprType = checkExpression((node.children.back().children.back()));
            functionTable[funcName] = {returnType, paramTypes};
            if (exprType != returnType)
            {
                error("function " + funcName + " return type mismatch");
            }
           
        }
    }

    void checkFunctionCall(const Node &node)
    {
        std::string funcName = node.value;
        if (functionTable.find(funcName) == functionTable.end())
        {
            error("Undeclared function " + funcName);
        }
        // auto [returnType, paramTypes] = functionTable[funcName];
        auto pair = functionTable[funcName];
        std::string returnType = pair.first;
        std::vector<std::string> paramTypes = pair.second;

        if (node.children.size() != paramTypes.size())
        {
            error("Incorrect number of arguments in call to function " + funcName);
        }
        for (size_t i = 0; i < node.children.size(); ++i)
        {
            std::string argType = checkExpression(node.children[i]);
            if (argType != paramTypes[i])
            {
                error("Type mismatch in argument " + std::to_string(i) + " of call to function " + funcName);
            }
        }
    }

    void checkPrintStatement(const Node &node)
    {
        for (const Node &child : node.children)
        {
            checkExpression(child);
        }
    }

    void checkIfStatement(const Node &node)
    {
        std::string conditionType = checkExpression(node.children[0]);
        if (conditionType != "bool" && node.children[0].type != NODE_BINARY_OP)
        {
            error("Condition in if statement must be a boolean expression");
        }
        check(node.children[1]);
        if (node.children.size() > 2)
        {
            check(node.children[2]);
        }
    }

    void checkForLoop(const Node &node)
    {
        check(node.children[0]);
        std::string conditionType = checkExpression(node.children[1]);
        if (conditionType != "bool")
        {
            error("Condition in for loop must be a boolean expression");
        }
        check(node.children[2]);
    }

    void checkBlock(const Node &node)
    {
        for (const Node &child : node.children)
        {
            check(child);
        }
    }

    void checkReturnStatement(const Node &node)
    {
        checkExpression(node);
    }

    std::string checkExpression(const Node &node)
    {
        switch (node.type)
        {
        case NODE_LITERAL:
            return getLiteralType(node.value);
        case NODE_IDENTIFIER:
            if (symbolTable.find(node.value) == symbolTable.end())
            {
                error("Undeclared variable " + node.value);
            }
            return symbolTable[node.value];
        case NODE_BINARY_OP:
            return checkBinaryOp(node);
        case NODE_UNARY_OP:
            return checkUnaryOp(node);
        case NODE_FUNCTION:
            checkFunctionCall(node);
            return functionTable[node.value].first;
        default:
            for (const Node &child : node.children)
            {
                return checkExpression(child);
            }
            break;
        }
        return "";
    }

    std::string checkBinaryOp(const Node &node)
    {
        std::string leftType = checkExpression(node.children[0]);
        std::string rightType = checkExpression(node.children[1]);
        if (leftType != rightType)
        {
            error("Type mismatch in binary operation");
        }
        return leftType;
    }

    std::string checkUnaryOp(const Node &node)
    {
        return checkExpression(node.children[0]);
    }

    std::string getLiteralType(const std::string &value)
    {
        if (isdigit(value[0]) || value[0] == '-')
        {
            if (find(value.begin(), value.end(), '.') != value.end() || find(value.begin(), value.end(), 'e') != value.end())
            {
                return "float";
            }
            else
            {
                return "int";
            }
        }
        else if (value[0] == '\'' && value.back() == '\'')
        {
            return "char";
        }
        else if (value == "true" || value == "false")
        {
            return "bool";
        }
        else
        {
            return "string";
        }
    }

    std::string inferType(const Node &node)
    {
        switch (node.type)
        {
        case NODE_LITERAL:
            return getLiteralType(node.value);
        case NODE_IDENTIFIER:
            if (symbolTable.find(node.value) != symbolTable.end())
            {
                return symbolTable[node.value];
            }
            error("Undeclared variable " + node.value);
            break;
        case NODE_BINARY_OP:
            return checkBinaryOp(node);
        case NODE_UNARY_OP:
            return checkUnaryOp(node);
        default:
            break;
        }
        return "";
    }

    void error(const std::string &message)
    {
        std::cerr << "Type error: " << message << "\n";
        // exit(1);
    }
};

int main()
{
    string filePath;
    cout << "Enter the path to the .pl file: ";
    getline(cin, filePath);
    vector<Token> Tokens;
    ifstream file(filePath);
    if (file.is_open())
    {
        string line;
        string input;
        while (getline(file, line))
            input += line + '\n'; // Append newline character

        tokenize(input, Tokens);

        file.close();
    }
    else
    {
        cout << "Unable to open file" << endl;
    }

    Parser parser(Tokens);
    Node root = parser.parse();
    ASTPrinter printer;
    printer.print(root);
    SemanticAnalyzer semanticAnalyzer;
    semanticAnalyzer.analyze(root);
    TypeChecker typeChecker;
    typeChecker.check(root);

    return 0;
}
