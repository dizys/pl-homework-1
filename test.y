%{
#include <iostream>

int yylex(); // A function that is to be generated and provided by flex,
             // which returns a next token when called repeatedly.
int yyerror(const char *p) { std::cerr << "error: " << p << std::endl; };
%}

%union {
    int val;
    /* You may include additional fields as you want. */
    /* char op; */
};

%start prog

%token LPAREN RPAREN
%token PLUS MINUS MUL DIV
%token <val> NUM    /* 'val' is the (only) field declared in %union
                       which represents the type of the token. */

%type <val> expr term factor
                                    