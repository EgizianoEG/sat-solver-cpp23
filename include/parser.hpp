#ifndef PARSER_HPP
#define PARSER_HPP

#include <istream>
#include "formula.hpp"

/**
 * @brief Reads CNF clauses interactively from an input stream and builds a `Formula`.
 *
 * Input contract:
 *   - One clause per line; tokens are space-separated literals.
 *   - A token with a leading '-' denotes a negated literal (e.g. `"-P`" → `¬P`).
 *   - The token "done" (case-insensitive, whole line) terminates input.
 *   - Empty lines are silently skipped.
 *   - Variable names are case-sensitive alphanumeric strings.
 *   - An empty formula (no clauses before "done") is valid and trivially satisfiable.
 *
 * The constructor accepts any `std::istream`, enabling file-based testing without
 * changing parsing logic.
 */
class Parser {
   public:
    /**
     * @brief Constructs a Parser bound to the provided input stream.
     * @param input The stream to read clause lines from.
     */
    explicit Parser(std::istream& input);

    /**
     * @brief Reads and parses all clauses until the `"done"` sentinel or EOF.
     *
     * Clause prompts ("Clause N: ") are written to `std::cout`. The method
     * populates the internal variable list as a side-effect, making it
     * available via variables() afterwards.
     *
     * @return The parsed CNF Formula.
     * @throws `std::invalid_argument` if a token contains a bare '-' with no name,
     *         or if a variable name contains non-alphanumeric characters.
     */
    Formula parse();

    /**
     * @brief Returns the ordered list of unique variable names seen during parse().
     *
     * Variables appear in the order they were first encountered while reading
     * clauses. The list is empty before parse() is called.
     *
     * @return VariableList in first-appearance order.
     */
    VariableList variables() const;

   private:
    std::istream* input_;     ///< Pointer to the bound input stream.
    VariableList variables_;  ///< Unique variable names accumulated during parse().
};

#endif  // PARSER_HPP