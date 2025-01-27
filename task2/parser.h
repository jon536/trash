#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <unordered_map>

struct parser {
    parser();
    struct Tree {
        Tree();
        std::string str;
        std::string exprSubTree;
        long long hash;
        int size;
        bool valWholeTree;
        Tree* left;
        Tree* right;
        ~Tree();
    };
    typedef Tree* linkOnTree;
    linkOnTree parse(const std::string& s2);
    static void print(linkOnTree);

private:
    enum Lexems {
        Begin, OpenBracket, CloseBracket, And, Or, Not, Entailment, Variable, End
    };

    Lexems curLexem;
    std::string var;
    std::string s;
    std::unordered_map<std::string, long long> axiomToHash;
    int it; 

    static const int maxLen = 1e4;
    long long q;
    long long qPow[maxLen];

    bool good(int); 
    char nextToken();
    void nextLexem();
    long long getHashStr(std::string const&);

    linkOnTree updateVertex(linkOnTree, linkOnTree, linkOnTree, std::string const&);
    linkOnTree nigation();
    linkOnTree conjunction();
    linkOnTree disjunction();
    linkOnTree expr();
    friend bool operator==(const Tree& fst, const Tree& scd);
    friend bool operator!=(const Tree& fst, const Tree& scd);
};
bool goodCharForVar(char);

bool operator==(const parser::Tree& fst, const parser::Tree& scd);
bool operator!=(const parser::Tree& fst, const parser::Tree& scd);
#endif
