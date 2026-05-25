# C++ Coding Standard

This project follows a defined set of styling, design, and architecture guidelines. The foundations of these guidelines aim at aligning with the standard [C++ Coding Standard](https://users.ece.cmu.edu/~eno/coding/CppCodingStandard.html#documentation), with constraints and decisions customized for this SAT solver project.

## 1. Standards and Tooling

- **Language Standard:** C++23.
- **Formatting:** Handled via `.clang-format`. We adopt Google's style with structured alignment, brace-wrapping, and include block preservation rules.
- **Line Width:** 100 characters max per line.
- **Linting & Analysis:** Handled via CI and `.clang-tidy`.

## 2. Naming Conventions

To maintain uniformity across the team, adhere strictly to the following conventions (enforced via style checks and `clang-tidy`):

* **Files:** `lower_snake_case` (e.g., `.cpp`, `formula_parser.h`).
* **Classes & Structs:** `PascalCase` (e.g., `class FormulaParser`).
* **Private Member Variables:** `snake_case_` with a trailing underscore (e.g., `int clause_count_;`).
* **Member Functions (Methods):** `camelCase` (e.g., `void getClauseCount()`).
* **Independent / Global Functions:** `PascalCase` (e.g., `void SolveFormula()`).
* **Parameters:** `snake_case` (e.g., `void setClauseCount(int clause_count)`).
* **Constants:** `UPPER_SNAKE_CASE` (e.g., `const int MAX_CLAUSES = 100;`).
* **General Variables:** `snake_case`.

## 3. Headers and Formats

* **Header Guards:** Always use `#ifndef` / `#define` standard macro guards for header files (do not use `#pragma once` as it aligns better with C standards and strict compatibility).

  ```cpp
  #ifndef FORMULA_PARSER_H
  #define FORMULA_PARSER_H
  // ...
  #endif
  ```

* **Includes Order:** `.clang-format` enforces sorted includes via `SortIncludes: true` and preserves block structure via `IncludeBlocks: Preserve`. Include blocks are: project headers, blank line, third-party headers, blank line, standard library headers.

## 4. Error Handling

* **Exceptions Allowed:** You may use C++ standard exceptions (`throw`, `try-catch`) to handle exceptional circumstances.
* **Error Codes Discouraged:** Returning arbitrary error flags or boolean status codes indicating failures as output variables is highly discouraged. Let errors be handled via idiomatic exceptions.

## 5. Documentation

We require **Doxygen-style comments** for all public types, classes, methods, and functions. A `Doxyfile` is provided in the repository.
* Include at least minimal explanation of the tags (e.g., `@brief`, `@param`, `@return`).
* It is not necessary to extensively detail the whole internal process stringently if the function/method is computationally trivial or self-explanatory (in such cases, standard brief tags are sufficient).

```cpp
/**
 * @brief Retrieves the current clause count.
 * @return The number of clauses currently tracked by the parser.
 */
int getSomething() const;
```

### Remote Verification (GitHub Actions)

Bypassing local hooks is possible with `git commit --no-verify` or `git commit -n`, but
merging to `main` is gated by server-side CI.

- CI triggers on every push and Pull Request.
- Formatting, naming, and lint violations will fail the workflow.
