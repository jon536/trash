#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <unordered_map>

using namespace std;

struct Tree {
    string str;
    long long hash;
    int size;
    Tree* left;
    Tree* right;
    Tree() {
        left = right = NULL;
        hash = size = 0;
    }
};
typedef Tree* linkOnTree;

enum Lexems {
    Begin, OpenBracket, CloseBracket, And, Or, Not, Entailment, Variable, End
};
Lexems curLexem = Begin;
string var;

string s;
vector<linkOnTree> forest;
vector<linkOnTree> axioms;
unordered_map<string, long long> axiomToHash;
unordered_map<long long, pair<int, linkOnTree> > rightTreeHash;
unordered_map<long long, pair<int, linkOnTree> > prevTreeHash;
int it; 

const int maxLen = 1e4;
long long q = 3;
long long qPow[maxLen];

linkOnTree expr();
linkOnTree disjunction();
linkOnTree conjunction();
bool f;

bool good(int x) {
    return x >= 0 && x < (int) s.length();
}

bool goodCharForVar(char x) {
    return (x >= 'A' && x <= 'Z') || (x >= '0' && x <= '9');
}

string trimWhiteSpace(string const& s) {
    string temp;
    for (auto c : s) {
        if (!isspace(c)) {
            temp.push_back(c);
        }
    }
    return temp;
}

void itIsAxiom(linkOnTree vertex, linkOnTree axiom) {
    if (!axiom || !vertex) {
        if (axiom || vertex) 
            f = false;
        return;
    }

    if (goodCharForVar(axiom->str[0])) {
        if (axiomToHash.count(axiom->str)) {
            if (axiomToHash[axiom->str] != vertex->hash) {
                f = false;
            }
        } else {
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

int isAxiom(linkOnTree vertex) {
    for (int i = 0; i < (int) axioms.size(); i++)  {
        axiomToHash.clear();
        f = true;
        itIsAxiom(vertex, axioms[i]);
        if (f) {
            return i + 1;
        }
    }
    return 0;
}

char nextToken() {
    do {
       it++; 
    } while(good(it) && isspace(s[it]));
    return s[it];
}

void nextLexem() {
    nextToken();
    if (!good(it)) {
        curLexem = End;
        return;
    }

    switch(s[it]) {
        case '(':
            curLexem = OpenBracket;        
        break;

        case ')':
            curLexem = CloseBracket;        
        break;

        case '!':
            curLexem = Not;
        break;

        case '&':
            curLexem = And;
        break;

        case '|':
            curLexem = Or;
        break;

        case '-': // ->
            it++;
            if (good(it) && s[it] == '>') {
                curLexem = Entailment;        
            } else {
                throw runtime_error("unexpected token on position" + to_string(it));
            }
        break;

        default:
            if (s[it] >= 'A' && s[it] <= 'Z') {
                var = "";
                while (good(it) && goodCharForVar(s[it])) {
                   var.push_back(s[it]); 
                   it++;
                } 
                it--;
                curLexem = Variable;        
            } else {
                throw runtime_error("unexpected token on position" + to_string(it));
            }
        break;
    } 
}

long long getHashStr(string const& s) {
    long long temp = 0;
    for (int i = 0; i < (int) s.length(); i++) {
        temp += qPow[i] * s[i];
    }
    return temp;
}

linkOnTree updateVertex(linkOnTree vertex, linkOnTree left, linkOnTree right, string const& str) {
    if (!vertex) 
        return left;
    vertex->right = right;
    vertex->left = left;
    vertex->str = str;

    int rsize = (vertex->right) ? vertex->right->size : 0; 
    int lsize = (vertex->left) ? vertex->left->size : 0; 

    int rhash = (vertex->right) ? vertex->right->hash : 0; 
    int lhash = (vertex->left) ? vertex->left->hash : 0; 

    vertex->size = lsize + rsize + 1;
    vertex->hash = lhash + qPow[lsize] * (getHashStr(str) + rhash * q);
    return vertex;
}

// not
linkOnTree nigation() { 
    linkOnTree vertex = NULL;
    if (curLexem == Not) {
        vertex = new Tree;
        nextLexem();
        updateVertex(vertex, nigation(), NULL, "!");
    } else if(curLexem == Variable) {
        vertex = new Tree;
        nextLexem();
        updateVertex(vertex, NULL, NULL, var); 
    } else if( curLexem == OpenBracket) {
        nextLexem();
        vertex = expr();

        if (curLexem != CloseBracket) {
            throw runtime_error("expected ) on position" + to_string(it));    
        } else {
            nextLexem();
        }
    }
    return vertex;
}

// &
linkOnTree conjunction() {
    linkOnTree left = nigation();
    linkOnTree vertex = NULL;
    linkOnTree right = NULL;
    if (curLexem == And) {
        vertex = new Tree;
        nextLexem();
        right = conjunction();
    }
    return updateVertex(vertex, left, right, "&");
}

// |
linkOnTree disjunction() {
    linkOnTree left = conjunction();
    linkOnTree vertex = NULL;
    linkOnTree right = NULL;
    if (curLexem == Or) {
        vertex = new Tree;
        nextLexem();
        right = disjunction(); 
    }
    return updateVertex(vertex, left, right, "|");
}

// ->, (expr)
linkOnTree expr() {
    linkOnTree left = disjunction(); 
    linkOnTree right = NULL;
    linkOnTree vertex = NULL;
    if (curLexem == Entailment) {
        vertex = new Tree;
        nextLexem();
        right = expr();
    }
    return updateVertex(vertex, left, right, "->");
}

linkOnTree parse(const string& s2) {
    s = s2;
    it = -1; 
    nextLexem(); 
    return expr();
}


void output(int n, string const& s, int flag, int x = 0, int y = 0) {
    cout << "(" << n + 1 << ") " << trimWhiteSpace(s); 
    if (flag == 0)
        cout << " (Сх. акс. " << x << ")";
    else if (flag == 1)
        cout << " (M.P. " << x + 1 << ", " << y + 1 << ")";
    else
        cout << " (Не доказано)";
    cout << endl;
}

int main() {
    #ifdef DEBUG
    freopen("in2", "r", stdin);
    freopen("out", "w", stdout);
    #endif
    double start = clock() * 1.0 / CLOCKS_PER_SEC;  
    qPow[0] = 1;
    for (int i = 1; i < maxLen; i++) {
        qPow[i] = qPow[i - 1] * q;
    }

    axioms.push_back(parse("A -> B -> A"));
    axioms.push_back(parse("(A -> B) -> (A -> B -> C) -> (A -> C)"));
    axioms.push_back(parse("A -> B -> A & B"));
    axioms.push_back(parse("A & B -> A"));
    axioms.push_back(parse("A & B -> B"));
    axioms.push_back(parse("A -> A | B"));
    axioms.push_back(parse("B -> A | B"));
    axioms.push_back(parse("(A -> C) -> (B -> C) -> (A | B -> C)"));
    axioms.push_back(parse("(A -> B) -> (A -> !B) -> !A"));
    axioms.push_back(parse("!!A -> A"));

    
    int i = 0;
    while (getline(cin, s)) {
        if (!s.length())
            continue;
        forest.push_back(parse(s));

        int nAxiom;
        if (nAxiom = isAxiom(forest[i])) {
            output(i, s, 0, nAxiom);
        } else {
            bool flag = false;
            auto iterOnTree = rightTreeHash.find(forest[i]->hash);
            if (iterOnTree != rightTreeHash.end()) {
                linkOnTree findedTree = iterOnTree->second.second;
                if (findedTree->str == "->") {
                    auto iterOnLeftTree = prevTreeHash.find(findedTree->left->hash);
                    if (iterOnLeftTree != prevTreeHash.end()) {
                        flag = true;
                        output(i, s, 1, iterOnLeftTree->second.first, iterOnTree->second.first);
                    }
                }
            }
            if (!flag) {
                output(i, s, 2);
                return 0;
            }
        }

        prevTreeHash[forest[i]->hash] = make_pair(i, forest[i]);
        if (forest[i]->right) 
            rightTreeHash[forest[i]->right->hash] = make_pair(i, forest[i]);

        i++;
    }
    double finish = clock() * 1.0 / CLOCKS_PER_SEC;  
    cerr << finish - start << endl;

    return 0;
}
