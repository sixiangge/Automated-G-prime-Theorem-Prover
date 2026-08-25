#include "sequent.h"

namespace logic {

static std::string ListToString(const std::vector<Prop>& props, size_t index) {
  if (index >= props.size()) {
    return "";
  }
  std::string current = PropToString(props[index]);
  if (index + 1 >= props.size()) {
    return current;
  }
  return current + ", " + ListToString(props, index + 1);
}

std::string SequentToString(const Sequent& sequent) {
  std::string left = ListToString(sequent.left, 0);
  std::string right = ListToString(sequent.right, 0);
  return left + " |- " + right;
}

}  // namespace logic
