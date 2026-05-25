#include "evaluator.hpp"
#include <algorithm>

bool EvaluateLiteral(const Literal& literal, const Assignment& assignment) {
    const auto ITERATOR = assignment.find(literal.name);

    if (ITERATOR == assignment.end()) {
        return false;  // Unassigned variables are treated as false.
    }

    return literal.is_negated ? !ITERATOR->second : ITERATOR->second;
}

bool EvaluateClause(const Clause& clause, const Assignment& assignment) {
    return std::ranges::any_of(
        clause, [&assignment](const Literal& lit) { return EvaluateLiteral(lit, assignment); });
}

bool EvaluateFormula(const Formula& formula, const Assignment& assignment) {
    return std::ranges::all_of(formula, [&assignment](const Clause& clause) {
        return EvaluateClause(clause, assignment);
    });
}