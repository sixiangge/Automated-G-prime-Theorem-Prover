#include <iostream>
#include <string>

#include "counterexample.h"
#include "parser.h"
#include "proof_tree.h"
#include "solver.h"

namespace {

constexpr int kDefaultStep = 100;

void PrintValuation(const logic::Valuation& valuation) {
  std::cout << "{";
  bool first = true;
  for (const auto& kv : valuation) {
    if (!first) {
      std::cout << ", ";
    }
    std::cout << kv.first << ": " << (kv.second ? "true" : "false");
    first = false;
  }
  std::cout << "}";
}

}  // namespace

int main() {
  using namespace logic;

  std::string line;
  if (!std::getline(std::cin, line)) {
    std::cerr << "No input provided.\n";
    return 1;
  }

  try {
    Sequent sequent = Parser::Parse(line);
    SolveResult result = Solve(kDefaultStep, sequent);

    if (result.status == SolveStatus::Provable) {
      std::cout << "Provable\n";
      std::cout << ProofTreeToString(*result.proof);
      return 0;
    }

    if (result.status == SolveStatus::Unprovable) {
      std::cout << "Unprovable\n";
      auto counter = FindCounterexample(sequent);
      if (counter.has_value()) {
        std::cout << "Counterexample: ";
        PrintValuation(*counter);
        std::cout << "\n";
      } else {
        std::cout << "Counterexample: unknown\n";
      }
      return 0;
    }

    std::cout << "Unknown (step exhausted)\n";
    return 0;
  } catch (const ParseError& e) {
    std::cerr << "Parse error: " << e.message() << "\n";
    return 1;
  }
}
