#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct Literal {
    std::string name;
    bool is_negated;
};

using Clause       = std::vector<Literal>;
using Formula      = std::vector<Clause>;
using Assignment   = std::unordered_map<std::string, bool>;
using VariableList = std::vector<std::string>;

VariableList ExtractVariables(const Formula& formula);
std::string LiteralToString(const Literal& literal);
std::string FormulaToString(const Formula& formula);

#endif  // FORMULA_HPP
