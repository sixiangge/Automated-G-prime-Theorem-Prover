#ifndef LOGIC_PARSER_H
#define LOGIC_PARSER_H

#include <string>

#include "sequent.h"

namespace logic {

class ParseError {
 public:
  explicit ParseError(const std::string& message) : message_(message) {}
  const std::string& message() const { return message_; }

 private:
  std::string message_;
};

class Parser {
 public:
  explicit Parser(const std::string& input);
  Sequent ParseSequent();

  static Sequent Parse(const std::string& input);

 private:
  Sequent ParseSequentInternal();
  std::vector<Prop> ParsePropListUntil(const std::string& stop_token, bool stop_on_eof);

  Prop ParseProp();
  Prop ParseImply();
  Prop ParseOr();
  Prop ParseAnd();
  Prop ParseNot();
  Prop ParseAtomOrParen();

  int ParseNumber();
  int ParseNumberRec(int value);

  void SkipSpaces();
  bool End() const;
  bool StartsWith(const std::string& token) const;
  bool Match(const std::string& token);
  bool MatchChar(char c);

  [[noreturn]] void Fail(const std::string& message) const;

  std::string input_;
  size_t index_;
};

}  // namespace logic

#endif  // LOGIC_PARSER_H
