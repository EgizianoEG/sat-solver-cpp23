#include "parser.hpp"

#include <algorithm>
#include <iostream>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>

Parser::Parser(std::istream& input) : input_(&input) {}

Formula Parser::parse() {
    Formula formula;

    int clause_num = 1;
    std::string line;
    bool done = false;

    while (!done) {
        std::cout << "Clause " << clause_num << ": ";

        if (!std::getline(*input_, line)) {
            break;  // EOF or read error.
        }

        // Trim leading and trailing whitespace.
        const auto FIRST = line.find_first_not_of(" \t\r\n");
        if (FIRST == std::string::npos) {
            continue;  // Blank line — re-prompt on the same clause number.
        }

        const auto LAST = line.find_last_not_of(" \t\r\n");
        line            = line.substr(FIRST, LAST - FIRST + 1);

        // Check for the "done" sentinel (whole trimmed line, case-insensitive).
        std::string lower_line = line;
        std::ranges::transform(lower_line, lower_line.begin(), [](unsigned char chr) {
            return static_cast<char>(std::tolower(chr));
        });
        if (lower_line == "done") {
            done = true;
            continue;  // Exit loop via loop condition check.
        }

        // Tokenize the line into literals.
        Clause clause;
        std::istringstream token_stream(line);
        std::string token;

        while (token_stream >> token) {
            Literal lit;

            if (token.front() == '-') {
                if (token.size() < 2) {
                    throw std::invalid_argument(
                        "Invalid literal token: '-' must be followed by a variable name.");
                }
                lit.name       = token.substr(1);
                lit.is_negated = true;
            } else {
                lit.name       = token;
                lit.is_negated = false;
            }

            clause.push_back(lit);
        }

        if (!clause.empty()) {
            formula.push_back(clause);
            clause_num++;
        }
    }

    variables_ = ExtractVariables(formula);
    return formula;
}

const VariableList& Parser::variables() const {
    return variables_;
}