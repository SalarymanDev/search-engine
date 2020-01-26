#ifndef HTML_PARSER_H
#define HTML_PARSER_H

#include "src/libs/document/Document.h"
#include "src/libs/parsedurl/ParsedUrl.h"
#include "src/libs/naughtyFilter/NaughtyFilter.h"
#include "src/libs/map/map.h"
#include <unordered_set>
#include <string>
#include <iostream>

namespace BBG {
    
    const static std::unordered_set<std::string> voidElements {
            "br",
            "hr",
            "col",
            "img",
            "wbr",
            "area",
            "base",
            "meta",
            "embed",
            "input",
            "param",
            "track",
            "source",
            "command",
            "keygen",
            "menuitem",
            "!doctype",
            "link"
        };

    enum TagType { body, anchor, title, voidElement, comment, img };

    class HtmlParser {
    public:
        HtmlParser(const NaughtyFilter &stopwordsFilter);
        ~HtmlParser();

        bool parse(string& html, Document& output);

    private:
        const NaughtyFilter &stopwordsFilter;
        string token;
        TagType currentType;
        bool isInsideAnchorTag;
        string currentUrl;
        ParsedUrl *currentUrlComponents;
        string relativeUrlRoot;
        char currentQuote;
        vector<bool> stack;

        // Use state
        void reset();
        void outputToken(Document& output);
        bool parse(const char* html, Document& output);

        // Helpers
        static bool isEndOfTag(const char c);
        static bool isWhiteSpace(const char c);
        static bool isVoidElement(const char* ptr);
        static bool isReadableContent(const char* ptr);
        static TagType getTagType(const char* ptr);
        static char skipTag(const char*& ptr);
        static void skipComment(const char*& ptr);
        static string getHref(const char*& ptr);
        static bool isImg(const char *ptr);
    };
}

#endif
