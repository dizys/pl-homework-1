%{
#include <iostream>
#include <stdlib.h>
#include <vector>

/* needs to come before trans.tab.h */
#include "treenode.h"
using namespace asttree;

#include "trans.tab.h" /* We want the tokens defined in trans.y. */

using namespace std;

/* section variable refers to:
  1 = definitions
  2 = rules
  3 = user code

  I recommend that you echo everything in sections 1 and 3 to the standard output
  and only return tokens for section 2, since the parser will be mainly concerned
  with the rules.
 */
int section=1;

/* Flag gets set to 1 immediately when the rewrite symbol ':' is observed.
  When 1, we should check for a newline.  If so, that means this is
  an epsilon transition.  In this case we should return an epsilon token
  rather than interpret it as a newline.
*/
int flag=0;

int countLines(const string &text) {
  string needle="\n";
  int nPos = text.find(needle, 0);
  int count = 1;
  while (nPos != string::npos)
  {
    count++;
    nPos = text.find(needle, nPos + needle.size());
  }
  return count;
}
%}

%option noyywrap

%%

"/*" {
  /* skip multiline comments */
  if (section != 2) {
    cout << yytext;
  }
  int c;
  while ((c = yyinput()) != 0) {
    if (section != 2) {
      cout << (char)c;
    }
    if (c == '\n') {
      ++yylineno; /* increment the line number */
    } else if (c == '*') {
      if ((c = yyinput()) == '/') {
        if (section != 2) {
          cout << (char)c;
        }
        break;
      }
      unput(c); /* we did not see the /.  Oh well, put it back. */
    }
  }
}

"<" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return REP_ZERO_LEFT;
  } else {
    cout << yytext;
  }
}

">" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return REP_ZERO_RIGHT;
  } else {
    cout << yytext;
  }
}

 /* RegEx for terminals and non-terminals goes here */
[a-zA-Z][a-zA-Z0-9_]+|'[a-zA-Z][a-zA-Z0-9_]+' {
  yylval.val = new terminal(yytext);
  if (section != 2) {
    cout << yytext;
  }
  flag=0; /* if we saw a terminal or non-terminal, the line cannot be empty */
  if (section == 2) {
    return TERM_OR_NONTERM;
  }
}

"[" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return OPTION_LEFT;
  } else {
    cout << yytext;
  }
}

"]" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return OPTION_RIGHT;
  } else {
    cout << yytext;
  }
}

"(" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return GROUPING_LEFT;
  } else {
    cout << yytext;
  }
}

")" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return GROUPING_RIGHT;
  } else {
    cout << yytext;
  }
}

"+" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return REP_ONE;
  } else {
    cout << yytext;
  }
}

"*" {
  yylval.val = new terminal(yytext);
  if (section==2) {
    return REP_ZERO;
  } else {
    cout << yytext;
  }
}

"||" {
  yylval.val = new terminal("|");
  if (section==2) {
    return GROUP_OR;
  } else {
    cout << yytext;
  }
}

"|" {
  yylval.val = new terminal("\n  |");
  if (section==2) {
    return RULE_OR;
  } else {
    cout << yytext;
  }
}

":" {
  if (section==2) {
    flag=1; /* line empty if newline afer this */
    yylval.val = new terminal(yytext);
    return REWRITES;
  } else {
    cout << yytext;
  }
}

";" {
  yylval.val = new terminal("\n   ;");
  if (section==2) {
    return SEMI;
  } else {
    cout << yytext;
  }
}

 /* Section divider */
"%%" {
  /* section divider */
  yylval.val = new terminal("\n%%\n");
  section++;
  return SECTION;
}


 /* Code block. %{ ...  %}
    Warning: possibly multiple lines!  */
"%{"[.\n]*"%}" {
  yylineno+=countLines(yytext)-1;
  /* discard flex code block */
  cout << yytext;
}


 /* This is the %union directive. Also can span multiple lines. */
"%union {"[^}]*"};" {
  yylineno+=countLines(yytext)-1;
  /* discard union code block */
  cout << yytext;
}

 /* Code action block of the form { .... } */
"{"[^}]*"}" {
  yylineno+=countLines(yytext)-1;
  /* discard action code block */
  if (section != 2) {
    cout << yytext;
  }
}

[\n\r]+ {
  yylineno+=countLines(yytext)-1;
  if (section!=2) {
    cout << yytext;
  }
  if (flag) {
    flag=0;
    yylval.val = new empty();
    return ENDL;
  }
}

[ \t]+ {
  /* Ignore white characters. */
  cout << yytext;
}

. {
  if (section != 2) {
    cout << yytext;
  } else {
    cerr << "Invalid token: " << yytext <<  " on line " << yylineno << endl;
    exit(1);
  }
}

%%
