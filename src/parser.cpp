#include "parser.h"

#include <cctype>

namespace logic {

Parser::Parser(const std::string& input) : input_(input), index_(0) {}

Sequent Parser::Parse(const std::string& input) {
  Parser parser(input);
  return parser.ParseSequent();
}

Sequent Parser::ParseSequent() {
  Sequent sequent = ParseSequentInternal();
  SkipSpaces();
  if (!End()) {
    Fail("Unexpected trailing input");
  }
  return sequent;
}

Sequent Parser::ParseSequentInternal() {
  Sequent sequent;
  sequent.left = ParsePropListUntil("|-", true);
  SkipSpaces();
  if (!Match("|-")) {
    Fail("Expected '|-' in sequent");
  }
  sequent.right = ParsePropListUntil("", true);
  return sequent;
}

static std::vector<Prop> PrependProp(const Prop& head, std::vector<Prop> tail) {
  tail.insert(tail.begin(), head);
  return tail;
}

std::vector<Prop> Parser::ParsePropListUntil(const std::string& stop_token,
                                             bool stop_on_eof) {
  SkipSpaces();
  if (!stop_token.empty() && StartsWith(stop_token)) {
    return {};
  }
  if (stop_on_eof && End()) {
    return {};
  }

  Prop first = ParseProp();
  SkipSpaces();
  if (MatchChar(',')) {
    return PrependProp(first, ParsePropListUntil(stop_token, stop_on_eof));
  }
  return {first};
}

Prop Parser::ParseProp() {
  return ParseImply();
}

Prop Parser::ParseImply() {
  Prop left = ParseOr();
  SkipSpaces();
  if (Match("->")) {
    Prop right = ParseImply();
    return Prop::MakeImply(left, right);
  }
  return left;
}

Prop Parser::ParseOr() {
  Prop left = ParseAnd();
  SkipSpaces();
  if (StartsWith("|-")) {
    return left;
  }
  if (MatchChar('|')) {
    Prop right = ParseOr();
    return Prop::MakeOr(left, right);
  }
  return left;
}

Prop Parser::ParseAnd() {
  Prop left = ParseNot();
  SkipSpaces();
  if (MatchChar('&')) {
    Prop right = ParseAnd();
    return Prop::MakeAnd(left, right);
  }
  return left;
}

Prop Parser::ParseNot() {
  SkipSpaces();
  if (MatchChar('~')) {
    return Prop::MakeNot(ParseNot());
  }
  return ParseAtomOrParen();
}

Prop Parser::ParseAtomOrParen() {
  SkipSpaces();
  if (MatchChar('(')) {
    Prop inner = ParseProp();
    SkipSpaces();
    if (!MatchChar(')')) {
      Fail("Expected ')' after sub-expression");
    }
    return inner;
  }
  int value = ParseNumber();
  return Prop::MakeAtom(value);
}

int Parser::ParseNumber() {
  SkipSpaces();
  if (End() || !std::isdigit(static_cast<unsigned char>(input_[index_]))) {
    Fail("Expected number");
  }
  int first = input_[index_] - '0';
  index_ += 1;
  return ParseNumberRec(first);
}

int Parser::ParseNumberRec(int value) {
  if (End() || !std::isdigit(static_cast<unsigned char>(input_[index_]))) {
    return value;
  }
  int digit = input_[index_] - '0';
  index_ += 1;
  int next = value * 10 + digit;
  return ParseNumberRec(next);
}

void Parser::SkipSpaces() {
  if (End()) {
    return;
  }
  if (std::isspace(static_cast<unsigned char>(input_[index_]))) {
    index_ += 1;
    SkipSpaces();
  }
}

bool Parser::End() const {
  return index_ >= input_.size();
}

bool Parser::StartsWith(const std::string& token) const {
  if (token.empty()) {
    return false;
  }
  if (index_ + token.size() > input_.size()) {
    return false;
  }
  for (size_t i = 0; i < token.size(); ++i) {
    if (input_[index_ + i] != token[i]) {
      return false;
    }
  }
  return true;
}

bool Parser::Match(const std::string& token) {
  SkipSpaces();
  if (!StartsWith(token)) {
    return false;
  }
  index_ += token.size();
  return true;
}

bool Parser::MatchChar(char c) {
  SkipSpaces();
  if (End() || input_[index_] != c) {
    return false;
  }
  index_ += 1;
  return true;
}

[[noreturn]] void Parser::Fail(const std::string& message) const {
  throw ParseError(message);
}

}  // namespace logic
