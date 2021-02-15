%{
#include <iostream>
#include <stdlib.h>

#include "tinybasic.tab.h"

using namespace std;
%}

%option noyywrap
%option yylineno

%%

"PRINT" {
  return PRINT;
}

"IF" {
  return IF;
}

(<[>=]?)|(>[<=]?)|= {
  return RELOP;
}

"THEN" {
  return THEN;
}

"GOTO" {
  return GOTO;
}

"INPUT" {
  return INPUT;
}

"LET" {
  return LET;
}

"GOSUB" {
  return GOSUB;
}

"RETURN" {
  return RETURN;
}

"CLEAR" {
  return CLEAR;
}

"LIST" {
  return LIST;
}

"RUN" {
  return RUN;
}

"END" {
  return END;
}

\"[^\"]*[^\\\"]\"|\"[^\\\"]*(\\.[^\\\"]*)+\" {
  return STRING;
}

"," {
  return COMMA;
}

[\+\-] {
  return PLUMIN;
}

[\*/] {
  return MULDIV;
}

[A-Z] {
  return VAR;
}

"(" {
  return LPAREN;
}

")" {
  return RPAREN;
}

\d {
  return DIGIT;
}

[\n\r]+ {
  return CR;
}

[ \t]+ {
  /* Ignore white characters. */
}

. {
  cerr << "Error: invalid token `" << yytext <<  "` on line " << yylineno << endl;
  exit(1);
}

%%
