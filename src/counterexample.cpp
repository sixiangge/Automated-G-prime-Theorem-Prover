#include "counterexample.h"

namespace logic {

constexpr size_t kMaxVarsForBruteforce = 20;

static bool ContainsVarRec(const std::vector<int>& vars, int value, size_t index) {
  if (index >= vars.size()) {
    return false;
  }
  if (vars[index] == value) {
    return true;
  }
  return ContainsVarRec(vars, value, index + 1);
}

static std::vector<int> InsertUnique(const std::vector<int>& vars, int value) {
  if (ContainsVarRec(vars, value, 0)) {
    return vars;
  }
  std::vector<int> result = vars;
  result.push_back(value);
  return result;
}

static std::vector<int> CollectAtomsProp(const Prop& prop, const std::vector<int>& vars) {
  switch (prop.kind()) {
    case Prop::Kind::Atom:
      return InsertUnique(vars, prop.atomId());
    case Prop::Kind::Not:
      return CollectAtomsProp(prop.left(), vars);
    case Prop::Kind::And: {
      std::vector<int> left_vars = CollectAtomsProp(prop.left(), vars);
      return CollectAtomsProp(prop.right(), left_vars);
    }
    case Prop::Kind::Or: {
      std::vector<int> left_vars = CollectAtomsProp(prop.left(), vars);
      return CollectAtomsProp(prop.right(), left_vars);
    }
    case Prop::Kind::Imply: {
      std::vector<int> left_vars = CollectAtomsProp(prop.left(), vars);
      return CollectAtomsProp(prop.right(), left_vars);
    }
  }
  return vars;
}

static std::vector<int> CollectAtomsList(const std::vector<Prop>& props, size_t index,
                                         const std::vector<int>& vars) {
  if (index >= props.size()) {
    return vars;
  }
  std::vector<int> next_vars = CollectAtomsProp(props[index], vars);
  return CollectAtomsList(props, index + 1, next_vars);
}

std::vector<int> CollectAtoms(const Sequent& sequent) {
  std::vector<int> vars = CollectAtomsList(sequent.left, 0, {});
  return CollectAtomsList(sequent.right, 0, vars);
}

bool EvalProp(const Prop& prop, const Valuation& valuation) {
  switch (prop.kind()) {
    case Prop::Kind::Atom: {
      auto it = valuation.find(prop.atomId());
      if (it == valuation.end()) {
        return false;
      }
      return it->second;
    }
    case Prop::Kind::Not:
      return !EvalProp(prop.left(), valuation);
    case Prop::Kind::And:
      return EvalProp(prop.left(), valuation) && EvalProp(prop.right(), valuation);
    case Prop::Kind::Or:
      return EvalProp(prop.left(), valuation) || EvalProp(prop.right(), valuation);
    case Prop::Kind::Imply:
      return !EvalProp(prop.left(), valuation) || EvalProp(prop.right(), valuation);
  }
  return false;
}

static bool EvalAllTrueRec(const std::vector<Prop>& props, size_t index,
                           const Valuation& valuation) {
  if (index >= props.size()) {
    return true;
  }
  if (!EvalProp(props[index], valuation)) {
    return false;
  }
  return EvalAllTrueRec(props, index + 1, valuation);
}

static bool EvalAllFalseRec(const std::vector<Prop>& props, size_t index,
                            const Valuation& valuation) {
  if (index >= props.size()) {
    return true;
  }
  if (EvalProp(props[index], valuation)) {
    return false;
  }
  return EvalAllFalseRec(props, index + 1, valuation);
}

bool EvalAllTrue(const std::vector<Prop>& props, const Valuation& valuation) {
  return EvalAllTrueRec(props, 0, valuation);
}

bool EvalAllFalse(const std::vector<Prop>& props, const Valuation& valuation) {
  return EvalAllFalseRec(props, 0, valuation);
}

static Valuation SetValue(const Valuation& valuation, int var, bool value) {
  Valuation next = valuation;
  next[var] = value;
  return next;
}

static std::optional<Valuation> EnumerateValuations(const std::vector<int>& vars,
                                                    size_t index,
                                                    const Sequent& sequent,
                                                    const Valuation& current) {
  if (index >= vars.size()) {
    if (EvalAllTrue(sequent.left, current) && EvalAllFalse(sequent.right, current)) {
      return current;
    }
    return std::nullopt;
  }

  int var = vars[index];
  auto with_false = EnumerateValuations(vars, index + 1, sequent,
                                       SetValue(current, var, false));
  if (with_false.has_value()) {
    return with_false;
  }
  return EnumerateValuations(vars, index + 1, sequent, SetValue(current, var, true));
}

std::optional<Valuation> FindCounterexample(const Sequent& sequent) {
  std::vector<int> vars = CollectAtoms(sequent);
  if (vars.size() > kMaxVarsForBruteforce) {
    return std::nullopt;
  }
  return EnumerateValuations(vars, 0, sequent, Valuation{});
}

}  // namespace logic
