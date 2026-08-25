#include "prop.h"

namespace logic {

struct Prop::Node {
  Kind kind;
  int atom_id;
  std::shared_ptr<const Node> left;
  std::shared_ptr<const Node> right;
};

Prop::Prop(std::shared_ptr<const Node> node) : node_(std::move(node)) {}

Prop Prop::MakeAtom(int id) {
  return Prop(std::make_shared<Node>(Node{Kind::Atom, id, nullptr, nullptr}));
}

Prop Prop::MakeNot(const Prop& value) {
  return Prop(std::make_shared<Node>(Node{Kind::Not, 0, value.node_, nullptr}));
}

Prop Prop::MakeAnd(const Prop& left, const Prop& right) {
  return Prop(std::make_shared<Node>(Node{Kind::And, 0, left.node_, right.node_}));
}

Prop Prop::MakeOr(const Prop& left, const Prop& right) {
  return Prop(std::make_shared<Node>(Node{Kind::Or, 0, left.node_, right.node_}));
}

Prop Prop::MakeImply(const Prop& left, const Prop& right) {
  return Prop(std::make_shared<Node>(Node{Kind::Imply, 0, left.node_, right.node_}));
}

Prop::Kind Prop::kind() const {
  return node_->kind;
}

int Prop::atomId() const {
  return node_->atom_id;
}

Prop Prop::left() const {
  return Prop(node_->left);
}

Prop Prop::right() const {
  return Prop(node_->right);
}

bool Prop::EqualsNode(const std::shared_ptr<const Prop::Node>& left,
                      const std::shared_ptr<const Prop::Node>& right) {
  if (left.get() == right.get()) {
    return true;
  }
  if (!left || !right) {
    return false;
  }
  if (left->kind != right->kind) {
    return false;
  }
  if (left->kind == Prop::Kind::Atom) {
    return left->atom_id == right->atom_id;
  }
  if (left->kind == Prop::Kind::Not) {
    return EqualsNode(left->left, right->left);
  }
  return EqualsNode(left->left, right->left) &&
         EqualsNode(left->right, right->right);
}

bool Prop::Equals(const Prop& other) const {
  return EqualsNode(node_, other.node_);
}

std::string Prop::ToStringNode(const std::shared_ptr<const Prop::Node>& node) {
  if (!node) {
    return "";
  }
  switch (node->kind) {
    case Prop::Kind::Atom: {
      return std::to_string(node->atom_id);
    }
    case Prop::Kind::Not: {
      return "~" + ToStringNode(node->left);
    }
    case Prop::Kind::And: {
      return "(" + ToStringNode(node->left) + "&" +
             ToStringNode(node->right) + ")";
    }
    case Prop::Kind::Or: {
      return "(" + ToStringNode(node->left) + "|" +
             ToStringNode(node->right) + ")";
    }
    case Prop::Kind::Imply: {
      return "(" + ToStringNode(node->left) + "->" +
             ToStringNode(node->right) + ")";
    }
  }
  return "";
}

std::string Prop::ToString() const {
  return ToStringNode(node_);
}

bool PropEquals(const Prop& left, const Prop& right) {
  return left.Equals(right);
}

std::string PropToString(const Prop& prop) {
  return prop.ToString();
}

}  // namespace logic
