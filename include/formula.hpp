#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

struct Literal {
    string name;
    bool is_negated;
};

using Clause       = vector<Literal>;
using Formula      = vector<Clause>;
using Assignment   = unordered_map<string, bool>;
using VariableList = vector<string>;

VariableList ExtractVariables(const Formula& formula);
string LiteralToString(const Literal& literal);
string FormulaToString(const Formula& formula);

#endif  // FORMULA_HPP
