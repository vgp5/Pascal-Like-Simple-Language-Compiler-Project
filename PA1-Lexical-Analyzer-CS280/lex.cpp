/*
    Vashu Patel
    
    Programming Assignment 01
    
    CS 280 Section 018

    vgp5

    March 06, 2022

*/





#include <iostream>
#include <regex>
#include <cstring>
#include <map>
#include "lex.h"

using std::map;
using namespace std;


 //   Creating a map for every single token 


static map<Token,string> EverySingleToken = { // Keywords
                                         {PROGRAM, "PROGRAM"}, {WRITELN, "WRITELN"},  
                                         { INTEGER, "INTEGER"},{BEGIN,"BEGIN"}, {END,"END"}, { IF, "IF" }, 
                                         { REAL, "REAL"}, {STRING,"STRING"},{VAR,"VAR"},
                                         {ELSE,"ELSE"},{FOR,"FOR"},{THEN, "THEN"}, { DO, "DO" }, {TO,"TO"}, {DOWNTO,"DOWNTO"},
                                         // Identifier 
                                         { IDENT, "IDENT" }, 
                                         // Integer, Real and String Constant
                                         { ICONST, "ICONST" }, { RCONST, "RCONST" }, { SCONST, "SCONST" },
                                         //Operators
                                         { PLUS, "PLUS" }, { MINUS, "MINUS" }, { MULT, "MULT" }, { DIV, "DIV" }, { ASSOP, "ASSOP" }, 
                                         { LPAREN, "LPAREN" }, { RPAREN, "RPAREN" }, { COLON, "COLON" }, {COMMA, "COMMA" }, 
                                         { EQUAL, "EQUAL" },  
                                         { GTHAN, "GTHAN" }, {LTHAN,"LTHAN"},{SEMICOL,"SEMICOL"}, 
                                         // Error and Done (EOF)
                                         { ERR, "ERR" },{ DONE, "DONE" } };

// Creating a map for only keywords in order to effectively identify and use the id_or_kw function.

static map<string,Token> KeyWordsOnly = {{"PROGRAM", PROGRAM}, {"WRITELN", WRITELN}, 
                                         { "INTEGER", INTEGER},{"BEGIN",BEGIN}, {"END", END}, { "IF", IF }, 
                                         { "REAL", REAL}, {"STRING",STRING},{"VAR",VAR},{"ELSE",ELSE},
                                         {"FOR",FOR},{"THEN", THEN}, { "DO", DO }, {"TO",TO}, {"DOWNTO",DOWNTO}};


// Creating an overload << operator.

ostream& operator<<(ostream& OutPut, const LexItem& tock){      // Overloading << Operator to print, OutPut is the variable that judges cout
    Token tick = tock.GetToken();      // Getting the token
        if (tick == IDENT || tick == ICONST || tick == SCONST || tick == RCONST
			|| tick == ERR){
        
            OutPut<<EverySingleToken[tick]<<" ("<<tock.GetLexeme()<<")";   //OutPutting (printing) the statements.
        }   
    
   
    return OutPut;
}



// Creating an id_or_kw function to identify which  is ident or keyword.


LexItem id_or_kw(const string& lexeme, int linenum)
{
	Token tok = IDENT;

	auto keyIt = KeyWordsOnly.find(lexeme);
	if( keyIt != KeyWordsOnly.end() ){
		tok = keyIt->second;
    }

	return LexItem(tok, lexeme, linenum);
}





bool stuff = false;      // Assigning a boolean to later check the INCOMMENT state.

// getNextToken Function

LexItem getNextToken(istream& in, int& linenumber){

    enum TokState {

        /*
         ===================================================================
         
         Start is state to begin the check, INID is to check identifiers, 
         Instring is to check for strings that start with ''',
         ININT is to check any integers, INDOT is to check if the interger 
         follows with a decimal point, If the point is followed by a valid,
         int, then it goes to INREAL state. INCOMMENT state checks if a 
         comment is valid or not.
        
         ===================================================================
        */
        
        START, INID, INSTRING, ININT, INDOT, INREAL, INCOMMENT};    
    
    TokState currentToken = START;
    string lexeme;
    char ch;
    bool commentCheck = false;
    
    
    while(in.get(ch)){     // While the input file gets and iterates over each character...
    
        switch(currentToken){    // A switch is initiated to perform the cases.
        
            case START:
                if(ch == '\n'){     // Any new line is a valid incremental to linenumber.
                
                    linenumber++;
                }
                if(isspace(ch)){    // If it has a space let it continue...
                    
                    continue;
                }
                else if(ch == '\''){     // If the character starts with a single string, it may be a valid string, changing the state to INSTRING.
                    lexeme += ch;     // Add that to the lexeme.
                    currentToken = INSTRING;
                    
                }
                lexeme = ch;
                
                if(isalpha(ch)){         // If the character is alphanumeric, 
                    lexeme = toupper(ch);     // Let the lexeme become uppercase (as per the requirements of the assignment)
                    currentToken = INID;     // The state will then check for INID
                
                }
               
               /* 
               ======================================================================
               The else-if statements below will judge if the characters are operators
               
               ======================================================================
               */ 
                
                
                
                
                else if(isdigit(ch)){   
                    currentToken = ININT;
                }
                else if (ch == '_'){
                    
                    currentToken = INID;
                }
                else if(ch == '.'){
                
                    currentToken = INDOT;
                }
                else if(ch == '+'){
                    return LexItem(PLUS, lexeme, linenumber);
                }
                else if(ch == '-'){
                    return LexItem(MINUS, lexeme, linenumber);
                }
                else if(ch == '*'){
                
                    return LexItem(MULT, lexeme, linenumber);
                }
                else if(ch == '/'){
                    return LexItem(DIV, lexeme, linenumber);
                }
               else if(ch == '='){
                    return LexItem(EQUAL, lexeme, linenumber);
                    
                }
                /*
                The else if below will check if the char is a left parentheses 
                and if it is followed by a * then it may be a valid comment.
                
                */
                
                else if(ch == '('){
                    if(in.peek() == '*'){
                        currentToken = INCOMMENT;
                    }
                    else{
                        return LexItem(LPAREN, lexeme, linenumber);
                    }
                    break;
                }
                
                
                else if(ch == ')'){
                
                    if (commentCheck == true){
                        continue;
                    }

                    else{
                        lexeme += ch;
                        return LexItem(RPAREN, lexeme, linenumber);
                    }
                    
                    //break;
                }
                
                else if(ch == ',')
                {
                    return LexItem(COMMA, lexeme, linenumber);
                }
                
                else if(ch == ':')
                {
                  if(in.peek()== '='){
                      in.get(ch);
                      lexeme = lexeme + ch;
                      return LexItem(ASSOP, lexeme, linenumber);
                  }
                 
                  else{
                      return LexItem(COLON, lexeme, linenumber);
                      }
                
                } 
                
                else if(ch == ';'){
                    return LexItem(SEMICOL, lexeme, linenumber);
                }
                
                else if(ch == '>'){
                
                    return LexItem(GTHAN, lexeme, linenumber);
                }
                else if(ch == '<'){
                
                    return LexItem(LTHAN, lexeme, linenumber);
                }
                else if (ch == '?') {
                    //lexeme += ch;
                    return LexItem (ERR, lexeme, linenumber+1);
                    }
                break;
            case INID:
                if (isalnum(ch)){
                    lexeme += toupper(ch);
                    currentToken = INID;
                
                }
                
                else if (ch== '_'){
                
                    lexeme += toupper(ch);
                 
               }
                    
                
                else {
                    in.putback(ch);
                    return id_or_kw(lexeme, linenumber);
                }
                break;
             
            case ININT:
                if(isdigit(ch)){
                    lexeme += ch;
                    currentToken = ININT;
                }
                
                else if(ch == '.'){
                
                    if(!isdigit(in.peek())){
                        return LexItem(ERR, lexeme, linenumber+1);
                    }
                    lexeme+= ch;
                    currentToken = INDOT;
                }
                else {
                
                    in.putback(ch);
                    return LexItem(ICONST, lexeme, linenumber);
                
                }
                break;
             
            case INDOT:
                lexeme += ch;
                if (isdigit(ch)){
                    currentToken = INREAL;
                }
                else {
                    in.putback(ch);
                    return LexItem(ERR, lexeme, linenumber+1);
                }
                break;
            case INREAL:
                if (isdigit(ch)){
                    lexeme += ch;
                    if(in.peek() == '.'){
                        lexeme+= '.';
                        return LexItem(ERR, lexeme, linenumber+1);
                    }
                    currentToken = INREAL;
                }
                
                else {
                    in.putback(ch);
                    if (lexeme.at(0) == '.'){
                    
                    return LexItem(RCONST, "0" + lexeme, linenumber);
                    }
                   return LexItem(RCONST, lexeme, linenumber);
                }
                break;
                
            case INSTRING:
                if(ch == '\n'){
                    linenumber++;
                    return LexItem(ERR, lexeme, linenumber);
                
                }
                
                else if (ch == '\''){
                    lexeme += ch;
                    lexeme = lexeme.substr(0, lexeme.size());
                     
                    return LexItem(SCONST, lexeme, linenumber);
                    
                }
                
                
                else {
                    lexeme += ch;
                    currentToken = INSTRING;
                }
                
                break;
                
            case INCOMMENT:
                stuff = true;
                if (ch == '*' && in.peek() == ')'){
                   // linenumber++;
                    stuff = false;
                    commentCheck =  true;
                    currentToken = START;
                    continue;
                }
                
                else if (ch == '\n') {
                   
                    linenumber++;
                }
                break;
        
        }
     
    
    
    
    }
    // If the boolean valued is true it means that there is no ending comment end characters building an error.
    if (stuff == true){
        cout <<endl<<"Missing a comment end delimiters '*)' at line "<<linenumber <<endl;
    }
    return LexItem(DONE, lexeme, linenumber);

// Return DONE after entire file is checked.




}

