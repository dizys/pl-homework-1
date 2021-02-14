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

statement::statement(treenode *t1, treenode *t2, treenode *ete, treenode *t3)
    : treenode(t1, t2, ete, t3) {
  parentRewrite = ((terminal *)t1)->value();
}

void statements::doConversion() {
  beforeConversion();
  if (numChildren == 1) {
  }
}

void statement::doConversion() {
  beforeConversion();
  terminal *t1_node = (terminal *)getChild(0);
  epsilon_trans_expr *ete_node = (epsilon_trans_expr *)getChild(2);
  ete_node->doConversion();
  productions = t1_node->value() + NAME_RULE_SPLITTER +
                ete_node->termOrNonterm + PRODUCTION_END_SEMI;
  while (!extraProductionVector.empty()) {
    productions += "\n" + *extraProductionVector.end() + PRODUCTION_END_SEMI;
    extraProductionVector.pop_back();
  }
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
    treenode *parent_node = getParent();
    if (parent_node->getType() == "expr") {
      expr *parent_expr = (expr *)parent_node;
      parent_expr->insertSelfRecursive(se_node->termOrNonterm);
      if (!e_node->termOrNonterm.empty()) {
        parent_expr->insertSelfRecursive(e_node->termOrNonterm);
      } else {
        for (vector<string>::iterator it = selfRecursiveVector.begin();
             it != selfRecursiveVector.end(); it++) {
          parent_expr->insertSelfRecursive(*it);
        }
      }
    } else {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_expr");
      string productionRule = se_node->termOrNonterm;
      for (vector<string>::iterator it = selfRecursiveVector.begin();
           it != selfRecursiveVector.end(); it++) {
        productionRule += RULE_OR_SPLITTER + *it;
      }
      insertExtraProduction(productionName, productionRule);
      termOrNonterm = productionName;
    }
  }
}

void subexpr::doConversion() {
  beforeConversion();
  quant *q_node = (quant *)getChild(0);
  q_node->doConversion();
  if (numChildren == 1) {
    termOrNonterm = q_node->termOrNonterm;
  } else if (numChildren == 3) {
    subexpr *se_node = (subexpr *)getChild(2);
    se_node->doConversion();
    treenode *parent_node = getParent();
    if (parent_node->getType() == "subexpr") {
      subexpr *parent_subexpr = (subexpr *)parent_node;
      parent_subexpr->insertSelfRecursive(q_node->termOrNonterm);
      if (!se_node->termOrNonterm.empty()) {
        parent_subexpr->insertSelfRecursive(se_node->termOrNonterm);
      } else {
        for (vector<string>::iterator it = selfRecursiveVector.begin();
             it != selfRecursiveVector.end(); it++) {
          parent_subexpr->insertSelfRecursive(*it);
        }
      }
    } else {
      string productionName =
          treenode::getNewUnTakenNonterm(parentRewrite + "_subexpr");
      string productionRule = q_node->termOrNonterm;
      for (vector<string>::iterator it = selfRecursiveVector.begin();
           it != selfRecursiveVector.end(); it++) {
        productionRule += RULE_OR_SPLITTER + *it;
      }
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
      string subProductionName =
          treenode::getNewUnTakenNonterm(productionName + "_sub");
      string productionRule =
          bs_name + RULE_OR_SPLITTER + bs_name + " " + subProductionName;
      string subProductionRule =
          bs_name + RULE_OR_SPLITTER + bs_name + " " + subProductionName;
      insertExtraProduction(productionName, productionRule);
      insertExtraProduction(subProductionName, subProductionRule);
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
    string nt2_val = nt2_node->termOrNonterm;
    termOrNonterm = t1_val + " " + nt2_val;
  }
}
} // namespace asttree
