#include "formula.hpp"
#include <algorithm>
#include <string>

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

std::string LiteralToString(const Literal& literal) {
    return literal.is_negated ? ("¬" + literal.name) : literal.name;
}

/**
 * @brief Formats a single clause as a disjunction std::string, e.g. "P ∨ ¬Q ∨ R".
 * @param clause The clause to format.
 * @return The formatted disjunction std::string (without surrounding parentheses).
 */
std::string ClauseToString(const Clause& clause) {
    std::string result;

    for (std::size_t i = 0; i < clause.size(); ++i) {
        if (i > 0) {
            result += " ∨ ";
        }
        result += LiteralToString(clause[i]);
    }

    return result;
}

std::string FormulaToString(const Formula& formula) {
    std::string result;

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