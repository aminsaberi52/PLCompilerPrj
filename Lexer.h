#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <cctype>
#include <cstdlib> // For strtol function
#include <sstream> // For stringstream
#include <iomanip> // For hex format
#include <vector>
#include <algorithm> //for find()


using namespace std;

enum TokenType
{
    T_Bool,        // bool
    T_Break,       // break
    T_Char,        // char
    T_Continue,    // continue
    T_Else,        // else
    T_False,       // false
    T_For,         // for
    T_If,          // if
    T_Int,         // int
    T_Print,       // print
    T_Return,      // return
    T_True,        // true
    T_AOp_PL,      // +
    T_AOp_MN,      // -
    T_AOp_ML,      // *
    T_AOp_DV,      // /
    T_AOp_RM,      // %
    T_ROp_L,       // <
    T_ROp_G,       // >
    T_ROp_LE,      // <=
    T_ROp_GE,      // >=
    T_ROp_NE,      // !=
    T_ROp_E,       // ==
    T_LOp_AND,     // &&
    T_LOp_OR,      // ||
    T_LOp_NOT,     // !
    T_Assign,      // =
    T_LP,          // (
    T_RP,          // )
    T_LC,          // {
    T_RC,          // }
    T_LB,          // [
    T_RB,          // ]
    T_Semicolon,   // ;
    T_Comma,       // ,
    T_Id,          // variable or function names
    T_Decimal,     // decimal integers
    T_Hexadecimal, // hexadecimal integers
    T_String,      // constant strings "[string]"
    T_Character,   // constant characters '[character]'
    T_Comment,     //[string]\n
    T_Whitespace,  // whitespace (newline, tab, and space characters)
    T_EOF          // end of file

};
struct Token
{
    TokenType type;
    std::string value;
    int lineNumber;
};
vector<string> TokenStrings = {
    "T_Bool",        // bool
    "T_Break",       // break
    "T_Char",        // char
    "T_Continue",    // continue
    "T_Else",        // else
    "T_False",       // false
    "T_For",         // for
    "T_If",          // if
    "T_Int",         // int
    "T_Print",       // print
    "T_Return",      // return
    "T_True",        // true
    "T_AOp_PL",      // +
    "T_AOp_MN",      // -
    "T_AOp_ML",      // *
    "T_AOp_DV",      // /
    "T_AOp_RM",      // %
    "T_ROp_L",       // <
    "T_ROp_G",       // >
    "T_ROp_LE",      // <=
    "T_ROp_GE",      // >=
    "T_ROp_NE",      // !=
    "T_ROp_E",       // ==
    "T_LOp_AND",     // &&
    "T_LOp_OR",      // ||
    "T_LOp_NOT",     // !
    "T_Assign",      // =
    "T_LP",          // (
    "T_RP",          // )
    "T_LC",          // {
    "T_RC",          // }
    "T_LB",          // [
    "T_RB",          // ]
    "T_Semicolon",   // ;
    "T_Comma",       // ,
    "T_Id",          // variable or function names
    "T_Decimal",     // decimal integers
    "T_Hexadecimal", // hexadecimal integers
    "T_String",      // constant strings "[string]"
    "T_Character",   // constant characters '[character]'
    "T_Comment",     //[string]\n
    "T_Whitespace",  // whitespace (newline, tab, and space characters)
    "T_EOF"};
// Set of keywords
unordered_set<string> keywords = {
    "bool", "break", "char", "continue", "else", "false", "for", "if", "int", "print", "return", "true"};

// Function to check if a character is a delimiter
bool isDelimiter(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n' || ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' || ch == '=' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '%' || ch == '!' || ch == '<' || ch == '>' || ch == '&' || ch == '|' || ch == '!';
}

bool isToken(const std::string &str)
{
    // چک کردن اولین کاراکتر برای بودن عدد
    if (isdigit(str[0]))
    {
        // چک کردن بقیه کاراکترها برای بودن حروف
        for (size_t i = 1; i < str.length(); ++i)
            if (isalpha(str[i]))
                return true;

        return false;
    }
    return false;
}

string capitalizeFirstLetter(string str)
{
    if (!str.empty())
        str[0] = toupper(str[0]);

    return str;
}

// Function to tokenize input string
void tokenize(const string &input, vector<Token> &Tokens)
{
    string currentToken;
    bool inChar = false;   // Flag to indicate if inside a char
    bool inString = false; // Flag to indicate if inside a string
    int lineNo = 1;        // Variable to hold line number, initialized to 1
    bool isHex = true;
    bool newLine = false; // Flag to indicate if string or char has new line character

    for (size_t i = 0; i < input.size(); ++i)
    {
        // state 0
        char ch = input[i]; // state 1

        if ((isDelimiter(ch) && !inChar) && (isDelimiter(ch) && !inString))
        {

            if (!currentToken.empty()) // state 2
            {
                // Output the token
                if (keywords.find(currentToken) != keywords.end())
                { // state 3
                    // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: " << "T_" + capitalizeFirstLetter(currentToken) << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),("T_" + capitalizeFirstLetter(currentToken))))));
                    temp.value = currentToken;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }
                else if (currentToken[0] == '0' && (currentToken[1] == 'x' || currentToken[1] == 'X')) // state 4
                {
                    for (size_t j = 2; j < currentToken.size(); ++j)
                    {
                        if (!isxdigit(currentToken[j])) // Check if character is a valid hexadecimal digit | state 5
                        {
                            isHex = false;
                            break;
                        }
                    }

                    if (isHex)
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: T_Hexadecimal" << endl; // state 6
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Hexadecimal"))));
                        temp.value = currentToken;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                    else // trap
                        cout << "This is not a token !" << endl;
                }

                else if (isdigit(currentToken[0]) || (currentToken[0] == '-' && currentToken.size() > 1 && isdigit(currentToken[1])) || (currentToken[0] == '+' && currentToken.size() > 1 && isdigit(currentToken[1]))) // state 7
                {
                    if (isToken(currentToken)) // state 8
                        cout << "This is not a token !" << endl;

                    else // trap
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: T_Decimal" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Decimal"))));
                        temp.value = currentToken;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }

                else if (!inChar && (currentToken[0] == '\'' && currentToken.back() == '\''))
                { // state 9

                    if (!inChar && newLine)
                    {
                        cout << "This is not a token !" << endl;
                        newLine = !newLine; // Toggle newLine flag
                    }

                    else
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: T_Character" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Character"))));
                        temp.value = currentToken;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                    inChar = false; // Reset the inString flag
                }

                else if (!inString && (currentToken[0] == '"' && currentToken.back() == '"'))
                { // state 10
                    if (!inString && newLine)
                    {
                        cout << "This is not a token !" << endl;
                        newLine = !newLine; // Toggle newLine flag
                    }

                    else
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: T_String" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_String"))));
                        temp.value = currentToken;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }

                    inString = false;
                }

                else // state 11
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << currentToken << ", Token: T_Id" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Id"))));
                    temp.value = currentToken;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }
                currentToken.clear();
            }
            if (ch != ' ' && ch != '\t' && ch != '\n') // state 16
            {
                if (ch == '/') // state 17
                {
                    // Check for comment
                    if (i + 1 < input.size() && input[i + 1] == '/') // state 18
                    {
                        // Search for newline character '\n' from the current position
                        size_t pos = input.find('\n', i);
                        if (pos != string::npos) // state 19
                        {
                            // Found newline character, treat the substring as a comment
                            // cout << "Line: " << lineNo << ", Lexeme: " << input.substr(i, pos - i) << ", Token: T_Comment" << endl;
                            Token temp;
                            temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Comment"))));
                            temp.value = input.substr(i, pos - i);
                            temp.lineNumber=lineNo;
                            Tokens.push_back(temp);
                            i = pos - 1; // Move the index to the end of the comment
                        }
                    }

                    else // state 20
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_AOp_DV" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_AOp_DV"))));
                        temp.value = "/";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }

                else if (ch == ';') // state 21
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Semicolon" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Semicolon"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == '=') // state 22
                {
                    if (i + 1 < input.size() && input[i + 1] == '=') // state 23
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: ==, Token: T_ROp_E" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_E"))));
                        temp.value = "==";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '=' character
                    }
                    else // state 24
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Assign" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Assign"))));
                        temp.value = "=";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }
                else if (ch == '<') // state 25
                {
                    if (i + 1 < input.size() && input[i + 1] == '=') // state 26
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: <=, Token: T_ROp_LE" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_LE"))));
                        temp.value = "<=";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '=' character
                    }
                    else // state 27
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_ROp_LT" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_L"))));
                        temp.value = "<";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }
                else if (ch == '>') // state 28
                {
                    if (i + 1 < input.size() && input[i + 1] == '=') // state 29
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: >=, Token: T_ROp_GE" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_GE"))));
                        temp.value = ">=";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '=' character
                    }
                    else // state 30
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_ROp_GT" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_G"))));
                        temp.value = ">";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }
                else if (ch == '!') // state 31
                {
                    if (i + 1 < input.size() && input[i + 1] == '=') // state 32
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: !=, Token: T_ROp_NE" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_ROp_NE"))));
                        temp.value = "!=";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '=' character
                    }
                    else // state 33
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_ⅬOp_NOT" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LOp_NOT"))));
                        temp.value = "!";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }
                else if (ch == '&') // state 34
                {
                    if (i + 1 < input.size() && input[i + 1] == '&') // state 35
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: &&, Token: T_LOp_AND" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LOp_AND"))));
                        temp.value = "&&";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '&' character
                    }
                    else // state 36
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Operator" << endl;
                        // Token temp;
                        // temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), TokenStrings.find("T_Operator"))));
                        // temp.value = "&";
                        // Tokens.push_back(temp);
                    }
                }
                else if (ch == '|') // state 37
                {
                    if (i + 1 < input.size() && input[i + 1] == '|') // state 38
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: ||, Token: T_ⅬOp_OR" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LOp_OR"))));
                        temp.value = "||";
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                        ++i; // Skip '|' character
                    }
                    else // state 39
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Operator" << endl;
                        // Token temp;
                        // temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), TokenStrings.find("T_Operator"))));
                        // temp.value = "|";
                        // Tokens.push_back(temp);
                    }
                }
                else if (ch == '(') // state 40
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_LP" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LP"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == ')') // state 41
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_RP" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_RP"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == '{') // state 42
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_LC" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LC"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == '}') // state 43
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_RC" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_RC"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == '[') // state 44
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_LB" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_LB"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == ']') // state 45
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_RB" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_RB"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == ',') // state 46
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Comma" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Comma"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else if (ch == '+') // state 47
                {
                    if (isdigit(input[i + 1]))
                        currentToken += ch;

                    else
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_AOp_PL" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_AOp_PL"))));
                        temp.value = ch;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }

                else if (ch == '-') // state 48
                {
                    if (isdigit(input[i + 1]))
                        currentToken += ch;

                    else
                    {
                        // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_AOp_MN" << endl;
                        Token temp;
                        temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_AOp_MN"))));
                        temp.value = ch;
                        temp.lineNumber=lineNo;
                        Tokens.push_back(temp);
                    }
                }

                else if (ch == '*') // state 49
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_AOp_ML" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_AOp_ML"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }

                else // state 50
                {
                    // cout << "Line: " << lineNo << ", Lexeme: " << ch << ", Token: T_Operator" << endl;
                    Token temp;
                    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_AOp_RM"))));
                    temp.value = ch;
                    temp.lineNumber=lineNo;
                    Tokens.push_back(temp);
                }
            }
            else if (ch == '\t') // state 15
            {
                // cout << "Line: " << lineNo << ", Lexeme: " << "Tab" << ", Token: T_Whitespace" << endl;
                // Token temp;
                // temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(),find( TokenStrings.begin(),TokenStrings.end(),"T_Whitespace"))));
                // temp.value = "Tab";
                // Tokens.push_back(temp);
            } // state 16

            else if (ch == '\n')
            { // state 15
                // cout << "Line: " << lineNo << ", Lexeme: " << "NewLine" << ", Token: T_Whitespace" << '\n'
                //  << '\n'; // state 16
                // Token temp;
                // temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(),find(TokenStrings.begin(),TokenStrings.end(),"T_Whitespace"))));
                // temp.value = "NewLine";
                // Tokens.push_back(temp);
                ++lineNo; // Increment line number
            }
            else if (ch == ' ') // state 15
            {
                // cout << "Line: " << lineNo << ", Lexeme: " << "Space" << ", Token: T_Whitespace" << endl;
                // Token temp;
                // temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_Whitespace"))));
                // temp.value = "Space";
                // Tokens.push_back(temp);
            } // state 16
        }
        else // state 12
        {
            currentToken += ch; // state 13

            if ((inChar || inString) && ch == '\n')
                newLine = !newLine; // Toggle the newLine flag

            // state 52, 53
            if (ch == '\'' && (input[i - 1] != '\\' || (input[i - 1] == '\\' && (input[i - 2] == '\\' || (input[i + 1] == ',' || input[i + 1] == ';')))))
                inChar = !inChar; // Toggle the inChar flag            state 54

            // state 55, 56
            if (ch == '"' && (input[i - 1] != '\\' || (input[i - 1] == '\\' && (input[i - 2] == '\\' || (input[i + 1] == ',' || input[i + 1] == ';')))))
                inString = !inString; // Toggle the inString flag       state 57
        } // state 14
    }
    Token temp;
    temp.type = static_cast<TokenType>(int(distance(TokenStrings.begin(), find(TokenStrings.begin(),TokenStrings.end(),"T_EOF"))));
    temp.value = "EOF";
    temp.lineNumber=-1;
    Tokens.push_back(temp);
}

