#ifndef LOGIC_SOLVER_H
#define LOGIC_SOLVER_H

#include "proof_tree.h"

namespace logic {

enum class SolveStatus {
  Provable,
  Unprovable,
  Unknown
};

struct SolveResult {
  SolveStatus status;
  ProofTree::Ptr proof;

  static SolveResult Provable(const ProofTree::Ptr& proof_tree) {
    return SolveResult{SolveStatus::Provable, proof_tree};
  }
  static SolveResult Unprovable() {
    return SolveResult{SolveStatus::Unprovable, nullptr};
  }
  static SolveResult Unknown() {
    return SolveResult{SolveStatus::Unknown, nullptr};
  }
};

SolveResult Solve(int step, const Sequent& sequent);
bool IsAxiom(const Sequent& sequent);

}  // namespace logic

#endif  // LOGIC_SOLVER_H
