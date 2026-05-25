#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "formula.hpp"

/**
 * @brief Evaluates a single literal under the given truth assignment.
 *
 * For a positive literal the result is the assigned value of the variable.
 * For a negated literal the result is the complement. If the variable is
 * absent from the assignment the literal is treated as false.
 *
 * @param literal    The literal to evaluate.
 * @param assignment The current (possibly partial) truth assignment.
 * @return `true` if the literal is satisfied, `false` otherwise.
 */
bool EvaluateLiteral(const Literal& literal, const Assignment& assignment);

/**
 * @brief Evaluates a clause (disjunction of literals) under the given assignment.
 *
 * A clause is satisfied when at least one of its literals is satisfied.
 * An empty clause (no literals) is vacuously false.
 *
 * @param clause     The clause to evaluate.
 * @param assignment The current truth assignment.
 * @return `true` if the clause is satisfied, `false` otherwise.
 */
bool EvaluateClause(const Clause& clause, const Assignment& assignment);

/**
 * @brief Evaluates a CNF formula under the given assignment.
 *
 * A formula is satisfied when every clause is satisfied.
 * An empty formula (no clauses) is vacuously true.
 *
 * @param formula    The CNF formula to evaluate.
 * @param assignment The current truth assignment.
 * @return `true` if every clause is satisfied, `false` otherwise.
 */
bool EvaluateFormula(const Formula& formula, const Assignment& assignment);

#endif  // EVALUATOR_HPP