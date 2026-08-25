#include "solver.h"

namespace logic {

static SolveResult CombineResults(const SolveResult& left, const SolveResult& right) {
  if (left.status == SolveStatus::Provable) {
    return left;
  }
  if (right.status == SolveStatus::Provable) {
    return right;
  }
  if (left.status == SolveStatus::Unknown || right.status == SolveStatus::Unknown) {
    return SolveResult::Unknown();
  }
  return SolveResult::Unprovable();
}

static std::vector<Prop> RemoveAtRec(const std::vector<Prop>& list, size_t index, size_t pos) {
  if (pos >= list.size()) {
    return {};
  }
  std::vector<Prop> rest = RemoveAtRec(list, index, pos + 1);
  if (pos == index) {
    return rest;
  }
  rest.insert(rest.begin(), list[pos]);
  return rest;
}

static std::vector<Prop> RemoveAt(const std::vector<Prop>& list, size_t index) {
  return RemoveAtRec(list, index, 0);
}

static std::vector<Prop> AppendProp(const std::vector<Prop>& list, const Prop& prop) {
  std::vector<Prop> result = list;
  result.push_back(prop);
  return result;
}

static std::vector<Prop> AppendTwo(const std::vector<Prop>& list, const Prop& first,
                                   const Prop& second) {
  std::vector<Prop> result = list;
  result.push_back(first);
  result.push_back(second);
  return result;
}

static bool ContainsRec(const std::vector<Prop>& list, const Prop& target, size_t index) {
  if (index >= list.size()) {
    return false;
  }
  if (PropEquals(list[index], target)) {
    return true;
  }
  return ContainsRec(list, target, index + 1);
}

static bool AxiomRec(const std::vector<Prop>& left, const std::vector<Prop>& right,
                     size_t index) {
  if (index >= left.size()) {
    return false;
  }
  if (ContainsRec(right, left[index], 0)) {
    return true;
  }
  return AxiomRec(left, right, index + 1);
}

bool IsAxiom(const Sequent& sequent) {
  return AxiomRec(sequent.left, sequent.right, 0);
}

static SolveResult TryNotL(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.left.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.left[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Not) {
    Sequent child;
    child.left = RemoveAt(sequent.left, index);
    child.right = AppendProp(sequent.right, prop.left());
    SolveResult child_result = Solve(step - 1, child);
    if (child_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeUnary(Rule::Not_L, sequent,
                                                           child_result.proof));
    } else if (child_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryNotL(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryNotR(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.right.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.right[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Not) {
    Sequent child;
    child.left = AppendProp(sequent.left, prop.left());
    child.right = RemoveAt(sequent.right, index);
    SolveResult child_result = Solve(step - 1, child);
    if (child_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeUnary(Rule::Not_R, sequent,
                                                           child_result.proof));
    } else if (child_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryNotR(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryAndL(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.left.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.left[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::And) {
    Sequent child;
    child.left = AppendTwo(RemoveAt(sequent.left, index), prop.left(), prop.right());
    child.right = sequent.right;
    SolveResult child_result = Solve(step - 1, child);
    if (child_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeUnary(Rule::And_L, sequent,
                                                           child_result.proof));
    } else if (child_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryAndL(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryAndR(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.right.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.right[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::And) {
    std::vector<Prop> base_right = RemoveAt(sequent.right, index);
    Sequent left_child{sequent.left, AppendProp(base_right, prop.left())};
    Sequent right_child{sequent.left, AppendProp(base_right, prop.right())};
    SolveResult left_result = Solve(step - 1, left_child);
    SolveResult right_result = Solve(step - 1, right_child);
    if (left_result.status == SolveStatus::Provable &&
        right_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeBinary(
          Rule::And_R, sequent, left_result.proof, right_result.proof));
    } else if (left_result.status == SolveStatus::Unknown ||
               right_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryAndR(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryOrL(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.left.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.left[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Or) {
    std::vector<Prop> base_left = RemoveAt(sequent.left, index);
    Sequent left_child{AppendProp(base_left, prop.left()), sequent.right};
    Sequent right_child{AppendProp(base_left, prop.right()), sequent.right};
    SolveResult left_result = Solve(step - 1, left_child);
    SolveResult right_result = Solve(step - 1, right_child);
    if (left_result.status == SolveStatus::Provable &&
        right_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeBinary(
          Rule::Or_L, sequent, left_result.proof, right_result.proof));
    } else if (left_result.status == SolveStatus::Unknown ||
               right_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryOrL(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryOrR(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.right.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.right[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Or) {
    Sequent child;
    child.left = sequent.left;
    child.right = AppendTwo(RemoveAt(sequent.right, index), prop.left(), prop.right());
    SolveResult child_result = Solve(step - 1, child);
    if (child_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeUnary(Rule::Or_R, sequent,
                                                           child_result.proof));
    } else if (child_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryOrR(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryImplyL(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.left.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.left[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Imply) {
    std::vector<Prop> base_left = RemoveAt(sequent.left, index);
    Sequent left_child{base_left, AppendProp(sequent.right, prop.left())};
    Sequent right_child{AppendProp(base_left, prop.right()), sequent.right};
    SolveResult left_result = Solve(step - 1, left_child);
    SolveResult right_result = Solve(step - 1, right_child);
    if (left_result.status == SolveStatus::Provable &&
        right_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeBinary(
          Rule::Imply_L, sequent, left_result.proof, right_result.proof));
    } else if (left_result.status == SolveStatus::Unknown ||
               right_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryImplyL(step, sequent, index + 1);
  return CombineResults(current, next);
}

static SolveResult TryImplyR(int step, const Sequent& sequent, size_t index) {
  if (index >= sequent.right.size()) {
    return SolveResult::Unprovable();
  }
  const Prop& prop = sequent.right[index];
  SolveResult current = SolveResult::Unprovable();
  if (prop.kind() == Prop::Kind::Imply) {
    Sequent child;
    child.left = AppendProp(sequent.left, prop.left());
    child.right = AppendProp(RemoveAt(sequent.right, index), prop.right());
    SolveResult child_result = Solve(step - 1, child);
    if (child_result.status == SolveStatus::Provable) {
      current = SolveResult::Provable(ProofTree::MakeUnary(Rule::Imply_R, sequent,
                                                           child_result.proof));
    } else if (child_result.status == SolveStatus::Unknown) {
      current = SolveResult::Unknown();
    }
  }
  SolveResult next = TryImplyR(step, sequent, index + 1);
  return CombineResults(current, next);
}

SolveResult Solve(int step, const Sequent& sequent) {
  if (step <= 0) {
    return SolveResult::Unknown();
  }
  if (IsAxiom(sequent)) {
    return SolveResult::Provable(ProofTree::MakeLeaf(Rule::Ax, sequent));
  }

  SolveResult best = SolveResult::Unprovable();

  SolveResult res = TryNotL(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryNotR(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryAndL(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryAndR(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryOrL(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryOrR(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryImplyL(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  res = TryImplyR(step, sequent, 0);
  if (res.status == SolveStatus::Provable) {
    return res;
  }
  if (res.status == SolveStatus::Unknown) {
    best = SolveResult::Unknown();
  }

  return best;
}

}  // namespace logic
