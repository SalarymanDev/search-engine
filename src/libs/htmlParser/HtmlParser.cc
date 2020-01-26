#include "src/libs/htmlParser/HtmlParser.h"
#include "src/libs/utils/utils.h"

using BBG::strncmp;
using BBG::HtmlParser;
using BBG::TagType;
using BBG::Document;
using BBG::string;

// Public member functions
HtmlParser::HtmlParser(const NaughtyFilter &stopwordsFilter) 
    : stopwordsFilter(stopwordsFilter), currentUrlComponents(nullptr) {}

HtmlParser::~HtmlParser() {
    if (currentUrlComponents)
        delete currentUrlComponents;
}

bool HtmlParser::parse(string& html, Document& output) {
    output.size = html.size();
    return parse(html.c_str(), output);
}

bool HtmlParser::parse(const char* html, Document& output) {
    reset();

    ParsedUrl urlComponents(output.url);
    if (strcmp(urlComponents.Service, "")) return false;
    if (strcmp(urlComponents.Host, "")) return false;

    relativeUrlRoot += urlComponents.Service;
    relativeUrlRoot += "://";
    relativeUrlRoot += urlComponents.Host;
    if (urlComponents.Port && !strcmp(urlComponents.Port, "")) {
        relativeUrlRoot += ':';
        relativeUrlRoot += urlComponents.Port;
    }

    for (; *html; ++html) {
        // Is opening tag
        if ( html[0] == '<' && *(html+1) && html[1] != ' ' && html[1] != '/' ) {
            currentType = getTagType(++html);
            if (currentType == TagType::anchor) {
                currentUrl = getHref(html);
                if (currentUrlComponents) delete currentUrlComponents;
                currentUrlComponents = new ParsedUrl(currentUrl);
                isInsideAnchorTag = true;
                currentQuote = skipTag(html);
            } else if (currentType == TagType::img) {
                output.imgCount++;
                currentQuote = skipTag(html);
                currentType = TagType::body;
            } else if (currentType == TagType::voidElement) {
                currentQuote = skipTag(html);
                currentType = TagType::body;
            } else if (currentType == TagType::comment) {
                skipComment(html);
            } else {
                stack.push_back( isReadableContent(html) );
                currentQuote = skipTag(html);
            }
            continue;
        }

        // Handle a closing quote
        if (currentQuote && *html == currentQuote) {
            currentQuote = '\0';
            continue;
        }

        // Ignore text inside quotes
        if (currentQuote) continue;

        // Is closing tag
        if ( html[0] == '<' && *(html+1) && html[1] == '/' ) {

            html += 2;

            // Check if holding a token
            if (token.size() > 0)
                outputToken(output);

            // Handle closing of anchor tag
            if (getTagType(html) == TagType::anchor) {
                isInsideAnchorTag = false;
                currentType = TagType::body;
            }

            stack.pop_back();
            skipTag(html);
            continue;
        }

        // Should read tokens
        if ( *html && (stack.empty() || stack.back()) ) {
            // Delimit tokens why whitespace
            if ( !isWhiteSpace(*html) )
                token += *html;
            else if (token.size() > 0)
                outputToken(output);
        }
    }

    if (!token.empty())
        outputToken(output);

    return true;
}

// Private non static member functions

void HtmlParser::reset() {
    stack.clear();
    token.clear();
    currentUrl.clear();
    relativeUrlRoot.clear();
    currentType = TagType::body;
    isInsideAnchorTag = false;
    currentQuote = '\0';
    if (currentUrlComponents) {
        delete currentUrlComponents;
        currentUrlComponents = nullptr;
    }
}

void HtmlParser::outputToken(Document& output) {
    token.lower();

    if (stopwordsFilter.isNaughty(token) || !token.isAlphaNum()) {
        token.clear();
        return;
    }

    if (isInsideAnchorTag || currentType == TagType::anchor) {
        if (!currentUrl.empty()) {
            // Check if relative url and apply
            if (currentUrl.front() == '/') {
                string fullUrl(relativeUrlRoot);
                fullUrl += currentUrl;
                output.anchors[fullUrl].push_back(token);
            } else if (strncmp("http", currentUrlComponents->Service, 4)) {
                output.anchors[currentUrl].push_back(token);
            }

            output.bodyText.push_back(token);
        }
    }
    else if (currentType == TagType::body)
        output.bodyText.push_back(token);
    else if (currentType == TagType::title)
        output.titleText.push_back(token);

    token.clear();
}

// Static member functions below this point

bool HtmlParser::isEndOfTag(const char c) {
    return c == ' ' || c == '>' || c == '/';
}

bool HtmlParser::isWhiteSpace(const char c) {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v' || c == '\f';
}

bool HtmlParser::isImg(const char *ptr) {
    return strncmp(ptr, "img", 3);
}

bool HtmlParser::isVoidElement( const char* ptr ) {
    char buffer[64];
    int i = 0;
    while (*ptr && !isWhiteSpace(*ptr) && *ptr != '>' && i < 64)
        buffer[i++] = *ptr++;

    buffer[i] = '\0';
    std::string tag(buffer);

    return voidElements.find(tag) != voidElements.end();
}

bool HtmlParser::isReadableContent(const char* ptr) {
    if ( *ptr == '!' )
        return false;
    
    for ( int i = 0;  i <= 4; ++i )
        if ( ptr[ i ] == '\0' )
            return false;

    if ( ptr[ 5 ] == '\0' )
        return false;
    if ( strncmp( ptr, "style", 5 ) && isEndOfTag( *(ptr + 5) ) )
        return false;
    
    if ( ptr[ 6 ] == '\0' ) 
        return false;
    if ( strncmp( ptr, "script", 6 ) && isEndOfTag( *(ptr + 6) ) )
        return false;
    

    return true;
}

TagType HtmlParser::getTagType( const char* ptr ) {
    if ( ptr[ 0 ] == '\0' || ptr[ 1 ] == '\0' ) 
        return TagType::body;
    if ( ptr[ 0 ] == 'a' && (ptr[1] == ' ' || ptr[1] == '>') )
        return TagType::anchor;

    if (ptr[2] == '\0') 
        return TagType::body;
    if (ptr[0] == '!' && ptr[1] == '-' && ptr[2] == '-')
        return TagType::comment;

    if ( ptr[ 3 ] == '\0' || ptr[ 4 ] == '\0' ) 
        return TagType::body;

    if ( ptr[ 5 ] == '\0' ) 
        return TagType::body;
    if ( strncmp( ptr, "title", 5 ) && isEndOfTag( *(ptr + 5) ) )
        return TagType::title;

    if (isImg(ptr))
        return TagType::img;

    if (isVoidElement(ptr))
        return TagType::voidElement;

    return TagType::body;
}

char HtmlParser::skipTag(const char*& ptr) {
    char quoteType = '\0';
    while (*ptr && *ptr != '>') {
        if (!quoteType && (*ptr == '"' || *ptr == '\''))
            quoteType = *ptr;
        else if (quoteType && *ptr == quoteType)
            quoteType = '\0';
        ++ptr;
    }
    return quoteType;
}

void HtmlParser::skipComment(const char*& ptr) {
    while (*ptr && (*ptr != '>' || *(ptr-1) != '-' || *(ptr-2) != '-')) ++ptr;
}

string HtmlParser::getHref(const char*& ptr) {
    while (*ptr && *ptr != '>' && !strncmp(ptr, "href", 4)) ++ptr;
    while (*ptr && *ptr != '>' && *ptr != '=') ++ptr;
    ++ptr;
    while (*ptr && isWhiteSpace(*ptr)) ++ptr;

    string url;
    if (*ptr == '\'' || *ptr == '"') {
        char endQuote = *ptr++;
        while (*ptr && *ptr != endQuote)
            url += *ptr++;
    } 
    else
        while (*ptr && *ptr != '>' && *ptr != ' ')
            url += *ptr++;

    ptr++;
    return url;
}
