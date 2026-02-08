#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <lexbor/html/parser.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

#include <string>
#include <iostream>
#include <chrono>
#include <thread>

class HtmlParser
{
    public:
        HtmlParser(const std::string& html);
        ~HtmlParser();

        lxb_dom_node_t* Select(const std::string& selector);

        lxb_status_t GetStatus();
    private:
        lxb_html_document* _document;
        lxb_css_parser_t* _parser;
        lxb_selectors_t* _selectors;

        lxb_status_t _lastStatus;

        lxb_dom_node_t* _lastNode;
        bool _searching = false;
    private:
        static lxb_status_t FindCallback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void *ctx);
};

#endif