#ifndef LOGIC_PROP_H
#define LOGIC_PROP_H

#include <memory>
#include <string>

namespace logic {

class Prop {
 public:
  enum class Kind {
    Atom,
    Not,
    And,
    Or,
    Imply
  };

  static Prop MakeAtom(int id);
  static Prop MakeNot(const Prop& value);
  static Prop MakeAnd(const Prop& left, const Prop& right);
  static Prop MakeOr(const Prop& left, const Prop& right);
  static Prop MakeImply(const Prop& left, const Prop& right);

  Kind kind() const;
  int atomId() const;
  Prop left() const;
  Prop right() const;
  bool Equals(const Prop& other) const;
  std::string ToString() const;

 private:
  struct Node;
  explicit Prop(std::shared_ptr<const Node> node);

  static bool EqualsNode(const std::shared_ptr<const Node>& left,
                         const std::shared_ptr<const Node>& right);
  static std::string ToStringNode(const std::shared_ptr<const Node>& node);

  std::shared_ptr<const Node> node_;
};

bool PropEquals(const Prop& left, const Prop& right);
std::string PropToString(const Prop& prop);

}  // namespace logic

#endif  // LOGIC_PROP_H
