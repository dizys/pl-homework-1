%{
#include <iostream>

int yylex();
int yyerror(const char *p) { std::cerr << "error: " << p << std::endl; };
%}

%start escape_hell

%type <val> escape_hell double_quoted single_quoted

%%

escape_hell: 
    double_quoted 
  | 
    single_quoted
  ;

double_quoted: 
    "hello" 
  | 
    "So \"easy\"!"
  |
    "line1\nline2"
  |
    "C:\\Users\\Documents"
  ;

single_quoted: '{' | '\n' | '\\';

%%

int main()
{
    yyparse(); // A parsing function that will be generated by Bison.
    return 0;
}
