#include "treenode.h"
#include <iostream>
#include <set>
#include <vector>

using namespace std;

namespace asttree {
set<string> treenode::termAndNontermSet;
vector<string> treenode::extraProductionVector;

string treenode::getNewUnTakenNonterm(const string &prefix) {
  if (treenode::termAndNontermSet.find(prefix) ==
      treenode::termAndNontermSet.end()) {
    treenode::termAndNontermSet.insert(prefix);
    return prefix;
  }

  int i = 1;

  while (true) {
    string newNonterm = prefix + "_" + to_string(i);
    if (treenode::termAndNontermSet.find(newNonterm) ==
        treenode::termAndNontermSet.end()) {
      treenode::termAndNontermSet.insert(newNonterm);
      return newNonterm;
    }
    i++;
  }
}

void prog::doConversion() {
  beforeConversion();
  statements *ss = (statements *)getChild(1);
  ss->doConversion();
  convertedSection = "\n%%\n\n" + ss->productions + "%%\n";
}

void statements::doConversion() {
  beforeConversion();
  statement *s_node = (statement *)getChild(0);
  s_node->doConversion();
  if (numChildren == 1) {
    productions = s_node->productions;
  } else if (numChildren == 2) {
    statements *ss_node = (statements *)getChild(1);
    ss_node->doConversion();
    productions += s_node->productions + ss_node->productions;
  }
}

statement::statement(treenode *t1, treenode *t2, treenode *ete, treenode *t3)
    : treenode(t1, t2, ete, t3) {
  parentRewrite = ((terminal *)t1)->value();
}

void statement::doConversion() {
  beforeConversion();
  terminal *t1_node = (terminal *)getChild(0);
  epsilon_trans_expr *ete_node = (epsilon_trans_expr *)getChild(2);
  ete_node->doConversion();
  productions = t1_node->value() + NAME_RULE_SPLITTER +
                ete_node->termOrNonterm + PRODUCTION_END_SEMI;
  for (vector<string>::reverse_iterator it = extraProductionVector.rbegin();
       it != extraProductionVector.rend(); it++) {
    productions += *it + PRODUCTION_END_SEMI;
  }
  extraProductionVector.clear();
}

void epsilon_trans_expr::doConversion() {
  beforeConversion();
  if (numChildren == 1) {
    expr *e_node = (expr *)getChild(0);
    e_node->doConversion();
    termOrNonterm = e_node->termOrNonterm;
  } else if (numChildren == 3) {
    expr *e_node = (expr *)getChild(2);
    e_node->doConversion();
    termOrNonterm = EPSILON_TRANS + RULE_OR_SPLITTER + e_node->termOrNonterm;
  }
}

void expr::doConversion() {
  beforeConversion();
  subexpr *se_node = (subexpr *)getChild(0);
  se_node->doConversion();
  if (numChildren == 1) {
    termOrNonterm = se_node->termOrNonterm;
  } else if (numChildren == 3) {
    expr *e_node = (expr *)getChild(2);
    e_node->doConversion();
    insertSelfRecursive(se_node->termOrNonterm);
    if (!e_node->termOrNonterm.empty()) {
      insertSelfRecursive(e_node->termOrNonterm);
    } else {
      for (vector<string>::iterator it = e_node->selfRecursiveVector.begin();
           it != e_node->selfRecursiveVector.end(); it++) {
        insertSelfRecursive(*it);
      }
    }
    treenode *parent_node = getParent();
    if (parent_node->getType() != "expr") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_expr");
      string productionRule;
      for (vector<string>::iterator it = selfRecursiveVector.begin();
           it != selfRecursiveVector.end(); it++) {
        if (it != selfRecursiveVector.begin()) {
          productionRule += RULE_OR_SPLITTER;
        }
        productionRule += *it;
      }
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}

void subexpr::doConversion() {
  beforeConversion();
  concat *c_node = (concat *)getChild(0);
  c_node->doConversion();
  if (numChildren == 1) {
    termOrNonterm = c_node->termOrNonterm;
  } else if (numChildren == 3) {
    subexpr *se_node = (subexpr *)getChild(2);
    se_node->doConversion();
    insertSelfRecursive(c_node->termOrNonterm);
    if (!se_node->termOrNonterm.empty()) {
      insertSelfRecursive(se_node->termOrNonterm);
    } else {
      for (vector<string>::iterator it = se_node->selfRecursiveVector.begin();
           it != se_node->selfRecursiveVector.end(); it++) {
        insertSelfRecursive(*it);
      }
    }
    treenode *parent_node = getParent();
    if (parent_node->getType() != "subexpr") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_subexpr");
      string productionRule;
      for (vector<string>::iterator it = selfRecursiveVector.begin();
           it != selfRecursiveVector.end(); it++) {
        if (it != selfRecursiveVector.begin()) {
          productionRule += RULE_OR_SPLITTER;
        }
        productionRule += *it;
      }
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}

void concat::doConversion() {
  beforeConversion();
  quant *q_node = (quant *)getChild(0);
  q_node->doConversion();
  if (numChildren == 1) {
    termOrNonterm = q_node->termOrNonterm;
  } else if (numChildren == 2) {
    concat *c_node = (concat *)getChild(1);
    c_node->doConversion();
    treenode *parent_node = getParent();
    if (parent_node->getType() == "concat") {
      termOrNonterm = q_node->termOrNonterm + " " + c_node->termOrNonterm;
    } else {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_concat");
      string productionRule =
          q_node->termOrNonterm + " " + c_node->termOrNonterm;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}

void quant::doConversion() {
  beforeConversion();
  if (numChildren == 1) {
    base *bs_node = (base *)getChild(0);
    bs_node->doConversion();
    termOrNonterm = bs_node->termOrNonterm;
  } else if (numChildren == 2) {
    base *bs_node = (base *)getChild(0);
    bs_node->doConversion();
    terminal *t2_node = (terminal *)getChild(1);
    string bs_name = bs_node->termOrNonterm;
    if (t2_node->value() == "+") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_quant");
      string productionRule =
          bs_name + RULE_OR_SPLITTER + bs_name + " " + productionName;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    } else if (t2_node->value() == "*") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_quant");
      string productionRule =
          EPSILON_TRANS + RULE_OR_SPLITTER + bs_name + " " + productionName;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  } else if (numChildren == 3) {
    terminal *t1_node = (terminal *)getChild(0);
    base *bs_node = (base *)getChild(1);
    bs_node->doConversion();
    string bs_name = bs_node->termOrNonterm;
    if (t1_node->value() == "<") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_quant");
      string productionRule =
          EPSILON_TRANS + RULE_OR_SPLITTER + bs_name + " " + productionName;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    } else if (t1_node->value() == "[") {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_quant");
      string productionRule = EPSILON_TRANS + RULE_OR_SPLITTER + bs_name;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}

void base::doConversion() {
  beforeConversion();
  if (numChildren == 1) {
    term_and_nonterms *nt_node = (term_and_nonterms *)getChild(0);
    nt_node->doConversion();
    termOrNonterm = nt_node->termOrNonterm;
  } else if (numChildren == 3) {
    subexpr *se_node = (subexpr *)getChild(1);
    se_node->doConversion();
    termOrNonterm = se_node->termOrNonterm;
  }
}

term_and_nonterms::term_and_nonterms(treenode *t) : treenode(t) {
  string t_val = ((terminal *)t)->value();
  treenode::termAndNontermSet.insert(t_val);
}

term_and_nonterms::term_and_nonterms(treenode *t, treenode *tn)
    : treenode(t, tn) {
  string t_val = ((terminal *)t)->value();
  treenode::termAndNontermSet.insert(t_val);
}

void term_and_nonterms::doConversion() {
  beforeConversion();
  terminal *t1_node = (terminal *)getChild(0);
  string t1_val = t1_node->value();
  if (numChildren == 1) {
    termOrNonterm = t1_val;
  } else if (numChildren == 2) {
    term_and_nonterms *nt2_node = (term_and_nonterms *)getChild(1);
    nt2_node->doConversion();
    treenode *parent_node = getParent();
    if (parent_node->getType() == "term_and_nonterms") {
      termOrNonterm = t1_val + " " + nt2_node->termOrNonterm;
    } else {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_nonterm");
      string productionRule = t1_val + " " + nt2_node->termOrNonterm;
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}
} // namespace asttree
