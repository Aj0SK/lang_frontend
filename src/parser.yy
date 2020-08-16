%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.4.1"
%defines

%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires {
  # include <iomanip>
  class driver;
  class NodeAST;
  class ProgramAST;
  class ExprAST;
  class NumberExprAST;
  class VariableExprAST;
  class BinaryExprAST;
  class CallExprAST;
  class PrototypeAST;
  class FunctionAST;
}

// The parsing context.
%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hh"
}

%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  MINUS   "-"
  PLUS    "+"
  STAR    "*"
  SLASH   "/"
  LPAREN  "("
  RPAREN  ")"
  LBRACE  "{"
  RBRACE  "}"
  COMMA   ","
  EQUAL   "="
  DEF    "def"
  EXTERN "extern"
;

%token <std::string> IDENTIFIER "identifier"
%token <double> FLOATING "floating"
%nterm <std::unique_ptr<ProgramAST> > program
%nterm <std::unique_ptr<ProgramAST> > stmts
%nterm <std::unique_ptr<NodeAST> > stmt
%nterm <std::unique_ptr<ExprAST> > expr
%nterm <std::vector<std::unique_ptr<ExprAST> > > call_args
%nterm <std::unique_ptr<FunctionAST> > definition
%nterm <std::unique_ptr<PrototypeAST> > prototype
%nterm <std::unique_ptr<PrototypeAST> > external
%nterm <std::vector<std::string> > proto_args
%nterm <char> operator

%%
%start program;

%left "+" "-";
%left "*" "/";

program : stmts {}
        ;

stmts : stmt {}
      | stmts stmt {}
      ;

stmt : expr { drv.result->statements.push_back(std::move($1)); }
     | definition { drv.result->statements.push_back(std::move($1)); }
     | external { drv.result->statements.push_back(std::move($1)); }
     ;

expr : "floating" { $$ = std::make_unique<NumberExprAST> ($1); }
     | "identifier" { $$ = std::make_unique<VariableExprAST>($1); }
     | expr operator expr { $$ = std::make_unique<BinaryExprAST>($2, std::move($1), std::move($3)); } 
     | "identifier" "(" call_args ")" { $$ = std::make_unique<CallExprAST> ($1, std::move($3));}
     ;

operator: PLUS { $$ = '+'; }
        | MINUS { $$ = '-'; }
        | STAR { $$ = '*'; }
        | SLASH { $$ = '/'; }
        ;

definition: "def" prototype LBRACE expr RBRACE { $$ = std::make_unique<FunctionAST> (std::move($2), std::move($4)); }
          ;

prototype: IDENTIFIER LPAREN proto_args RPAREN { $$ = std::make_unique<PrototypeAST> (std::move($1), std::move($3)); }
         ;

external: "extern" prototype { $$ = std::move($2); }
        ;
         
proto_args : /*blank*/  { }
          | IDENTIFIER { $$.push_back($1); }
          | proto_args COMMA IDENTIFIER  { $1.push_back($3); }
          ;

call_args : /*blank*/  { }
          | expr { $$.push_back(std::move($1)); }
          | call_args COMMA expr  { $1.push_back(std::move($3)); }
          ;

%%

void
yy::parser::error (const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}
