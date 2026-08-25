#include "proof_tree.h"

namespace logic {

ProofTree::ProofTree(Rule rule, const Sequent& sequent, std::vector<Ptr> children)
    : rule_(rule), sequent_(sequent), children_(std::move(children)) {}

ProofTree::Ptr ProofTree::MakeLeaf(Rule rule, const Sequent& sequent) {
  return Ptr(new ProofTree(rule, sequent, std::vector<Ptr>{}));
}

ProofTree::Ptr ProofTree::MakeUnary(Rule rule, const Sequent& sequent, const Ptr& child) {
  return Ptr(new ProofTree(rule, sequent, std::vector<Ptr>{child}));
}

ProofTree::Ptr ProofTree::MakeBinary(Rule rule, const Sequent& sequent,
                                     const Ptr& left, const Ptr& right) {
  return Ptr(new ProofTree(rule, sequent, std::vector<Ptr>{left, right}));
}

Rule ProofTree::rule() const {
  return rule_;
}

const Sequent& ProofTree::sequent() const {
  return sequent_;
}

const std::vector<ProofTree::Ptr>& ProofTree::children() const {
  return children_;
}

static std::string RuleToString(Rule rule) {
  switch (rule) {
    case Rule::Ax:
      return "Ax";
    case Rule::Not_L:
      return "Not_L";
    case Rule::Not_R:
      return "Not_R";
    case Rule::And_L:
      return "And_L";
    case Rule::And_R:
      return "And_R";
    case Rule::Or_L:
      return "Or_L";
    case Rule::Or_R:
      return "Or_R";
    case Rule::Imply_L:
      return "Imply_L";
    case Rule::Imply_R:
      return "Imply_R";
  }
  return "";
}

static void AppendTreeLines(const ProofTree& tree, const std::string& indent,
                            std::string* out) {
  *out += indent + RuleToString(tree.rule()) + ": " + SequentToString(tree.sequent()) + "\n";
  const auto& kids = tree.children();
  if (kids.empty()) {
    return;
  }
  auto appendChildAt = [&](size_t index, const auto& self) -> void {
    if (index >= kids.size()) {
      return;
    }
    AppendTreeLines(*kids[index], indent + "  ", out);
    self(index + 1, self);
  };
  appendChildAt(0, appendChildAt);
}

std::string ProofTreeToString(const ProofTree& tree) {
  std::string result;
  AppendTreeLines(tree, "", &result);
  return result;
}

}  // namespace logic
