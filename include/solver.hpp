#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <cstdint>

#include "formula.hpp"

struct SolverResult {
    bool is_satisfiable;
    Assignment assignment;
    uint64_t steps;
};

class BruteForceSolver {
   public:
    SolverResult solve(const Formula& formula, const VariableList& variables);
};

#endif  // SOLVER_HPP
