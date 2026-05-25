#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>

#include "formula.hpp"

/**
 * @brief Encapsulates the result returned by any solver strategy.
 *
 * When is_satisfiable is false the assignment field is empty and should not
 * be consulted. The steps counter is always populated and records how many
 * candidate assignments were evaluated before reaching the verdict.
 */
struct SolverResult {
    bool is_satisfiable;    ///< True when a satisfying assignment was found.
    Assignment assignment;  ///< Satisfying assignment (valid only when `is_satisfiable` is true).
    uint64_t steps;         ///< Number of candidate assignments evaluated.
};

/**
 * @brief SAT solver that uses exhaustive brute-force enumeration.
 *
 * Iterates over all 2ⁿ possible truth assignments via a 64-bit bitmask loop.
 * Bit k of the counter encodes the truth value for variable k in the ordered
 * VariableList. The solver returns on the first satisfying assignment found;
 * if none exists it exhausts the entire space and reports UNSAT.
 *
 * Practical upper bound: n ≤ 20 variables for interactive use;
 * the implementation enforces a hard limit of n ≤ 63.
 */
class BruteForceSolver {
   public:
    constexpr static uint64_t MAX_VARIABLES = 63;  ///< Maximum variables supported by this solver.

    /**
     * @brief Solves a CNF formula by exhaustive assignment enumeration.
     *
     * @param formula   The CNF formula to solve.
     * @param variables Ordered variable list that defines the bit-to-variable mapping.
     * @return SolverResult containing the verdict, step count, and (on SAT) assignment.
     * @throws `std::invalid_argument` if `variables.size()` exceeds 63.
     */
    static SolverResult solve(const Formula& formula, const VariableList& variables);
};

#endif  // SOLVER_HPP