#ifndef FORMULA_HPP
#define FORMULA_HPP

#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

/**
 * @brief A single propositional variable reference, optionally negated.
 *
 * A positive literal represents the variable itself (e.g. P); a negative
 * literal represents its negation (e.g. ¬P).
 */
struct Literal {
    string name;      ///< Variable identifier, e.g. "P", "Q1", "Rain"
    bool is_negated;  ///< True when this literal is the negation of the variable
};

/// @brief A disjunction of literals: (L₁ ∨ L₂ ∨ … ∨ Lₘ).
using Clause = vector<Literal>;

/// @brief A conjunction of clauses in CNF: C₁ ∧ C₂ ∧ … ∧ Cₙ.
using Formula = vector<Clause>;

/// @brief A (partial) truth assignment mapping variable names to boolean values.
using Assignment = unordered_map<string, bool>;

/// @brief An ordered list of unique variable names, in first-appearance order.
using VariableList = vector<string>;

/**
 * @brief Scans a formula and returns all unique variable names in appearance order.
 * @param formula The CNF formula to scan.
 * @return VariableList of unique names ordered by first occurrence.
 */
VariableList ExtractVariables(const Formula& formula);

/**
 * @brief Converts a literal to a display string.
 * @param literal The literal to format.
 * @return `"P"` for a positive literal, `"¬P"` for a negated one.
 */
string LiteralToString(const Literal& literal);

/**
 * @brief Converts a CNF formula to a human-readable string.
 * @param formula The formula to format.
 * @return A string such as `"(P ∨ Q) ∧ (¬P ∨ R) ∧ (¬Q ∨ ¬R)"`.
 */
string FormulaToString(const Formula& formula);

#endif  // FORMULA_HPP