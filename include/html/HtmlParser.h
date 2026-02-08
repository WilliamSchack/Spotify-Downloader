#ifndef HTMLPARSER_H
#define HTMLPARSER_H

#include <lexbor/html/parser.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

#include <string>
#include <iostream>

class HtmlParser
{
    public:
        HtmlParser(const std::string& html);
        ~HtmlParser();

        lxb_status_t GetStatus();
    private:
        lxb_html_document* _document;
        lxb_css_parser_t* _parser;
        lxb_selectors_t* _selectors;

        lxb_status_t _lastStatus;
    private:
        void CheckLastStatus();
};

#endif