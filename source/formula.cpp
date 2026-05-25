#include "formula.hpp"
#include <algorithm>

VariableList ExtractVariables(const Formula& formula) {
    VariableList variables;

    for (const auto& clause : formula) {
        for (const auto& lit : clause) {
            if (!std::ranges::contains(variables, lit.name)) {
                variables.push_back(lit.name);
            }
        }
    }

    return variables;
}

string LiteralToString(const Literal& literal) {
    return literal.is_negated ? ("¬" + literal.name) : literal.name;
}

string ClauseToString(const Clause& clause) {
    string result;

    for (std::size_t i = 0; i < clause.size(); ++i) {
        if (i > 0) {
            result += " ∨ ";
        }

        result += LiteralToString(clause[i]);
    }

    return result;
}

string FormulaToString(const Formula& formula) {
    string result;

    for (std::size_t i = 0; i < formula.size(); ++i) {
        if (i > 0) {
            result += " ∧ ";
        }

        result += "(";
        result += ClauseToString(formula[i]);
        result += ")";
    }

    return result;
}