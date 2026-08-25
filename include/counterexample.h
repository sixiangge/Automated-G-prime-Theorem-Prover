#ifndef LOGIC_COUNTEREXAMPLE_H
#define LOGIC_COUNTEREXAMPLE_H

#include <map>
#include <optional>
#include <vector>

#include "sequent.h"

namespace logic {

using Valuation = std::map<int, bool>;

std::vector<int> CollectAtoms(const Sequent& sequent);
bool EvalProp(const Prop& prop, const Valuation& valuation);
bool EvalAllTrue(const std::vector<Prop>& props, const Valuation& valuation);
bool EvalAllFalse(const std::vector<Prop>& props, const Valuation& valuation);
std::optional<Valuation> FindCounterexample(const Sequent& sequent);

}  // namespace logic

#endif  // LOGIC_COUNTEREXAMPLE_H
