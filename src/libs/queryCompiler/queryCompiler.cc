#include "src/libs/queryCompiler/queryCompiler.h"

using BBG::ISR;
using BBG::ISRWord;
using BBG::ISROr;
using BBG::ISRAnd;
using BBG::QueryCompiler;
using BBG::string;
using BBG::vector;
using BBG::NaughtyFilter;

// Helpers
bool isLeftBracket(const string& str) {
  return str == "(" || str == "{";
}

bool isRightBracket(const string& str) {
  return str == ")" || str == "}";
}

bool isOr(const string& str) {
  return str == "OR" || str == "|" || str == "||";
}

bool isAnd(const string& str) {
  return str == "AND" || str == "&" || str == "&&";
}

bool isSearchTerm(const string& str) {
  return !(isLeftBracket(str) || isAnd(str) || isOr(str) || isRightBracket(str));
}

QueryCompiler::QueryCompiler() : naughtyFilter(false), stopwordsFilter(true) {}

QueryCompiler::QueryCompiler(IndexChunk* _index, const Map<string, uint32_t> *_bagOfWords) :
  index(_index), naughtyFilter(false), stopwordsFilter(true), bagOfWords(_bagOfWords) {}

bool QueryCompiler::pastQueryEnd() {
  if (pos >= query.size()) {
    log.Debug("[QueryCompiler] Not found - current position is past query end");
    return true;
  }
  return false;
}

ISR* QueryCompiler::findConstraint() {
  log.Debug("[QueryCompiler] Looking for Constraint");
  if (pastQueryEnd())
    return nullptr;
  
  ISR* left = findBaseConstraint();
  if (!left) {
    log.Debug("[QueryCompiler] Not found - unable to find Base Constraint");
    return nullptr;
  }

  ISROr* self = new ISROr();
  log.Debug("[QueryCompiler] Adding new Base Constraint to ISROr");
  self->addTerm(left);

  while (findOrOp()) {
    left = findBaseConstraint();
    if (left) {
      log.Debug("[QueryCompiler] Adding new Base Constraint to ISROr");
      self->addTerm(left);
    }
    else {
      log.Debug("[QueryCompiler] Not found - unable to find Base Constraint");
      return nullptr;
    }
  }
  return self;
}

bool QueryCompiler::findOrOp() {
  log.Debug("[QueryCompiler] Looking for Or Op");
  if (pastQueryEnd())
    return false;

  if (isOr(query[pos])) {
    ++pos;
    log.Debug("[QueryCompiler] Or Op found");
    return true;
  }
  log.Debug("[QueryCompiler] Or Op not found");
  return false;
}

ISR* QueryCompiler::findBaseConstraint() {
  log.Debug("[QueryCompiler] Looking for Base Constraint");
  if (pastQueryEnd())
    return nullptr;
  
  ISR* left = findSimpleConstraint();
  if (!left) {
    log.Debug("[QueryCompiler] Not found - unable to find Simple Constraint");
    return nullptr;
  }

  ISRAnd* self = new ISRAnd();
  log.Debug("[QueryCompiler] Adding new Simple Constraint to ISRAnd");
  self->addTerm(left);

  while (findAndOp()) {
    left = findSimpleConstraint();
    if (left) {
    log.Debug("[QueryCompiler] Adding new Simple Constraint to ISRAnd");
      self->addTerm(left);
    }
    else {
      log.Debug("[QueryCompiler] Not found - unable to find Simple Constraint");
      return nullptr;
    }
  }
  return self;
}

bool QueryCompiler::findAndOp() {
  log.Debug("[QueryCompiler] Looking for And Op");
  if (pastQueryEnd())
    return false;

  if (isAnd(query[pos])) {
    ++pos;
    return true;
  }

  return false;
}

ISR* QueryCompiler::findSimpleConstraint() {
  log.Debug("[QueryCompiler] Looking for Simple Constraint");
  if (pastQueryEnd())
    return nullptr;

  ISR* self = findPhrase();

  if (!self)
    self = findNestedConstraint();
  if (!self)
    self = findSearchWord();

  return self;
}

// We aren't doing phrases for now.
ISR* QueryCompiler::findPhrase() {
  log.Debug("[QueryCompiler] Looking for Phrase");
  return nullptr;
  // if (query[pos] != '"')
  //   return nullptr;

  // ISR* self = nullptr; // TODO: multiple words???

  // return (query[pos] == '"') ? self : nullptr;
}

ISR* QueryCompiler::findNestedConstraint() {
  log.Debug("[QueryCompiler] Looking for Nested Constraint");
  if (pastQueryEnd())
    return nullptr;

  if (query[pos] != "(") {
    log.Debug("[QueryCompiler] Not found - unable to find (");
    return nullptr;
  }
  ++pos;

  ISR* self = findConstraint();

  if (query[pos] != ")") {
    log.Debug("[QueryCompiler] Not found - unable to find )");
    return nullptr;
  }
  ++pos;

  return self;
}

ISR* QueryCompiler::findSearchWord() {
  log.Debug("[QueryCompiler] Looking for Search Word");
  if (pastQueryEnd())
    return nullptr;

  if (isAnd(query[pos]) || isOr(query[pos]) || isLeftBracket(query[pos]) || isRightBracket(query[pos])) {
    log.Debug("[QueryCompiler] Not found - found token " + query[pos] + " instead");
    return nullptr;
  }

  log.Debug("[QueryCompiler] Constructing ISRWord for " + query[pos]);
  return getWordISR(query[pos++]);
}

ISRWord* QueryCompiler::getWordISR(const string& word) {
  const uint32_t *wordId = bagOfWords->get(word);
  if (!wordId) {
    log.Fatal("[QueryCompiler:getWordISR] Word not in bag of words you stoopid!");
    return nullptr;
  }
  return new ISRWord(index->getPostingList(*wordId), index->getEndDocPostingList());
}

void QueryCompiler::tokenize(const string& _query) {
  log.Debug("[QueryCompiler] Tokenizing query");
  size_t start = 0, end;
  string tmp;
  query.clear();

  do {
    end = _query.find(' ', start);
    tmp = _query.substr(start, end - start);

    if ( tmp.empty() || tmp == " "  || naughtyFilter.isNaughty(tmp) || stopwordsFilter.isNaughty(tmp)) {
      start = end + 1;
      continue;
    }

    if ( !(query.empty() || isLeftBracket(query.back()) || isAnd(query.back()) || isOr(query.back())
                         || isRightBracket(tmp)         || isAnd(tmp)          || isOr(tmp)) )
      query.push_back("AND");

    query.push_back(tmp);

    start = end + 1;
  } while (end != string::npos);
}

void QueryCompiler::cleanQuery() {
  vector<string> validQuery;
  for (size_t i = 0; i < query.size(); ++i) {
    if (isSearchTerm(query[i]) && !(bagOfWords->contains(query[i]))) {
      if (!validQuery.empty() && !(isLeftBracket(validQuery.back())))
        validQuery.pop_back();
    }
    else if (isRightBracket(query[i]) && (validQuery.empty() || isLeftBracket(validQuery.back()))) {
      validQuery.pop_back();
      if (!validQuery.empty() && (isAnd(validQuery.back()) || isOr(validQuery.back())))
        validQuery.pop_back();
    }
    else if (!(validQuery.empty() && (isAnd(query[i]) || isOr(query[i]))))
      validQuery.push_back(query[i]);
  }
  query = validQuery;
}

ISR* QueryCompiler::parseQuery(const string& _query) {
  tokenize(_query);
  cleanQuery();
  pos = 0;
  log.Debug("[QueryCompiler] Constructing ISR structure");
  return findConstraint();
}
