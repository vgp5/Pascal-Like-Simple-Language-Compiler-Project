/*
    Vashu Patel
    
    Programming Assignment 01
    
    CS 280 Section 018

    vgp5

    March 06, 2022

*/

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>
#include <algorithm>
#include <iostream>
#include "lex.h"

using namespace std;

int main(int argc, char *argv[])
{
    
    /*
    
    =======================================================================================================
    
    Creating boolean values to check if the types of flags will late be true or not based on user input on
    command line argument.
    
    Initializing vectors for each one  and lNum to count the number of lines, tokNum to count the token
    numbers and fileCount if user tries to input more than one file name.
    
    Creating a new LexItem named tok.
    
    =======================================================================================================
    */
    
    
    
    string s = "";
    int x = 0;
    string trim = "";  
    
    ifstream infile;
    bool vFlag = false;
    bool iConstFlag = false;
    bool rConstFlag = false;
    bool sConstFlag = false;
    bool idsFlag = false;
    vector<string> identVec;
    vector<string> stringVec;
    vector<string> realVec;
    vector<int> intVec;
    int lNum = 0;
    int tokNum = 0;
    int fileCount = 0;
    LexItem tok;
    
    /*
    
    =======================================================================================================
    
    Checks all the flags based on user input, if the argument is that certain flag, then its true.
    
    =======================================================================================================
    */
    
    if(argc >= 2)
    {
        for(int i = 1; i < argc; i++)
        {
            string arg = string(argv[i]);
            if (arg == "-v") {
                vFlag = true;
            } else if (arg == "-iconst") {
                iConstFlag = true;
            } else if (arg == "-rconst") {
                rConstFlag = true;
            } else if (arg == "-sconst") {
                sConstFlag = true;
            } else if (arg == "-ident") {
                idsFlag = true;
            }
            else if (arg.at(0) == '-')
            {
                cerr << "UNRECOGNIZED FLAG " << arg << endl;
                exit(-1);
            }
           
            else if (arg.at(0) != '-'){
                fileCount++;
            }
                     
        }
   
    
        if (fileCount > 1){
                cerr << "ONLY ONE FILE NAME ALLOWED."<<endl;
                exit(1);
            }
        
                infile.open(argv[1]);
                if(!infile.is_open())
                {
                    cerr << "CANNOT OPEN the File " << argv[1] << endl; 
                    exit(1);
                }
             
     }
    else
    {
        cerr << "NO SPECIFIED INPUT FILE NAME." << endl;
        exit(1);
    }
    
    while ((tok = getNextToken(infile, lNum)) != DONE && tok != ERR)
    {
        if(vFlag == true)
        {
            switch(tok.GetToken())
            {
                case(IDENT):
                    cout << "IDENT" << "(" << tok.GetLexeme() << ")" << endl;
                    break;
                case(ICONST):
                    cout << "ICONST" << "(" << tok.GetLexeme() << ")" << endl;
                    break;
                case(RCONST):
                    cout << "RCONST" << "(" << tok.GetLexeme() << ")" << endl;
                    break;
                case(SCONST):
                    s = tok.GetLexeme();
                    x = s.length();
                    trim = s.substr(1, x-2);
                    cout << "SCONST" << "(" << trim << ")" << endl;
                    break;
                case(PROGRAM):
                    cout << "PROGRAM" << endl;
                    break;
               
                case(IF):
                    cout << "IF" << endl;
                    break;
                case(THEN):
                    cout << "THEN" << endl;
                    break;
                case(INTEGER):
                    cout << "INTEGER" << endl;
                    break;
                case(REAL):
                    cout << "REAL" << endl;
                    break;
                
               case(STRING):
                    cout << "STRING" <<endl;
                    break;
                    
                case(BEGIN):
                    cout << "BEGIN" << endl;
                    break;
                case(END):
                    cout << "END" << endl;
                    break;
                case(WRITELN):
                    cout << "WRITELN" << endl;
                    break;
                case(DO):
                    cout << "DO" << endl;
                    break;
                
                case(TO):
                    cout << "TO" <<endl;
                    break;
                    
                case(DOWNTO):
                    cout<< "DOWNTO"<<endl;
                    break;
                    
                case(VAR):
                    cout<< "VAR"<<endl;
                    break;
              
                case(ELSE):
                    cout << "ELSE"<<endl;
                    break;
                
                case(FOR):
                    cout << "FOR"<< endl;
                    break;
                    
                
                  
                case(PLUS):
                    cout << "PLUS" << endl;
                    break;
                case(MINUS):
                    cout << "MINUS" << endl;
                    break;
                case(MULT):
                    cout << "MULT" << endl;
                    break;
                
                case(ASSOP):
                    cout << "ASSOP" << endl;
                    break;
                case(LPAREN):
                    cout << "LPAREN" << endl;
                    break;
                case(RPAREN):
                    cout << "RPAREN" << endl;
                    break;
                case(EQUAL):
                    cout << "EQUAL" << endl;
                    break;
                case(LTHAN):
                    cout << "LTHAN" << endl;
                    break;
                    
                case(DIV):
                    cout << "DIV" << endl;
                    break;
                    
                case(GTHAN):
                    cout << "GTHAN" << endl;
                    break;
                case(COLON):
                    cout << "COLON" << endl;
                    break;
                    
                case(SEMICOL):
                    cout << "SEMICOL" <<endl;
                    break;
                case(DOT):
                    cout << "DOT" << endl;
                    break;
                case(COMMA):
                    cout << "COMMA" << endl;
                    break;
                default:
                    break;
            }
        }
        
        
        /*
    
    =======================================================================================================
    
    Checks all the boolean values and uses the vector to get that lexeme to work with.
    
    
    
    =======================================================================================================
    */
        
        if(tok == IDENT && idsFlag)
        {
            identVec.push_back(tok.GetLexeme());
        }
        else if(tok == SCONST && sConstFlag)
        {
            stringVec.push_back(tok.GetLexeme());
        }
        else if(tok == RCONST && rConstFlag)
        {
            realVec.push_back(tok.GetLexeme());
        }
        
        /*
    
    =======================================================================================================
    
    The else if statement below if the token is of integer family and extracts the integers (that are in 
    string format) to convert them into proper integers for easier sorting.
    
    =======================================================================================================
    */
        
        else if(tok == ICONST && iConstFlag)
        {
            string inttok = tok.GetLexeme();
            stringstream iss (inttok);
            
            int Int;
            while (iss >> Int)
                intVec.push_back(Int);
        }
        tokNum++;
    }
    
    
    infile.close();
    
    /*
    
    =======================================================================================================
    If the token is ERR, print the error statement.
    =======================================================================================================
    */
    
     if(tok.GetToken() == ERR)
       {
     cerr << "Error in line " << tok.GetLinenum() << " (" << tok.GetLexeme() << ")" << endl;
        
       }
    
    /*
    
    =======================================================================================================
    
    If its not an error, then print the lines number, the strings or integers or reals or identifiers
    based on the flags.
    
    =======================================================================================================
    */
    
    
    else
    {
        cout << "Lines: " << lNum << endl;
        if(lNum != 0)
            cout << "Tokens: " << tokNum << endl;
        if(sConstFlag == true)
        {
            sort(stringVec.begin(), stringVec.end());
            cout << "STRINGS:" << endl;
            vector<string>::iterator it1;
            for(it1 = stringVec.begin(); it1 != stringVec.end(); it1++)
            {
                cout << *it1 << endl;
            }
        }
        if(iConstFlag == true)
        {
            sort(intVec.begin(), intVec.end());
            intVec.erase( unique(intVec.begin(), intVec.end()), intVec.end());
            cout << "INTEGERS:" << endl;
            vector<int>::iterator it;
            for(it = intVec.begin(); it != intVec.end(); it++)
            {
                cout << *it << endl;
            }
        }
        if(rConstFlag == true)
        {
            sort(realVec.begin(), realVec.end());
            cout << "REALS:" << endl;
            vector<string>::iterator it;
            for(it = realVec.begin(); it != realVec.end(); it++)
            {
                if(*it == "0.0"){
                    *it = "0";
                }
                cout << *it << endl;
            }
        }
        if(idsFlag == true)
        {
            sort(identVec.begin(), identVec.end());
            identVec.erase( unique(identVec.begin(), identVec.end()), identVec.end());
            cout << "IDENTIFIERS:" << endl;
            vector<string>::iterator it;
            
            for(it = identVec.begin(); it != identVec.end(); it++)
            {
                cout << *it;
                if ( std:: next(it) != identVec.end()){
                    cout<<", ";
                }
                else
                    cout<<endl;
            }
        
           
        
        }
      
        
        
        
    }
    return 0;
}