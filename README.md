# SAT Solver for Logic Problems Using Discrete Mathematics

> A Boolean Satisfiability (SAT) solver built in C++23 as part of the Discrete Mathematics
> course project. Given a propositional logic formula in Conjunctive Normal Form (CNF), the
> program determines whether a satisfying truth assignment exists; and if so, outputs it.

---

## Table of Contents

- [Background](#background)
- [Algorithms](#algorithms)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Example](#example)
- [Input Format](#input-format)
- [Development](#development)
- [License](#license)

---

## Background

A **Boolean Satisfiability Problem (SAT)** asks: given a propositional logic formula over
boolean variables, is there at least one assignment of `true`/`false` values to all variables
that makes the entire formula evaluate to `true`?

This project operates on formulas in **Conjunctive Normal Form (CNF)**, the standard
representation used by SAT solvers universally. A CNF formula is structured as:

```dsconfig
Formula  =  Clause₁  ∧  Clause₂  ∧  ...  ∧  Clauseₙ
Clause   =  Literal₁  ∨  Literal₂  ∨  ...  ∨  Literalₘ
Literal  =  variable  |  ¬variable
```

The formula is satisfied when **every clause** contains at least one `true` literal.

### Example

```dsconfig
(P ∨ Q) ∧ (¬P ∨ R) ∧ (¬Q ∨ ¬R)
```

The solver searches for values of `P`, `Q`, `R` and finds:

```cpp
P = true,  Q = false,  R = true   →   SATISFIABLE ✓
```

---

## Algorithms

The solver implements and exposes two distinct strategies, allowing direct comparison
of their efficiency on the same input:

### 1. Brute-Force Enumeration

Systematically generates all `2ⁿ` possible truth assignments (where `n` is the number
of variables) and evaluates the formula against each. Guaranteed to find a solution if
one exists, but does not scale beyond ~20 variables.

- **Time Complexity:** O(2ⁿ · m), where m is the number of clauses.
- **Space Complexity:** O(n)

### 2. DPLL (Davis–Putnam–Logemann–Loveland) - *[To be implemented and tested in the future, I hope]*

A backtracking search algorithm that prunes the search space aggressively using two
inference rules applied before every branch decision:

- **Unit Propagation** — If a clause contains only one unassigned literal, that literal
  must be `true`. Assign it immediately (no branching needed).
- **Pure Literal Elimination** — If a variable appears with only one polarity across all
  remaining clauses (always positive, or always negative), assign it favorably and remove
  all clauses it satisfies.

These rules allow DPLL to solve many formulas without ever exhausting the full search tree.

- **Time Complexity:** O(2ⁿ) worst case, far better in practice due to pruning.
- **Space Complexity:** O(n · d), where d is the depth of the recursion tree.

Both solvers report the number of assignments explored, making the pruning effect of
DPLL directly visible.

---

## Project Structure

```mathematica
sat-solver/
│
├── include/                   # Public header files
│   ├── formula.hpp            # Core types: Literal, Clause, Formula, Assignment
│   ├── parser.hpp             # CNF formula parser (interactive + file input)
│   ├── evaluator.hpp          # Formula and clause evaluation logic
│   └── solver.hpp             # BruteForceSolver interface
│
├── source/                    # Implementation files
│   ├── formula.cpp
│   ├── parser.cpp
│   ├── evaluator.cpp
│   ├── solver.cpp
│   └── main.cpp               # CLI entry point
│
├── tests/                     # Hand-crafted CNF test cases (.cnf files) 
│
├── docs/                      # Doxygen-generated HTML documentation (after `make docs`)
│
├── .clang-format              # Formatting rules (Google style, C++23, 100-col)
├── .clang-tidy                # Static analysis and naming convention rules
├── .editorconfig              # Editor whitespace and encoding rules
├── .gitattributes             # Line-ending normalization (LF-only)
├── .gitignore
├── .github/
│   └── workflows/
│       └── code-quality.yml   # CI: clang-format, cpplint, cppcheck, clang-tidy
├── ARCHITECTURE.md            # In-depth component and data-flow documentation
├── CODING_STANDARD.md         # Naming, formatting, and documentation conventions
├── CPPLINT.cfg                # cpplint configuration
├── compile_flags.txt          # clangd LSP flags for IDE integration
├── Doxyfile                   # Doxygen configuration
├── LICENSE.md
├── Makefile
└── README.md
```

---

## Getting Started

### Prerequisites

| Tool         | Minimum Version | Purpose                                              |
| ------------ | --------------- | ---------------------------------------------------- |
| g++          | 13+             | Compiler (C++23 support required)                    |
| GNU Make     | 4.0+            | Build system                                         |
| clang-format | 22              | Code formatting (optional, for `make format`)        |
| clang-tidy   | 22              | Static analysis (optional, for `make lint`)          |
| cppcheck     | 2.x             | Static analysis (optional, for `make lint`)          |
| cpplint      | any             | Style linting (optional, for `make lint`)            |
| Doxygen      | 1.9+            | Documentation generation (optional, for `make docs`) |

### Building

```bash
# Clone the repository
git clone https://github.com/EgizianoEG/sat-solver-cpp23
cd sat-solver

# Build the binary (output: build/bin/sat_solver)
make build

# Or simply:
make
```

---

## Usage

```bash
# Build and run interactively
make run

# Or run the binary directly after building
./build/bin/sat_solver
```

### Additional Targets

```bash
make help      # Show all available make targets
make lint      # Run cpplint, cppcheck, and clang-tidy
make format    # Auto-format all C++ source files with clang-format
make docs      # Generate Doxygen HTML docs → open docs/html/index.html
make clean     # Remove build/ artifacts and generated docs
```

---

## Example

```text
══════════════════════════════════════════════════
  SAT Solver — Discrete Mathematics Project
══════════════════════════════════════════════════

Enter CNF formula clauses (one per line).
Format: space-separated literals, prefix '-' for negation.
Type 'done' when finished.

Clause 1: P Q
Clause 2: -P R
Clause 3: -Q -R
Clause 4: done

Formula entered:
  (P ∨ Q) ∧ (¬P ∨ R) ∧ (¬Q ∨ ¬R)

Variables detected: P, Q, R

──────────────────────────────────────────────────
[Brute-Force]
  Assignments explored : 8
  Result               : SATISFIABLE

[DPLL] (Projected)
  Assignments explored : 3
  Result               : SATISFIABLE

──────────────────────────────────────────────────
Satisfying Assignment:
  P  =  true
  Q  =  false
  R  =  true
──────────────────────────────────────────────────
```

---

## Input Format

Clauses are entered interactively, one per line. Each line is a space-separated list
of literals. A leading `-` denotes negation.

| Input token | Meaning                              |
| ----------- | ------------------------------------ |
| `P`         | The variable P (positive literal)    |
| `-P`        | The negation of P (negative literal) |
| `done`      | Signals end of input                 |

Variable names are case-sensitive and may be any non-whitespace alphanumeric string
(e.g., `P`, `Q1`, `Rain`, `x`).

---

## Development

### Coding Standard

See [`CODING_STANDARD.md`](CODING_STANDARD.md) for the full naming, formatting, and
documentation conventions enforced on this project.

Key rules at a glance:
- Language standard: **C++23**
- Classes/Structs: `PascalCase` — Private members: `snake_case_` (trailing underscore)
- Methods: `camelCase` — Free functions: `PascalCase` — Variables/Parameters: `snake_case`
- All public API must carry **Doxygen** `/** @brief ... */` comments
- Header guards via `#ifndef` / `#define` (no `#pragma once`)

### Continuous Integration

Every push and pull request to `main` is gated by GitHub Actions (`.github/workflows/code-quality.yml`),
which runs:

1. **clang-format-22** — formatting check (fails on any diff)
2. **cpplint** — Google style linting
3. **cppcheck** — static analysis
4. **clang-tidy-22** — modernization and naming checks

CI failures block merging. Local hooks can be bypassed with `git commit --no-verify`,
but server-side enforcement remains active.

### Architecture

See [`ARCHITECTURE.md`](ARCHITECTURE.md) for the full component breakdown, data-flow
diagrams, class designs, and algorithm walkthroughs.

---

## License

MIT License — see [`LICENSE.md`](LICENSE.md) for details.
