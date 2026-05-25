#include <iostream>
#include <string_view>

#include "formula.hpp"
#include "parser.hpp"
#include "solver.hpp"

namespace {

    constexpr std::string_view BANNER_LINE  = "══════════════════════════════════════════════════";
    constexpr std::string_view SECTION_LINE = "──────────────────────────────────────────────────";

    /// @brief Prints the welcome banner.
    void PrintBanner() {
        std::cout << "\n"
                  << BANNER_LINE << "\n"
                  << "  SAT Solver - Discrete Mathematics Project\n"
                  << BANNER_LINE << "\n\n";
    }

    /// @brief Prints the clause-entry instructions.
    void PrintInstructions() {
        std::cout << "Enter CNF formula clauses (one per line).\n"
                  << "Format: space-separated literals, prefix '-' for negation.\n"
                  << "Type 'done' when finished.\n\n";
    }

    /**
     * @brief Prints the parsed formula and detected variable list.
     * @param formula   The parsed CNF formula.
     * @param variables The ordered variable list extracted from the formula.
     */
    void PrintFormulaSection(const Formula& formula, const VariableList& variables) {
        std::cout << "\nFormula entered:\n  ";

        if (formula.empty()) {
            std::cout << "(empty \x{e2}\x{80}\x{94} trivially satisfiable)";
        } else {
            std::cout << FormulaToString(formula);
        }

        std::cout << "\n\nVariables detected: ";

        if (variables.empty()) {
            std::cout << "(none)";
        } else {
            for (std::size_t i = 0; i < variables.size(); ++i) {
                if (i > 0) {
                    std::cout << ", ";
                }
                std::cout << variables[i];
            }
        }

        std::cout << "\n";
    }

    /**
     * @brief Prints the brute-force solver summary.
     * @param result The result returned by BruteForceSolver::solve().
     */
    void PrintBruteForceResult(const SolverResult& result) {
        std::cout << "\n"
                  << SECTION_LINE << "\n"
                  << "[Brute-Force]\n"
                  << "  Assignments explored : " << result.steps << "\n"
                  << "  Result               : "
                  << (result.is_satisfiable ? "SATISFIABLE" : "UNSATISFIABLE") << "\n";
    }

    /**
     * @brief Prints the satisfying assignment, or an UNSAT verdict.
     * @param result    The solver result.
     * @param variables The ordered variable list (controls print order).
     */
    void PrintAssignment(const SolverResult& result, const VariableList& variables) {
        std::cout << "\n" << SECTION_LINE << "\n";

        if (result.is_satisfiable) {
            std::cout << "Satisfying Assignment:\n";
            for (const auto& var : variables) {
                std::cout << "  " << var << "  =  "
                          << (result.assignment.at(var) ? "true" : "false") << "\n";
            }
        } else {
            std::cout << "No satisfying assignment exists (UNSATISFIABLE).\n";
        }

        std::cout << SECTION_LINE << "\n";
    }

}  // namespace

int main() {  // NOLINT(bugprone-exception-escape)
    try {
        PrintBanner();
        PrintInstructions();

        Parser parser(std::cin);
        Formula formula;
        formula                      = parser.parse();

        const VariableList VARIABLES = parser.variables();
        PrintFormulaSection(formula, VARIABLES);

        SolverResult result;
        result = BruteForceSolver::solve(formula, VARIABLES);

        PrintBruteForceResult(result);
        PrintAssignment(result, VARIABLES);

        return 0;
    } catch (const std::invalid_argument& err) {
        std::cerr << "\n[Error] " << err.what() << "\n";
        return 1;
    } catch (const std::exception& err) {
        std::cerr << "\n[Error] " << err.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[Error] Unexpected exception.\n";
        return 1;
    }
}