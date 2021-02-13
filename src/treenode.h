#ifndef TREENODE_H
#define TREENODE_H

#include <iostream>
#include <string>
#include <set>
#include <vector>

using namespace std;

#define MAX_RULENAME 25
#define MAX_CHILD 25
#define NULL_STRING_VAL "NULL"
#define EMPTY_VAL " /* empty */"

namespace asttree {
  class treenode;
  class prog;
  class statements;
  class statement;
  class epsilon_trans_expr;
  class empty;
  class expr;
  class subexpr;
  class quant;
  class base;
  class term_and_nonterms;
  class terminal;

  /*************************************************************
  class  treenode

  Represents a node in the abstract syntax tree. The constructor
  overloads correspond to the number of items on the right-hand-side
  of rules in your Bison grammar, regardless of whether they
  are terminals or non-terminals.  We don't construct instances
  of this class directly---use the subclasses instead.

  *************************************************************/

  class treenode {
    treenode* children[MAX_CHILD];
    treenode* parent;
  protected:
    int numChildren=0;
  public:
    static set<string>* termAndNontermSet;
    // static vector<string>* extraProductionVector;
    static string getNewUnTakenNonterm(const string& prefix);
    string parentRewrite;
    string termOrNonterm = "";

    treenode() : numChildren(0), parent(NULL) {
      children[0] = NULL;
    }

    treenode(treenode* t1) : numChildren(1), parent(NULL) {
      children[0] = t1;
    }

    treenode(treenode* t1, treenode* t2) : numChildren(2), parent(NULL) {
      children[0] = t1;
      children[1] = t2;
    }


    treenode(treenode* t1, treenode* t2, treenode* t3) : numChildren(3), parent(NULL) {
      children[0] = t1;
      children[1] = t2;
      children[2] = t3;
    }

    treenode(treenode* t1, treenode* t2, treenode* t3, treenode* t4) : numChildren(4), parent(NULL) {
      children[0] = t1;
      children[1] = t2;
      children[2] = t3;
      children[3] = t4;
    }

    void setParent(treenode* p) {
      if (!(p->parentRewrite).empty()) {
        parentRewrite = p->parentRewrite;
      }
      parent = p;
    }

    treenode* getParent() const {
      return parent;
    }

    void replace(treenode* t, treenode* with) {
      for (int i=0; i < numChildren; i++) {
        if (children[i] != NULL && children[i] == t) {
          children[i] = with;
          return;
        }
      }
    }

    virtual void addChild(treenode* t) {
      children[numChildren++] = t;
    }

    treenode* getChild(int c) {
      if (c >= MAX_CHILD || c >= numChildren) {
        return NULL;
      }

      return children[c];
    }

    void setChild(int c, treenode* t) {
      children[c]=t;
    }

    virtual string value() const {
      return NULL;
    }

    virtual string setValue(string s) {}

    virtual void print(ostream& o) const {
      for (int i=0; i < numChildren; i++) {
        if (children[i] != NULL) {
          children[i]->print(o);
        }
      }
    }

    virtual void beforeConversion() {
      for (int i=0; i < numChildren; i++) {
        if (children[i] != NULL) {
          children[i]->setParent(this);
        }
      }
    }

    virtual void doConversion() {
      beforeConversion();
      for (int i=0; i < numChildren; i++) {
        if (children[i] != NULL) {
          children[i]->doConversion();
        }
      }
      afterConversion();
    }

    virtual void afterConversion() {
      for (int i=0; i < numChildren; i++) {
        if (children[i] != NULL) {
          // vector<string> childProductions = children[i]->
        }
      }
    }
  };

  /* Subclasses should be named after the left-hand-side of each
    rule in your Bison grammar.  Some limited classes are supplied
    to you.
  */

  class prog : public treenode {
  public:
    prog(treenode* t1, treenode* ss, treenode* t2) : treenode(t1, ss, t2) {}
  };

  class statements: public treenode {
  public:
    statements(treenode* s) : treenode(s) {}
    statements(treenode* s, treenode* ss) : treenode(s, ss) {}
  };

  class statement : public treenode {
  public:
    statement(treenode* t1, treenode* t2, treenode* ete, treenode* t3);

    virtual void print(ostream& o) const {
      treenode::print(o);
      o << endl << endl;
    }
  };

  class epsilon_trans_expr : public treenode {
  public:
    epsilon_trans_expr(treenode* t1, treenode* t2, treenode* e) : treenode(t1, t2, e) {}
    epsilon_trans_expr(treenode* e) : treenode(e) {}
  };

  class empty : public treenode {
  };

  class expr : public treenode {
  public:
    expr(treenode* se) : treenode(se) {}
    expr(treenode* se, treenode* t, treenode* e) : treenode(se, t, e) {}
  };

  class subexpr : public treenode {
  public:
    subexpr(treenode* q) : treenode(q) {}
    subexpr(treenode* q, treenode* t, treenode* se) : treenode(q, t, se) {}
  };

  class quant : public treenode {
  public:
    quant(treenode* b) : treenode(b) {}
    quant(treenode* b, treenode* t) : treenode(b, t) {}
    quant(treenode* t1, treenode* b, treenode* t2) : treenode(t1, b, t2) {}
    void doConversion();
  };

  class base : public treenode {
  public:
    base(treenode* t1, treenode* se, treenode* t2) : treenode(t1, se, t2) {}
    base(treenode* tn) : treenode(tn) {}
    void doConversion();
  };

  class term_and_nonterms : public treenode {
  public:
    term_and_nonterms(treenode* t);
    term_and_nonterms(treenode* t, treenode* tn);
    void doConversion();
  };

  class terminal : public treenode {
  string s;
  public:
    terminal(string v) : s(v) {}

    virtual string setValue(string s1) {
      s = s1;
    }

    virtual string value() const {
      return s;
    }

    virtual void print(ostream& o) const {
      o << s << " ";
    }
  };
}

#endif
