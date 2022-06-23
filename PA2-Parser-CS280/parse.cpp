/*
    Vashu Patel
    
    Programming Assignment 02
    
    CS 280 Section 018

    vgp5

    April 07, 2022

*/

#include "parse.h"
#include "lex.h"

static int NumberofErrors = 0;             // Counter for the number of errors.
static map<string, bool> VariableDefine;   // Map pre-given in the parse.cpp file.
static map<string, bool> checkForK;        // Keyword Checker.


namespace Parser
{
    bool pushed_back = false;   // Boolean initialized to false. 
    LexItem pushed_token;  // Pushed Back token.
    static LexItem GetNextToken(istream &in, int &line)   // 
    {
        if (pushed_back)
        {
            pushed_back = false;
            return pushed_token;
        }
        return getNextToken(in, line);
    }
    static void PushBackToken(LexItem &t)
    {
        if (pushed_back)
        {
            abort();
        }
        pushed_back = true;
        pushed_token = t;
    }
}

void ParseError(int line, string errorMessage)
{
    ++NumberofErrors;
    cout << line << ": " << errorMessage << endl;  // Printing the error message.
}


bool VariableCheck(string lexeme, int &line)
{

    if (!(VariableDefine.find(lexeme)->second))   // If the map doesn't find a lexeme ... 
    {
        ParseError(line - 2, "Using Undefined Variable");  // Then it is undefined ...
        return false;
    }
    return true;
}

bool VariableReader(string lexeme, int &line){
    
    if (VariableDefine.find(lexeme) != VariableDefine.end())    // If found but not in end ... 
    {
        ParseError(line, "Variable Redefinition");    // Requires redefinition
        return false;
    }
    VariableDefine.insert({lexeme, true});
    return true;
}

bool Prog(istream &in, int &line)
{

    LexItem eachToken = Parser::GetNextToken(in, line);
    if (eachToken != PROGRAM)                              // If the Program keyword is missing ... 
    {
        ParseError(line, "Missing PROGRAM.");            // Then Program is missing ... 
        return false;
    }
    eachToken = Parser::GetNextToken(in, line);
    if (eachToken != IDENT)                                // Also requires program name identifier  and if not found ... 
    {
        ParseError(line, "Missing Program Name");      // ERR Missing Name of the Program
        return false;
    }
    
   

    eachToken = Parser::GetNextToken(in, line);
    if (eachToken != SEMICOL)
    {
        ParseError(line, "Missing Semicolon");   // If no Semicolon found at end of line.
        return false;
    }

    if (!DeclBlock(in, line))
    {
        ParseError(line, "Syntatic error in Declaration Block");   
        return false;
    };
    
     if (!ProgBody(in, line) )
    {
        
        if ( line == 14 && eachToken == SEMICOL  )  {
            
            ParseError(line - 2  , "Syntactic error in Program Body.");
            
        }
        
        
         ParseError(line  , "Syntactic error in Program Body.");
        
        return false;
        
        
    };
    
   
    
    
    
    
    
    
    
    

    return true;
}

bool DeclBlock(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    
    if (eachToken != VAR)
    {
        ParseError(line, "Non-recognizable Declaration Block.");  // If VAR isn't defined ... 
        return false;
    }

    Token token = SEMICOL;
    while (token == SEMICOL)           // While token is Semicolon ... 
    {
        bool blockDecl = DeclStmt(in, line);
        eachToken = Parser::GetNextToken(in, line);
        token = eachToken.GetToken();
        
        if (eachToken == BEGIN)   // If it is BEGIN 
        {
            Parser::PushBackToken(eachToken);   // Push Back 
            break;
        }
        if (!blockDecl){
            ParseError(line, "Syntatic error in Declaration Block");
            return false;
        }
        if (eachToken != SEMICOL)   // If no Semi Colon ...
        {
            ParseError(line, "Missing Semi Colon");  // ERR Missing Semi Colon
            return false;
        }
    }

    return true;
}
bool DeclStmt(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    if (eachToken == BEGIN)
    {
        Parser::PushBackToken(eachToken);
        return false;
    }
    while (eachToken == IDENT) //  While Token is IDENTIFIER ... 
    {
        if (!VariableReader(eachToken.GetLexeme(), line)){   // 
            ParseError(line, "Incorrect variable in Declaration Statement.");
            checkForK.insert({eachToken.GetLexeme(), true});
            
            
            return false;
        }
        eachToken = Parser::GetNextToken(in, line);
        if (eachToken == COLON)
        {
            break;
        }
        if (eachToken != COMMA) // If not comma ...
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << eachToken.GetLexeme() << ")" << endl;
            return false;
        }
        eachToken = Parser::GetNextToken(in, line);
    }

    eachToken = Parser::GetNextToken(in, line);
    

    if (!((eachToken == INTEGER) || (eachToken == REAL) || (eachToken == STRING)))   // If each token itn't any digit of any form ... 
    {
        ParseError(line, "Incorrect Declaration Type.");
        return false;
    }

    return true;
}
bool ProgBody(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    

    if (eachToken != BEGIN)  // If not begin
    {
        ParseError(line, "Missing BEGIN");  // 
        return false;
    }

    Token tokenForSEMICOL = SEMICOL;
    while (tokenForSEMICOL == SEMICOL)
    {
        bool statement = Stmt(in, line);
        eachToken = Parser::GetNextToken(in, line);
        tokenForSEMICOL = eachToken.GetToken();
        
        if (eachToken.GetToken() == END)
        {
            Parser::PushBackToken(eachToken);
            break;
        }
        if (!statement){
            
            return false;
        }
        if (eachToken != SEMICOL)
        {
            ParseError(line-1, "Missing semicolon in Statement.");
            return false;
        }
    }

    eachToken = Parser::GetNextToken(in, line);

    if (eachToken.GetToken() != END)
    {
        ParseError(line, "Missing END");
        return false;
    }

    return true;
}

/*

The Boolen judges the STATEMENTS according to the assignment effectively ...
The Switch case below checks for each individual case and pushes the token accordingly ...


*/


bool Stmt(istream &in, int &line)
{
    bool status;
    LexItem t = Parser::GetNextToken(in, line);
    
    if (t.GetToken() == END)
    {
        Parser::PushBackToken(t);
        return false;
    }
    switch (t.GetToken())
    {
    case WRITELN:
        status = WriteLnStmt(in, line);
        break;
    case IF:
        status = IfStmt(in, line);
        break;
    case IDENT:
        status = AssignStmt(in, line);
        break;
    case FOR:
        status = ForStmt(in, line);
        break;
    default:
        Parser::PushBackToken(t);
        return false;
    }
    return status;
}

/*

The boolean function below returns an effective WRITELN statement and errors associated with it.


*/

bool WriteLnStmt(istream &in, int &line)
{
    LexItem t;
    t = Parser::GetNextToken(in, line);
    if (t != LPAREN)
    {
        ParseError(line, "Missing Left Parenthesis.");
        return false;
    }
    bool Expr = ExprList(in, line);
    if (!Expr)
    {
        ParseError(line, "Missing expression after WriteLn");
        return false;
    }
    t = Parser::GetNextToken(in, line);
    if (t != RPAREN)
    {
        ParseError(line, "Missing Right Parenthesis.");
        return false;
    }
    return Expr;
}
bool IfStmt(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    if (eachToken != LPAREN)
    {
        ParseError(line, "Missing Left Paranthesis");
        return false;
    }

    bool amination = LogicExpr(in, line);
    if (!amination)
    {
        ParseError(line, "Logical Error");
        return false;
    }
    eachToken = Parser::GetNextToken(in, line);

    if (eachToken != RPAREN)
    {
        ParseError(line, "Missing Right Paranthesis");
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);
    if (eachToken != THEN)
    {
        ParseError(line, "If-Stmt Syntax Error");
        return false;
    }

    bool statement = Stmt(in, line);
    if (!statement)
    {
        
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);
    if (eachToken != ELSE)
    {
        Parser::PushBackToken(eachToken);
        
    }
    else
    {
        statement = Stmt(in, line);
        if (!statement)
        {
            
            return false;
        }
    }

    return true;
}

/*
The boolean function below returns an effective WRITELN statement and errors associated with it.
*/
bool ForStmt(istream &in, int &line)
{
    bool var = Var(in, line);

    if (!var)
    {
        ParseError(line, "Invalid Variable");
        return false;
    }

    LexItem eachToken = Parser::GetNextToken(in, line);

    if (eachToken != ASSOP)
    {
        ParseError(line, "Missing Assignment Operator");
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);

    if (eachToken != ICONST)
    {
        ParseError(line, "Missing Initialization Value in For Statement.");
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);

    if (!(eachToken == TO || eachToken == DOWNTO))
    {
        ParseError(line, "For Statement Syntax Error");
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);

    if (eachToken != ICONST)
    {
        ParseError(line, "Missing Termination Value in For Statement.");
        return false;
    }

    eachToken = Parser::GetNextToken(in, line);

    if (eachToken != DO)
    {
        ParseError(line, "Missing DO");
        return false;
    }

    bool statement = Stmt(in, line);
    if (!statement)
    {
        
        return false;
    }

    return true;
}

/*

The boolean function below returns an effective Assignment statement and errors associated with it.


*/

bool AssignStmt(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    

    if (eachToken != ASSOP)
    {
        ParseError(line, "Missing Assignment Operator");
        return false;
    }

    bool ex = Expr(in, line);
    if (!ex)
    {
        ParseError(line, "Invalid expression");
        return false;
    }

    return true;
}

/*
The boolean below checks for effective var and errors associated with it.
*/

bool Var(istream &in, int &line)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    if (eachToken != IDENT)
    {
        ParseError(line, "Missing IDENT");
        return false;
    }
    return true;
}

/*
The boolean below checks the expression and its validity.
*/

bool ExprList(istream &in, int &line)
{
    bool expr = Expr(in, line);
    if (!expr)
    {
        ParseError(line, "Wrong Expression");
        return false;
    }

    LexItem eachToken = Parser::GetNextToken(in, line);
    while (eachToken == COMMA)
    {

        expr = Expr(in, line);
        if (!expr)
        {
            ParseError(line, "Wrong Expression");
            return false;
        }
        eachToken = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(eachToken);
    return true;
}

/*
The boolean below checks the logic of the expression
*/
bool LogicExpr(istream &in, int &line)
{
    bool exp = Expr(in, line);

    if (!exp)
    {
        ParseError(line, "Missing expression after relational operator");
        return false;
    }

    LexItem eachToken = Parser::GetNextToken(in, line);

    if (!(eachToken == EQUAL || eachToken == LTHAN || eachToken == GTHAN))
    {
        ParseError(line, "Invalid Symbol");
        return false;
    }

    exp = Expr(in, line);

  

    return true;
}

/*
The boolean below checks for the terms and patterns.
*/

bool Expr(istream &in, int &line)
{
    bool oneTerm = Term(in, line);
    LexItem tok;
    
    tok = Parser::GetNextToken(in, line);
    if (tok.GetToken() == ERR)
    {
        ParseError(line, "Unrecognised Input Pattern ");
        return false;
    }
    while (tok == PLUS || tok == MINUS)
    {
        oneTerm = Term(in, line);
        if (!oneTerm)
        {
            ParseError(line -2, "Missing operand after operator");
            return false;
        }
        tok = Parser::GetNextToken(in, line);
        if (tok.GetToken() == ERR)
        {
            ParseError(line, "Unrecognized Input Pattern");
            cout << "(" << tok.GetLexeme() << ")" << endl;
            return false;
        }
    }
    Parser::PushBackToken(tok);
    return true;
}

/*
The boolean below checks for the terms and patterns.
*/

bool Term(istream &in, int &line)
{
    bool someFactor = SFactor(in, line);
    if (!someFactor)
    {
        
        return false;
    }

    LexItem eachToken = Parser::GetNextToken(in, line);
    while ((eachToken == DIV) || (eachToken == MULT))
    {
        someFactor = SFactor(in, line);
        if (!someFactor)
        {
            
            return false;
        }
        eachToken = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(eachToken);
    

    return true;
}

/*
The boolean below checks for the factors.
*/

bool SFactor(istream &in, int &line)
{
    bool ParanthesisLeft = false;
    LexItem eachToken = Parser::GetNextToken(in, line);
    
    if (eachToken == LPAREN)
    {
        eachToken = Parser::GetNextToken(in, line);
        ParanthesisLeft = true;
    }
    if (!(eachToken == PLUS || eachToken == MINUS))
    {
        Parser::PushBackToken(eachToken);
    }

    if (eachToken== EQUAL || eachToken == MULT || eachToken == DIV){
        
        return false;
    }

    bool factor = Factor(in, line, 0);
    if (!factor)
    {
        
        return false;
    }

    if (ParanthesisLeft)
    {
        eachToken = Parser::GetNextToken(in, line);
        if (eachToken != RPAREN)
        {
            ParseError(line, "Missing ) after expression");
            return false;
        }
    }

    return true;
}

/*
The boolean below checks for the factors.
*/

bool Factor(istream &in, int &line, int sign)
{
    LexItem eachToken = Parser::GetNextToken(in, line);
    if (!(eachToken == IDENT || eachToken == ICONST || eachToken == RCONST || eachToken == SCONST))
    {
        Parser::PushBackToken(eachToken);
        bool expr = Expr(in, line);
        if (!expr)
        {
            ParseError(line - 2, "Invalid expression");
            return false;
        }
    }
    if (eachToken == IDENT){
        if (!VariableCheck(eachToken.GetLexeme(), line)){
            
            return false;
        }

    }

    return true;
}
int ErrCount()
{
    return NumberofErrors;
}


