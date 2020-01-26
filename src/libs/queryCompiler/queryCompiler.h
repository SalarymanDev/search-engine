/* 
 * BNF language from slides for our query compiler
 *
 * <Constraint>       ::= <BaseConstraint> { <OrOp> <BaseConstraint> }
 *
 * <OrOp>             ::= '|' | '||' | 'OR'
 *
 * <BaseConstraint>   ::= <SimpleConstraint> { [ <AndOp> ] <SimpleConstraint> }
 *
 * <AndOp>            ::= '&' | '&&' | 'AND'
 *
 * <SimpleConstraint> ::= <Phrase> | <NestedConstraint> | <SearchWord>
 *
 * <Phrase>           ::= '"' { <SearchWord> } '"'
 *
 * <NestedConstraint> ::= '(' <Constraint> ')'
 *
 * <SearchWord>       ::= string
 *
 */

#ifndef QUERY_COMPILER_H
#define QUERY_COMPILER_H

#include "src/libs/string/string.h"
#include "src/libs/ISR/ISR.h"
#include "src/libs/index/index.h"
#include "src/libs/logger/logger.h"
#include "src/libs/naughtyFilter/NaughtyFilter.h"


namespace BBG {
  class QueryCompiler {
  public:
    Logger log;
    vector<string> query;
    size_t pos = 0;
    IndexChunk* index;
    NaughtyFilter naughtyFilter;
    NaughtyFilter stopwordsFilter;
    const Map<string, uint32_t> *bagOfWords;

    void tokenize(const string& _query);
    void cleanQuery();
    bool pastQueryEnd();

    ISR* findConstraint();
    bool findOrOp();
    ISR* findBaseConstraint();
    bool findAndOp();
    ISR* findSimpleConstraint();
    ISR* findPhrase();
    ISR* findNestedConstraint();
    ISR* findSearchWord();

    // This method is here, so that it can be mocked out by tests
    virtual ISRWord* getWordISR(const string& word);

    QueryCompiler(); // for testing purposes
    QueryCompiler(IndexChunk* _index, const Map<string, uint32_t> *_bagOfWords);
    ISR* parseQuery(const string& _query);
  };
}

#endif
