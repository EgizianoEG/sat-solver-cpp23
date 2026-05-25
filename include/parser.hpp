#ifndef PARSER_HPP
#define PARSER_HPP

#include <istream>

#include "formula.hpp"

class Parser {
   public:
    explicit Parser(std::istream& input);

    Formula parse();
    VariableList variables() const;

   private:
    std::istream* input_;
    VariableList variables_;
};

#endif  // PARSER_HPP
