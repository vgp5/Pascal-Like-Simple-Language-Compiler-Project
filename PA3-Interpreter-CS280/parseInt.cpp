/* Implementation of PLSL Interpreter
 * parseInt.cpp
 * Programming Assignment 3
 * Spring 2022
 */

#include <vector>
#include <string>
#include<cmath>
#include "parseInt.h"
#include "val.h"

using namespace std;
map<string, bool> defVar;
map<string, Token> SymTable;
map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> *ValQue;			 // declare a pointer variable to a queue of Value objects

namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
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

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool checkVar(string lexeme, int &line)
{

	if (!(defVar.find(lexeme) -> second))
	{
		ParseError(line, "Using Undefined Variable");
		return false;
	}
	return true;
}

void OperationofUnary(int AddorSubtract, Value &ValuetoReturn)
{
	if (AddorSubtract == 0){
		return;
	}

	if (ValuetoReturn.IsInt())
	{
		ValuetoReturn.SetInt(ValuetoReturn.GetInt() * AddorSubtract);
	}
	else if (ValuetoReturn.IsReal())
	{
		ValuetoReturn.SetReal(ValuetoReturn.GetReal() * AddorSubtract);
	}
	else{
		ValuetoReturn = Value();
	}
}

bool updateValues(string varname, Value value, int line)
{
	if (!checkVar(varname, line))
	{
		ParseError(line, "Using Undefined Variable");
		return false;
	}

	Token tk = SymTable.find(varname)->second;

	if (((tk == STRING) || (tk == SCONST)) || (value.GetType() == VSTRING)){
		if (!(((tk == STRING) || (tk == SCONST)) & (value.GetType() == VSTRING))){
			
			return false;
		}
	}

	if (tk == REAL){
		if (value.IsInt()){
			
			value = Value((float)value.GetInt());
		}
	}

	if (tk == INTEGER){
		if (value.IsReal()){
			
			value = Value((int)value.GetReal());
		}
	}

	if (TempsResults.find(varname) != TempsResults.end()){
		TempsResults.find(varname)->second = value;
	}
	else{
		TempsResults.insert({varname, value});
	}

	return true;
}

Value ValueCatcherVar(LexItem tk, int line)
{
	if (!checkVar(tk.GetLexeme(), line))
	{
		ParseError(line, "Using Undefined Variable");
		return Value();
	}
	
	if (TempsResults.find(tk.GetLexeme()) != TempsResults.end()){
		return TempsResults.find(tk.GetLexeme())->second;
	}
	return Value();
}

Value ConstValue(LexItem ItemL, int line)
{
	Token tk = ItemL.GetToken();
	string stuff = ItemL.GetLexeme();
	
	if (tk == ICONST){
		return Value(stoi(stuff));
      }
	else if (tk == RCONST){
		return Value(stof(stuff));
      }
	else if (tk == SCONST){
		return Value(stuff);
      }
	else if (tk == IDENT){
		return ValueCatcherVar(ItemL, line);
      }
	return Value();
}


Value setValue(LexItem tk, int line)
{

	if (!checkVar(tk.GetLexeme(), line))
	{
		ParseError(line, "Using Undefined Variable");
		return Value();
	}
	Token token = SymTable.find(tk.GetLexeme())->second;

	if (token == INTEGER){
		return Value((int)0);
      }
	else if (token == REAL){
		return Value((float)0.0);
      }
	else if (token == STRING){
		return Value("");
      }
	return Value();
}

bool addVar(string lexeme, Token tk, int &line)
{
	
	if (!(defVar.find(lexeme) == defVar.end()))
	{
		ParseError(line, "Variable Redefinition");
		return false;
	}
	defVar.insert({lexeme, true});
	
	return true;
}

void renovateSym(Token tk){

	map<string, bool>::iterator eachToken;

	for (eachToken = defVar.begin(); eachToken != defVar.end(); eachToken++){
		if (SymTable.find(eachToken->first) == SymTable.end()){
			SymTable.insert({eachToken->first, tk});
		}

	}
}


bool Prog(istream &in, int &line)
{

	LexItem tk = Parser::GetNextToken(in, line);
	
	if (tk != PROGRAM)
	{
		ParseError(line, "Missing PROGRAM.");
		return false;
	}
	tk = Parser::GetNextToken(in, line);
	if (tk != IDENT)
	{
		ParseError(line, "Missing Program Name");
		return false;
	}
	

	tk = Parser::GetNextToken(in, line);
	if (tk != SEMICOL)
	{
		ParseError(line, "Semicolon not found");
		return false;
	}

	if (!DeclBlock(in, line))
	{
		ParseError(line, "Declaration Body Error");
		return false;
	};
	if (!ProgBody(in, line))
	{
		ParseError(line, "Prog Body Error");
		return false;
	};

	return true;
}

bool DeclBlock(istream &in, int &line)
{
	LexItem tk = Parser::GetNextToken(in, line);
	
	if (tk != VAR)
	{
		ParseError(line, "Non-recognizable Declaration Block.");
		return false;
	}

	Token token = SEMICOL;
	while (token == SEMICOL)
	{
		bool decl = DeclStmt(in, line);
		tk = Parser::GetNextToken(in, line);
		token = tk.GetToken();
		
		if (tk == BEGIN)
		{
			Parser::PushBackToken(tk);
			break;
		}
		if (!decl)
		{
			ParseError(line, "Declaration Body Error");
			return false;
		}
		if (tk != SEMICOL)
		{
			ParseError(line, "Missing Semi Colon");
			return false;
		}
	}

	return true;
}
bool DeclStmt(istream &in, int &line)
{
	LexItem tk = Parser::GetNextToken(in, line);
	if (tk == BEGIN)
	{
		Parser::PushBackToken(tk);
		return false;
	}
	while (tk == IDENT)
	{
		if (!addVar(tk.GetLexeme(), tk.GetToken(), line))
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return false;
		}
		tk = Parser::GetNextToken(in, line);
		if (tk == COLON)
		{
			break;
		}
		if (tk != COMMA)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tk.GetLexeme() << ")" << endl;
			return false;
		}
		tk = Parser::GetNextToken(in, line);
	}

	tk = Parser::GetNextToken(in, line);
	// cout << tk.GetLexeme() << endl;

	if (!((tk == INTEGER) || (tk == REAL) || (tk == STRING)))
	{
		ParseError(line, "Incorrect Declaration Type");
		return false;
	}
	renovateSym(tk.GetToken());

	return true;
}
bool ProgBody(istream &in, int &line)
{
	LexItem tk = Parser::GetNextToken(in, line);
	

	if (tk != BEGIN)
	{
		ParseError(line, "BEGIN missing");
		return false;
	}

	Token token = SEMICOL;
	while (token == SEMICOL)
	{
		bool stmt = Stmt(in, line);
		tk = Parser::GetNextToken(in, line);
		token = tk.GetToken();
		
		if (tk.GetToken() == END)
		{
			Parser::PushBackToken(tk);
			break;
		}
		if (!stmt)
		{
			ParseError(line, "Syntactic error in Program Body.");
			return false;
		}
		if (tk != SEMICOL)
		{
			ParseError(line, "Missing semicolon in Statement.");
			return false;
		}
	}

	tk = Parser::GetNextToken(in, line);

	if (tk.GetToken() != END)
	{
		ParseError(line, "END not found");
		return false;
	}

	return true;
}
bool Stmt(istream &in, int &line)
{
	bool progress;
	LexItem t = Parser::GetNextToken(in, line);
	
	if (t.GetToken() == END)
	{
		Parser::PushBackToken(t);
		return false;
	}
	switch (t.GetToken())
	{
	case WRITELN:
		progress = WriteLnStmt(in, line);
		break;
	case IF:
		progress = IfStmt(in, line);
		break;
	case IDENT:
		if (!checkVar(t.GetLexeme(), line))
		{
			ParseError(line, "Variable exists");
			return false;
		}
		Parser::PushBackToken(t);
		progress = AssignStmt(in, line);
		break;
	default:
		Parser::PushBackToken(t);
		return false;
	}
	return progress;
}

bool WriteLnStmt(istream &in, int &line)
{
	LexItem t;
	
	ValQue = new queue<Value>;
	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	cout << endl;

	return ex;
} 
bool IfStmt(istream &in, int &line)
{
	Value firstValue;
	LexItem tk = Parser::GetNextToken(in, line);
	if (tk != LPAREN)
	{
		ParseError(line, "Left Paranthesis Missing");
		return false;
	}

	bool ex = LogicExpr(in, line, firstValue);
	if (!ex)
	{
		ParseError(line, "Logical Error");
		return false;
	}
	tk = Parser::GetNextToken(in, line);

	if (tk != RPAREN)
	{
		ParseError(line, "Right Paranthesis Missing");
		return false;
	}

	tk = Parser::GetNextToken(in, line);
	if (tk != THEN)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	bool stmt;
	if (firstValue.GetBool())
	{
		stmt = Stmt(in, line);
		if (!stmt)
		{
			ParseError(line, "Error in Statement");
			return false;
		}
		while(tk != SEMICOL){
			tk = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(tk);
	}
	else
	{
		while (tk != ELSE)
		{
			tk = Parser::GetNextToken(in, line);
		}
		Parser::PushBackToken(tk);

		tk = Parser::GetNextToken(in, line);
		if (tk != ELSE)
		{
			Parser::PushBackToken(tk);
		}
		else
		{
			stmt = Stmt(in, line);
			if (!stmt)
			{
				ParseError(line, "Error in Statement");
				return false;
			}
		}
	}

	return true;
}
bool AssignStmt(istream &in, int &line)
{
	LexItem tk = Parser::GetNextToken(in, line);
	string varname = tk.GetLexeme();
	Value firstValue = Value();// setValue(tk, line);

	tk = Parser::GetNextToken(in, line);
	if (tk != ASSOP)
	{
		ParseError(line, "Missing Assignment Operator");
		return false;
	}

	bool ex = Expr(in, line, firstValue);
	if (!ex)
	{
		ParseError(line, "Invalid expression");
		return false;
	}
	// cout <<"Assign " << firstValue << tk << endl;
	if (!updateValues(varname, firstValue, line)){
		ParseError(line, "Illegal Assignment Operation");
		return false;
	}

	return true;
}
bool Var(istream &in, int &line, LexItem &idtok)
{
	LexItem tk = Parser::GetNextToken(in, line);
	if (tk != IDENT)
	{
		ParseError(line, "Ident not found");
		return false;
	}
	if (!checkVar(tk.GetLexeme(), line))
	{
		ParseError(line, "Wrong expression");
		return false;
	}
	return true;
}
bool ExprList(istream &in, int &line)
{
	Value firstValue;
	bool expr = Expr(in, line, firstValue);
	if (!expr)
	{
		ParseError(line, "Wrong expression");
		return false;
	}

	ValQue->push(firstValue);

	LexItem tk = Parser::GetNextToken(in, line);
	while (tk == COMMA)
	{

		expr = Expr(in, line, firstValue);
		if (!expr)
		{
			ParseError(line, "Wrong expression");
			return false;
		}
		ValQue->push(firstValue);
		tk = Parser::GetNextToken(in, line);
	}

	Parser::PushBackToken(tk);
	return true;
}
bool LogicExpr(istream &in, int &line, Value &retVal)
{
	Value firstValue, secondValue;
	bool exp = Expr(in, line, firstValue);

	if (!exp)
	{
		ParseError(line, "Missing expression after relational operator");
		return false;
	}

	LexItem tk = Parser::GetNextToken(in, line);
	// cout << "logic expression " << tk  << " " << tk.GetLexeme() << endl;

	if (!(tk == EQUAL || tk == LTHAN || tk == GTHAN))
	{
		ParseError(line, "symbol not found");
		return false;
	}

	exp = Expr(in, line, secondValue);

	if (tk == EQUAL) retVal = firstValue == secondValue;
	else if (tk == LTHAN) retVal = firstValue < secondValue;
	else if (tk == GTHAN) retVal = firstValue > secondValue;

	

	if (retVal.IsErr()){
		ParseError(line, "Run-Time Error-Illegal Mixed Type Operands for a Logic Expression");
		return false;
	}

	if (!exp)
	{
		ParseError(line, "Invalid Expression");
		return false;
	}

	return true;
}

bool Expr(istream &in, int &line, Value &retVal)
{
	Value firstValue, secondValue;
	
	bool firstToken = Term(in, line, firstValue);
	LexItem tok;

	if (!firstToken)
	{
		return false;
	}
	retVal = firstValue;
	

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	while (tok == PLUS || tok == MINUS)
	{
		firstToken = Term(in, line, secondValue);
		if (!firstToken)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == PLUS)
		{
			retVal = retVal + secondValue;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal Addition operation.");
				// cout << "(" << tok.GetLexeme() << ")" << endl;
				return false;
			}
		}
		else if (tok == MINUS)
		{
			retVal = retVal - secondValue;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal Subtraction operation.");
				return false;
			}
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
bool Term(istream &in, int &line, Value &retVal)
{

	bool lparen = false;
	LexItem tk = Parser::GetNextToken(in, line);
	
	if (tk == LPAREN)
	{
		
		lparen = true;
	}
	else{
		Parser::PushBackToken(tk);
	}
	Value firstValue, secondValue;
	bool binarySF = SFactor(in, line, firstValue);
	if (!binarySF)
	{
		
		return false;
	}

	retVal = firstValue;

	tk = Parser::GetNextToken(in, line);
	while ((tk == DIV) || (tk == MULT))
	{
		binarySF = SFactor(in, line, secondValue);
		if (!binarySF)
		{
			
			return false;
		}
		if (tk == DIV)
		{
			
			if (secondValue.IsReal()) if (abs(secondValue.GetReal()) < 1e-14){

				ParseError(line, "Run-Time Error-Illegal Division by Zero.");
				
				return false;
			}
			if (secondValue.IsInt()) if (secondValue.GetInt() == 0){

				ParseError(line, "Run-Time Error-Illegal Division by Zero.");
				
				return false;
			}
			retVal = firstValue / secondValue;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal division operation.");
				
				return false;
			}
		}
		else if (tk == MULT)
		{
			retVal = firstValue * secondValue;
			if (retVal.IsErr())
			{
				ParseError(line, "Illegal multiplication operation.");
				
				return false;
			}
		}
		tk = Parser::GetNextToken(in, line);
	}

	Parser::PushBackToken(tk);
	
	if (lparen)
	{
		tk = Parser::GetNextToken(in, line);
		if (tk != RPAREN)
		{
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}

	return true;
}
bool SFactor(istream &in, int &line, Value &retVal)
{
	bool lparen = false;
	int symbol = 0;
	LexItem tk = Parser::GetNextToken(in, line);
	
	if (tk == LPAREN)
	{
		tk = Parser::GetNextToken(in, line);
		lparen = true;
	}
	if (!(tk == PLUS || tk == MINUS))
	{
		Parser::PushBackToken(tk);
	}
	else
	{
		if (tk == MINUS)
		{
			symbol = -1;
		}
		else if (tk == PLUS){
			symbol = 1;
		}
	}

	if (tk == EQUAL || tk == MULT || tk == DIV)
	{
		
		return false;
	}

	bool fac = Factor(in, line, symbol, retVal);
	if (!fac)
	{
		
		return false;
	}

	if (lparen)
	{
		tk = Parser::GetNextToken(in, line);
		if (tk != RPAREN)
		{
			ParseError(line, "Missing ) after expression");
			return false;
		}
	}

	return true;
}
bool Factor(istream &in, int &line, int symbol, Value &retVal)
{
	LexItem tk = Parser::GetNextToken(in, line);
	if (!(tk == IDENT || tk == ICONST || tk == RCONST || tk == SCONST))
	{
		Parser::PushBackToken(tk);
		bool expr = Expr(in, line, retVal);
		if (!expr)
		{
			ParseError(line, "Invalid Expression");
			return false;
		}
	}
	else
	{
		Value firstValue = ConstValue(tk, line);
		
		if (firstValue.IsErr()){
			ParseError(line, "Undefined Variable");
			return false;
		}
		OperationofUnary(symbol, firstValue);
		if (firstValue.IsErr()){
			ParseError(line, "Illegal Operand Type for Sign Operator");
			return false;
		}
		retVal = firstValue;
		
	}
	if (tk == IDENT)
	{
		if (!checkVar(tk.GetLexeme(), line))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
	}

	return true;
}