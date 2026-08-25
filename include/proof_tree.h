#ifndef LOGIC_PROOF_TREE_H
#define LOGIC_PROOF_TREE_H

#include <memory>
#include <string>
#include <vector>

#include "sequent.h"

namespace logic {

enum class Rule {
  Ax,
  Not_L,
  Not_R,
  And_L,
  And_R,
  Or_L,
  Or_R,
  Imply_L,
  Imply_R
};

class ProofTree {
 public:
  using Ptr = std::shared_ptr<const ProofTree>;

  static Ptr MakeLeaf(Rule rule, const Sequent& sequent);
  static Ptr MakeUnary(Rule rule, const Sequent& sequent, const Ptr& child);
  static Ptr MakeBinary(Rule rule, const Sequent& sequent,
                        const Ptr& left, const Ptr& right);

  Rule rule() const;
  const Sequent& sequent() const;
  const std::vector<Ptr>& children() const;

 private:
  ProofTree(Rule rule, const Sequent& sequent, std::vector<Ptr> children);

  Rule rule_;
  Sequent sequent_;
  std::vector<Ptr> children_;
};

std::string ProofTreeToString(const ProofTree& tree);

}  // namespace logic

#endif  // LOGIC_PROOF_TREE_H
