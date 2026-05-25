#include "solver.hpp"
#include "evaluator.hpp"

#include <format>
#include <stdexcept>

SolverResult BruteForceSolver::solve(const Formula& formula, const VariableList& variables) {
    const std::size_t VAR_COUNT = variables.size();

    // Guard: 2^64 is impractical; a 63-bit bitmask is the maximum safe range
    // for uint64_t arithmetic while keeping the shift well-defined.
    if (VAR_COUNT > MAX_VARIABLES) {
        throw std::invalid_argument(std::format(
            "Variable count {} exceeds the brute-force limit of {}.", VAR_COUNT, MAX_VARIABLES));
    }

    const uint64_t TOTAL = (uint64_t{1} << VAR_COUNT);

    for (uint64_t i = 0; i < TOTAL; ++i) {
        // Build an assignment from the bitmask: bit k → truth value for variables[k].
        Assignment assignment;
        assignment.reserve(VAR_COUNT);
        for (std::size_t k = 0; k < VAR_COUNT; ++k) {
            assignment[variables[k]] = ((i >> k) & uint64_t{1}) != 0U;
        }

        if (EvaluateFormula(formula, assignment)) {
            return {
                .is_satisfiable = true,
                .assignment     = std::move(assignment),
                .steps          = i + 1,
            };
        }
    }

    return {
        .is_satisfiable = false,
        .assignment     = {},
        .steps          = TOTAL,
    };
}
