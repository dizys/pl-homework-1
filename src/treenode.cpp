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

void quant::doConversion() {
  beforeConversion();
  if (numChildren == 1) {
    base *bs_node = (base *)getChild(0);
    bs_node->setParent(this);
    bs_node->doConversion();
    termOrNonterm = bs_node->termOrNonterm;
  } else if (numChildren == 2) {
    base *bs_node = (base *)getChild(0);
    bs_node->setParent(this);
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
    bs_node->setParent(this);
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
    subexpr *se_node = (subexpr *)getChild(0);
    se_node->doConversion();
    string productionName =
        treenode::getNewUnTakenNonterm(parentRewrite + "_base");
    insertExtraProduction(productionName, se_node->termOrNonterm);
    termOrNonterm = productionName;
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
