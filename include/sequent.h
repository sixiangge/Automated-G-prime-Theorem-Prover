#ifndef LOGIC_SEQUENT_H
#define LOGIC_SEQUENT_H

#include <string>
#include <vector>

#include "prop.h"

namespace logic {

struct Sequent {
  std::vector<Prop> left;
  std::vector<Prop> right;
};

std::string SequentToString(const Sequent& sequent);

}  // namespace logic

#endif  // LOGIC_SEQUENT_H
