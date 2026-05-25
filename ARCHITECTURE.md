# Architecture — SAT Solver for Logic Problems

This document describes the internal design of the SAT Solver: its components,
data structures, control flow, and the rationale behind key decisions. It is intended
as the authoritative technical reference for contributors.

---

## Table of Contents

- [Architecture — SAT Solver for Logic Problems](#architecture--sat-solver-for-logic-problems)
  - [Table of Contents](#table-of-contents)
  - [High-Level Overview](#high-level-overview)
  - [Component Map](#component-map)
  - [Data Structures](#data-structures)
    - [Why `std::unordered_map` for Assignment?](#why-stdunordered_map-for-assignment)
    - [Why `std::vector` for Clause and Formula?](#why-stdvector-for-clause-and-formula)
  - [Component Details](#component-details)
    - [1. `formula.hpp` / `formula.cpp`](#1-formulahpp--formulacpp)
    - [2. `parser.hpp` / `parser.cpp`](#2-parserhpp--parsercpp)
    - [3. `evaluator.hpp` / `evaluator.cpp`](#3-evaluatorhpp--evaluatorcpp)
    - [4. `solver.hpp` / `solver.cpp`](#4-solverhpp--solvercpp)
      - [BruteForceSolver internals](#bruteforcesolver-internals)
    - [5. `main.cpp`](#5-maincpp)
  - [Control Flow](#control-flow)
  - [Algorithm Walkthroughs](#algorithm-walkthroughs)
    - [Brute-Force Enumeration](#brute-force-enumeration)
  - [Design Decisions](#design-decisions)
    - [Free functions for Evaluator, class for Solver](#free-functions-for-evaluator-class-for-solver)
    - [`std::unordered_map` over `std::map` for Assignment](#stdunordered_map-over-stdmap-for-assignment)
    - [`VariableList` is ordered (`std::vector`)](#variablelist-is-ordered-stdvector)
    - [No global state](#no-global-state)
  - [Known Limitations and Extensions](#known-limitations-and-extensions)

---

## High-Level Overview

```mathematica
 ┌──────────────────────────────────────────────────────────────────┐
 │                         main.cpp (CLI)                           │
 │   Orchestrates: parse → solve → display result                   │
 └───────────────────┬──────────────────────────────────────────────┘
                     │
        ┌────────────▼────────────┐
        │       Parser            │  Reads raw text input,
        │  (parser.hpp/.cpp)      │  produces a typed Formula.
        └────────────┬────────────┘
                     │ Formula (CNF)
        ┌────────────▼────────────┐
        │      Evaluator          │  Pure functions. Given a Formula
        │  (evaluator.hpp/.cpp)   │  and an Assignment, return bool.
        └────────────┬────────────┘
                     │ used by ↓
        ┌────────────▼────────────┐
        │       Solver            │  Brute-force strategy returning
        │   (solver.hpp/.cpp)     │  SolverResult.
        └─────────────────────────┘
                     │
        ┌────────────▼────────────┐
        │  formula.hpp (types)    │  Shared data types consumed by
        │  Literal, Clause,       │  all components. No logic here.
        │  Formula, Assignment    │
        └─────────────────────────┘
```

The dependency graph is deliberately **one-directional**. No component imports from
a layer above it. `formula.hpp` is a pure type header included by everyone.

---

## Component Map

```mathematica
include/
  formula.hpp      ← Shared types only. No logic.
  parser.hpp       ← Declares Parser class.
  evaluator.hpp    ← Declares Evaluate* free functions.
  solver.hpp       ← Declares SolverResult, BruteForceSolver.

source/
  formula.cpp      ← Any non-trivial Formula/Assignment helpers.
  parser.cpp       ← Parsing implementation.
  evaluator.cpp    ← Evaluation implementation.
  solver.cpp       ← Solver implementation.
  main.cpp         ← Entry point. No logic, only orchestration.
```

---

## Data Structures

All core types live in `formula.hpp`. They are the lingua franca passed between
every component. Keeping them in one place means changing a type is a single-file
edit with a clearly visible blast radius.

```cpp
// A single variable reference, possibly negated.
struct Literal {
    std::string name;   // e.g. "P", "Q1", "Rain"
    bool        is_negated;
};

// A disjunction of literals: (L1 ∨ L2 ∨ ... ∨ Lm)
using Clause = std::vector<Literal>;

// A conjunction of clauses: C1 ∧ C2 ∧ ... ∧ Cn  (CNF)
using Formula = std::vector<Clause>;

// A complete or partial truth assignment: variable name → bool
using Assignment = std::unordered_map<std::string, bool>;

// The ordered list of all unique variable names in the formula.
// Determined at parse time; consumed by the BruteForce solver for enumeration.
using VariableList = std::vector<std::string>;
```

### Why `std::unordered_map` for Assignment?

Lookup during evaluation is O(1) average rather than O(log n). Since `EvaluateClause`
calls `assignment.find(literal.name)` for every literal in every clause, and this is
called millions of times in brute-force mode, hash-map access is the right trade-off.

### Why `std::vector` for Clause and Formula?

Clauses and formulas are built once (at parse time) and then read many times. Contiguous
memory gives the evaluator better cache performance when iterating. Mutation needed
during solving is done on **copies**, not in place.

---

## Component Details

### 1. `formula.hpp` / `formula.cpp`

**Role:** Type definitions and lightweight helpers.

Contains no logic beyond utility operations on the types — for example, extracting the
set of unique variable names from a Formula, or pretty-printing a Literal as `P` or `¬P`.

```cpp
formula.hpp exports:
  struct  Literal
  using   Clause
  using   Formula
  using   Assignment
  using   VariableList

formula.cpp exports:
  VariableList  ExtractVariables(const Formula&)
  std::string   LiteralToString(const Literal&)
  std::string   FormulaToString(const Formula&)
```

---

### 2. `parser.hpp` / `parser.cpp`

**Role:** Converts raw text input into a typed `Formula`.

The `Parser` class reads clauses line by line from `std::istream` (defaulting to
`std::cin`, but accepting any stream — useful for file-based testing). Each non-empty,
non-sentinel line is tokenized on whitespace. A token starting with `-` is parsed as a
negated literal; otherwise it is positive.

```cpp
parser.hpp exports:
  class Parser {
    explicit Parser(std::istream& input = std::cin)
    Formula      parse()           // Reads until "done" sentinel
    VariableList variables() const // Returns unique vars after parse()
  }
```

**Input contract:**
- One clause per line.
- Tokens are space-separated literals (`P`, `-Q`, `Rain`, `-x1`).
- The token `done` (case-insensitive) terminates input.
- Empty lines are skipped.
- Variable names are alphanumeric, case-sensitive.

**Error handling:** Malformed tokens throw `std::invalid_argument`. An empty formula
(no clauses entered before `done`) is valid — it is trivially satisfiable.

---

### 3. `evaluator.hpp` / `evaluator.cpp`

**Role:** Pure evaluation functions. No state, no side effects.

These are free functions (not a class) because they carry no state. They are the single
source of truth for what "satisfying a clause" and "satisfying a formula" means.

```cpp
evaluator.hpp exports:
  bool EvaluateLiteral(const Literal&,  const Assignment&)
  bool EvaluateClause (const Clause&,   const Assignment&)
  bool EvaluateFormula(const Formula&,  const Assignment&)
```

**Evaluation rules:**

```cpp
EvaluateLiteral(L, A):
  value = A[L.name]             // look up variable's current assignment
  return L.is_negated ? !value : value

EvaluateClause(C, A):
  return any literal in C satisfies EvaluateLiteral

EvaluateFormula(F, A):
  return every clause in F satisfies EvaluateClause
```

**Partial assignment behaviour:** If a literal's variable is not present in `A`,
`EvaluateLiteral` returns `false` for unassigned variables rather than throwing.

---

### 4. `solver.hpp` / `solver.cpp`

**Role:** The two solving strategies. Both conform to the same result type.

```cpp
struct SolverResult {
    bool        is_satisfiable;
    Assignment  assignment;       // populated only when is_satisfiable == true
    uint64_t    steps;            // number of assignments explored (for comparison)
};
```

The solver is a class with a single public `solve(const Formula&, const VariableList&)`
method, making it easy to call from `main.cpp`.

```cpp
solver.hpp exports:
  struct         SolverResult
  class          BruteForceSolver  { SolverResult solve(Formula, VariableList) }
```

#### BruteForceSolver internals

Uses a bitmask loop over `[0, 2ⁿ)`. Bit `k` of the integer encodes the truth value for
variable `k`. For each integer, builds an `Assignment` and calls `EvaluateFormula`.
Returns on first success; exhausts all options before reporting UNSAT.

---

### 5. `main.cpp`

**Role:** CLI entry point. Orchestration only — no logic.

```d
1. Print banner
2. Instantiate Parser, call parse() → Formula + VariableList
3. Print the parsed formula in human-readable form
4. Run BruteForceSolver::solve() → print result + steps
5. Print the satisfying assignment (or UNSAT verdict)
```

`main.cpp` does not know how solving works. It only knows the public interfaces of
`Parser`, `Solver`, and the display helpers from `formula`.

---

## Control Flow

```dsconfig
main()
  │
  ├─► Parser::parse(std::cin)
  │     ├── Read line
  │     ├── Tokenize → Literals → Clause
  │     ├── Append Clause to Formula
  │     └── Repeat until "done"
  │         └── Returns: Formula, VariableList
  │
  ├─► ExtractVariables(Formula)   [if not already done by Parser]
  │
  ├─► BruteForceSolver::solve(Formula, Variables)
  │     └── For i in [0, 2ⁿ):
  │           build Assignment from bitmask i
  │           EvaluateFormula(Formula, Assignment) → true? return SAT
  │         → return UNSAT
  │
  └─► Display results
```

---

## Algorithm Walkthroughs

### Brute-Force Enumeration

Given: `(P ∨ Q) ∧ (¬P ∨ R) ∧ (¬Q ∨ ¬R)`,  Variables: `[P, Q, R]`

```mathematica
i=0 (000): P=F Q=F R=F  →  (F∨F)=F  →  FAIL
i=1 (001): P=F Q=F R=T  →  (F∨F)=F  →  FAIL
i=2 (010): P=F Q=T R=F  →  (F∨T)=T, (T∨F)=T, (F∨T)=T  →  SAT ✓
```

Returns after 3 iterations (out of 8 possible).

---

## Design Decisions

### Free functions for Evaluator, class for Solver

`Evaluator` functions carry no state — they are mathematical functions. Free functions
(rather than a class with a single method) make this explicit and allow the compiler to
inline them more aggressively. `Solver` classes carry mutable state (`steps_` counter),
which justifies the class boundary.

### `std::unordered_map` over `std::map` for Assignment

Evaluation calls `assignment[var]` for every literal in every clause, potentially
millions of times in brute-force mode. Hash-map O(1) average lookup outperforms
tree-map O(log n) at this call frequency.

### `VariableList` is ordered (`std::vector`)

The brute-force solver maps bit positions to variables. The order must be stable and
deterministic across calls. `std::vector` preserves insertion order; `std::unordered_set`
does not.

### No global state

Every component receives its inputs as parameters and returns outputs as values. There
are no global variables. This makes unit-testing individual components straightforward
without any setup or teardown.

---

## Known Limitations and Extensions

| Limitation                      | Possible Extension                                                      |
| ------------------------------- | ----------------------------------------------------------------------- |
| Interactive input only          | Add DIMACS `.cnf` file parser (standard benchmark format)               |
| No unit tests framework         | Integrate Catch2 or GoogleTest; add `make test` target                  |
| No heuristic variable selection | Add VSIDS (Variable State Independent Decaying Sum) heuristic           |
| Brute-force limited to ~20 vars | Expected; academic scope only                                           |
| No clause learning              | Add conflict-driven clause learning (CDCL) for production-grade solving |
