# ──────────────────────────────────────────────────────────────────────────────
# SAT Solver for Logic Problems Using Discrete Mathematics
# Discrete Mathematics Course Project — C++23
# ──────────────────────────────────────────────────────────────────────────────

# On Windows: prepend the compiler's bin directory to PATH to avoid DLL conflicts
# with Git Bash or MSYS2 when an outdated runtime is found first.
ifeq ($(OS),Windows_NT)
  COMPILER_PATH := $(shell cygpath -w $$(which g++) 2>/dev/null)
  ifneq ($(COMPILER_PATH),)
    export PATH := $(dir $(COMPILER_PATH));$(PATH)
  endif
endif

# ──────────────────────────────────────────────────────────────────────────────
# Toolchain:
# ──────────
CXX      = g++
CXXFLAGS = -Wall -Wextra -Wpedantic -std=c++23 -g -Iinclude
DEPFLAGS = -MMD -MP -MF $(@:.o=.d)

# ──────────────────────────────────────────────────────────────────────────────
# Sources:
# ────────
# All translation units, excluding the entry point.
# Adding a new .cpp file only requires an entry here.
LIB_SRC = source/formula.cpp    \
           source/parser.cpp     \
           source/evaluator.cpp  \
           source/solver.cpp

MAIN_SRC = source/main.cpp
ALL_SRC  = $(LIB_SRC) $(MAIN_SRC)

# ──────────────────────────────────────────────────────────────────────────────
# Object and Dependency Files:
# ────────────────────────────
LIB_OBJ  = $(LIB_SRC:source/%.cpp=build/obj/%.o)
MAIN_OBJ = build/obj/main.o
ALL_OBJ  = $(LIB_OBJ) $(MAIN_OBJ)
DEP      = $(ALL_OBJ:.o=.d)

# ──────────────────────────────────────────────────────────────────────────────
# Output Binary and Platform Detection:
# ─────────────────────────────────────
ifeq ($(OS),Windows_NT)
  TARGET = build/bin/sat_solver.exe
  # Detect whether we are running under MINGW/MSYS (Unix-like shell) or cmd/PS.
  ifneq (,$(findstring MINGW,$(shell uname -s 2>/dev/null)))
    MKDIR_P = mkdir -p
    RM_RF   = rm -rf
  else
    MKDIR_P = powershell -Command "New-Item -ItemType Directory -Force -Path"
    RM_RF   = powershell -Command "Remove-Item -Recurse -Force -ErrorAction SilentlyContinue"
  endif
else
  TARGET  = build/bin/sat_solver
  MKDIR_P = mkdir -p
  RM_RF   = rm -rf
endif

# ──────────────────────────────────────────────────────────────────────────────
# All source files (headers included) for format/tidy targets:
# ────────────────────────────────────────────────────────────
CPP_FILES = $(ALL_SRC)
HDR_FILES = $(wildcard include/*.hpp)
ALL_FILES = $(CPP_FILES) $(HDR_FILES)

# ──────────────────────────────────────────────────────────────────────────────
# Phony Targets:
# ──────────────
.PHONY: all build run lint format clean docs help
.DELETE_ON_ERROR:

# ──────────────────────────────────────────────────────────────────────────────
# Default Goal:
# ─────────────
all: $(TARGET)

help:
	@echo ""
	@echo "  SAT Solver — Build System"
	@echo "  ─────────────────────────────────────────"
	@echo "  make build     Build the sat_solver binary"
	@echo "  make run       Build and run the solver interactively"
	@echo "  make lint      Run cpplint, cppcheck, and clang-tidy"
	@echo "  make format    Auto-format all C++ source files"
	@echo "  make docs      Generate Doxygen HTML documentation"
	@echo "  make clean     Remove all build artifacts and docs"
	@echo ""

build: $(TARGET)
	@echo "[+] Build complete: $(TARGET)"

# ──────────────────────────────────────────────────────────────────────────────
# Linking:
# ────────
$(TARGET): $(ALL_OBJ) | build/bin
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "[+] Linked: $@"

# ──────────────────────────────────────────────────────────────────────────────
# Compilation:
# ────────────
build/obj/%.o: source/%.cpp | build/obj
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) -c $< -o $@

# Automatic header dependency tracking.
# Note: on virtual/mapped drives (e.g. G:) or some MSYS2/Windows setups,
# -MMD can cause silent Error 1. Disable DEPFLAGS above if that occurs.
-include $(DEP)

# ──────────────────────────────────────────────────────────────────────────────
# Directory Creation:
# ───────────────────
build/bin:
	@$(MKDIR_P) build/bin

build/obj:
	@$(MKDIR_P) build/obj

# ──────────────────────────────────────────────────────────────────────────────
# Run:
# ────
run: $(TARGET)
	@echo "Starting SAT Solver..."
	@echo "-----------------------"
	./$(TARGET)

# ──────────────────────────────────────────────────────────────────────────────
# Static Analysis & Linting:
# ──────────────────────────
lint:
	@echo "[1/3] Running cpplint..."
	cpplint --recursive source/ include/
	@echo ""
	@echo "[2/3] Running cppcheck..."
	cppcheck --enable=all                    \
	         --inconclusive                  \
	         --std=c++23                     \
	         --language=c++                  \
	         --suppress=checkersReport       \
	         --suppress=missingIncludeSystem \
	         --suppress=normalCheckLevelMaxBranches \
	         --suppress=unusedFunction       \
	         --error-exitcode=1              \
	         -Iinclude --inline-suppr        \
	         source/ include/
	@echo ""
	@echo "[3/3] Running clang-tidy..."
	clang-tidy $(CPP_FILES) -- $(CXXFLAGS)
	@echo ""
	@echo "[+] Lint complete."

# ──────────────────────────────────────────────────────────────────────────────
# Auto-Formatting:
# ────────────────
format:
	@echo "Formatting all C++ files..."
	clang-format -i $(ALL_FILES)
	@echo "[+] Format complete."

# ──────────────────────────────────────────────────────────────────────────────
# Documentation:
# ──────────────
docs:
	@echo "Generating Doxygen documentation..."
	doxygen Doxyfile
	@echo "[+] Docs generated in docs/html/index.html"

# ──────────────────────────────────────────────────────────────────────────────
# Cleanup:
# ────────
clean:
	@echo "Cleaning build artifacts..."
	@$(RM_RF) build
	@echo "Cleaning generated documentation..."
	@$(RM_RF) docs/html docs/latex docs/doxygen-warnings.log
	@echo "[+] Clean complete."