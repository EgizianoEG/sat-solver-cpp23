#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "formula.hpp"

bool EvaluateLiteral(const Literal& literal, const Assignment& assignment);
bool EvaluateClause(const Clause& clause, const Assignment& assignment);
bool EvaluateFormula(const Formula& formula, const Assignment& assignment);

#endif  // EVALUATOR_HPP
