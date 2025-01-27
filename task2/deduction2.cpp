#include "deduction.h"
#include <iostream>
using namespace std;

bool deduction::good(int x) {
    return x >= 0 && x < (int) s.length();
}

std::string deduction::trimWhiteSpace(std::string const& s) {
    std::string temp;
    for (auto c : s)
        if (!isspace(c))
            temp.push_back(c);
    return temp;
}

bool deduction::isContext() {
    return curAxiom >= 10;
}

void deduction::itIsAxiom(parser::linkOnTree vertex, parser::linkOnTree axiom) {
    if (!axiom || !vertex) {
        if (axiom || vertex) {
            f = false;
        } 
        return;
    }

    if (goodCharForVar(axiom->str[0])) {
        if (isContext() || axiomToHash.count(axiom->str)) {
            if ((!isContext() && (axiomToHash[axiom->str] != vertex->hash)) ||
                 (isContext() && (axiom->str != vertex->str))) {
                f = false;
            }
        } else if (!isContext()){
            axiomToHash[axiom->str] = vertex->hash;
        }
    } else if (axiom->str == vertex->str) {
        itIsAxiom(vertex->left, axiom->left); 
        itIsAxiom(vertex->right, axiom->right); 
    } else {
        f = false;
        return;
    }
}

int deduction::isAxiom(parser::linkOnTree vertex) {
    for (curAxiom = 0; curAxiom < (int) axioms.size(); curAxiom++)  {
        axiomToHash.clear();
        f = true;
        itIsAxiom(vertex, axioms[curAxiom]);
        if (f) {
            return curAxiom + 1;
        }
    }
    return 0;
}

void deduction::output(std::string const& s1, std::string const& s2, std::string const& s3 = "") {
    result[r_sz] = "(" + s1 + ")" + "->(" + s2 + ")";
    if (s3 != "") 
        result[r_sz] = "->(" + s1 + ")";
}

deduction::deduction() {
    axioms.push_back(main_parser.parse("A->B->A"));
    axioms.push_back(main_parser.parse("(A->B)->(A->B->C)->(A->C)"));
    axioms.push_back(main_parser.parse("A->B->A&B"));
    axioms.push_back(main_parser.parse("A&B->A"));
    axioms.push_back(main_parser.parse("A&B->B"));
    axioms.push_back(main_parser.parse("A->A|B"));
    axioms.push_back(main_parser.parse("B->A|B"));
    axioms.push_back(main_parser.parse("(A->C)->(B->C)->(A|B->C)"));
    axioms.push_back(main_parser.parse("(A->B)->(A->!B)->!A"));
    axioms.push_back(main_parser.parse("!!A->A"));
}

deduction::~deduction() {
    for (int i = 0; i < (int) axioms.size(); i++) {
        delete axioms[i];
    }

    for (int i = 0; i < (int) forest.size(); i++) {
        delete forest[i];
    }
}

void deduction::doDeduction(std::vector<std::string> const& expr) { 
    std::vector<std::string> history;
    std::string exprAfterDeduction;
    int i = 0;
    s = "";
    curExpr = 0;
    result.clear();
    f = false;

    lastContext = expr[curExpr++];
    int turniketPos = lastContext.find("|-");
    int commaPos = 0;
    int cntNewAxioms = 0;
    for (int i = 0; i < turniketPos; i++) {
        if (lastContext[i] == ',') {
            commaPos = i;
            axioms.push_back(main_parser.parse(s));
            cntNewAxioms++;
            s = "";
            continue;
        }
        s.push_back(lastContext[i]);
    }

    if (!s.length()) { // if expr is without context
        result = expr;
        return;
    }

    exprAfterDeduction = lastContext.substr(0, commaPos) + "|-(" + s + ")->" + lastContext.substr(turniketPos + 2);  
    result.push_back(exprAfterDeduction); // .push_back(newContext without lastContext)
    lastContext = "(" + s + ")";
    result.push_back("");
    expressions = expr;
    for (int i = 1; i < (int) expressions.size(); i++) {
        expressions[i] = "(" + expressions[i] + ")";
    }

    for (;curExpr < (int) expressions.size(); curExpr++) {
        s = expressions[curExpr];

        history.push_back(s);
        forest.push_back(main_parser.parse(s));

        int nAxiom = isAxiom(forest[i]);
        if (nAxiom) {
            result[r_sz] = s;
            result.push_back("");

            result[r_sz] = s + "->" + lastContext + "->" + s;
            result.push_back("");

            result[r_sz] = lastContext + "->" + s;
            result.push_back("");
        } else { // MP
            long long curHash = forest[i]->hash;
            long long leftNewHash = 0;
            bool flag = false;
            for (int j = i - 1; j >= 0 && !flag; j--) {
                if (forest[j]->right && forest[j]->right->hash == curHash) {
                    leftNewHash = forest[j]->left->hash;
                    for (int z = i - 1; z >= 0; z--) {
                        if (forest[z]->hash == leftNewHash) {
                            flag = true;
                            result[r_sz] = '(' + lastContext + "->" + history[z] + ")->";
                            result[r_sz] += "(" + lastContext + "->" + history[z] + "->" + s + ")->";
                            result[r_sz] += lastContext + "->" + s;
                            result.push_back("");

                            result[r_sz] = "(" + lastContext + "->" + history[z] + "->" + s + ")->";
                            result[r_sz] += lastContext + "->" + s;
                            result.push_back("");

                            result[r_sz] = lastContext + "->" + s;
                            result.push_back("");
                            break;
                        }
                    }
                }
            }

            //!MP
            //a -> a
            if (!flag) {
                // a->a->a
                result[r_sz] = lastContext + "->" + lastContext + "->" + lastContext;
                result.push_back("");

                //aks 2
                result[r_sz] = "(" + lastContext + "->" + lastContext + "->" + lastContext + ")->";
                result[r_sz] += "(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")->";
                result[r_sz] += lastContext + "->" + lastContext;
                result.push_back("");

                //MP
                result[r_sz] = "(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")->";
                result[r_sz] += lastContext + "->" + lastContext;
                result.push_back("");

                // aks 1
                result[r_sz] = "(" + lastContext + "->" + "(" + lastContext + "->" + lastContext + ")->" + lastContext + ")" ;
                result.push_back("");

                //MP
                result[r_sz] = lastContext + "->" + lastContext;
                result.push_back("");
            }
        }
        i++;
    }

    for (int i = (int) axioms.size() - cntNewAxioms; i < (int) axioms.size(); i++) {
        delete axioms[i];
    }
    axioms.erase(axioms.end() - cntNewAxioms, axioms.end());
}
