#include <iostream>
#include <string>

#include "counterexample.h"
#include "parser.h"
#include "solver.h"

namespace {

int failures = 0;

void Check(bool condition, const std::string& name) {
  if (condition) {
    std::cout << "[PASS] " << name << "\n";
  } else {
    std::cout << "[FAIL] " << name << "\n";
    failures += 1;
  }
}

}  // namespace

int main() {
  using namespace logic;

  try {
    {
      Sequent s = Parser::Parse("0 |- 0");
      SolveResult r = Solve(100, s);
      Check(r.status == SolveStatus::Provable, "Case1 Provable");
    }
    {
      Sequent s = Parser::Parse("(0->1), 0 |- 1");
      SolveResult r = Solve(100, s);
      Check(r.status == SolveStatus::Provable, "Case2 Provable");
    }
    {
      Sequent s = Parser::Parse("~(0|1) |- (~0 & ~1)");
      SolveResult r = Solve(100, s);
      Check(r.status == SolveStatus::Provable, "Case3 Provable");
    }
    {
      Sequent s = Parser::Parse("|- ((0->1)->0)->0");
      SolveResult r = Solve(100, s);
      Check(r.status == SolveStatus::Provable, "Case4 Provable");
    }
    {
      Sequent s = Parser::Parse("(0|1) |- (0&1)");
      SolveResult r = Solve(100, s);
      Check(r.status == SolveStatus::Unprovable, "Case5 Unprovable");

      auto counter = FindCounterexample(s);
      bool valid = false;
      if (counter.has_value()) {
        valid = EvalAllTrue(s.left, *counter) && EvalAllFalse(s.right, *counter);
      }
      Check(valid, "Case5 Counterexample");
    }
  } catch (const ParseError& e) {
    std::cout << "[FAIL] Parse error: " << e.message() << "\n";
    return 1;
  }

  if (failures == 0) {
    std::cout << "All tests passed.\n";
    return 0;
  }
  std::cout << failures << " test(s) failed.\n";
  return 1;
}
