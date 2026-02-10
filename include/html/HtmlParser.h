#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include "HtmlNode.h"

#include <lexbor/html/parser.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

#include <string>
#include <vector>
#include <iostream>

class HtmlParser
{
    public:
        HtmlParser(const std::string& html);
        ~HtmlParser();

        // HtmlNode(s) _node is owned by the document and will become useless when this destroyed
        std::vector<HtmlNode> SelectAll(lxb_dom_node_t* node, const std::string& selector);
        std::vector<HtmlNode> SelectAll(const HtmlNode& node, const std::string& selector);
        std::vector<HtmlNode> SelectAll(const std::string& selector);

        HtmlNode Select(lxb_dom_node_t* node, const std::string& selector);
        HtmlNode Select(const HtmlNode& node, const std::string& selector);
        HtmlNode Select(const std::string& selector);

        lxb_status_t GetStatus();
    private:
        static inline const int SELECT_TIMEOUT_MS = 1000;

        lxb_html_document* _document;
        lxb_css_parser_t* _parser;
        lxb_selectors_t* _selectors;
        lxb_css_selector_list_t* _currentSelectorList;

        lxb_status_t _lastStatus;

        std::vector<HtmlNode> _lastNodes;
    private:
        // Assumes the parser object is passed to ctx
        static lxb_status_t FindCallback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void *ctx);

        bool CreateSelectors();
        void CleanupSelectors();
};

#endif