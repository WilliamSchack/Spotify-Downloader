#include "HtmlParser.h"

HtmlParser::HtmlParser(const std::string& html)
{
    // Setup document
    const lxb_char_t* lxbHtml = reinterpret_cast<const lxb_char_t*>(html.c_str());
    _document = lxb_html_document_create();
    _lastStatus = lxb_html_document_parse(_document, lxbHtml, html.size());
    if (_lastStatus != LXB_STATUS_OK) return;

    // Create parser
    _parser = lxb_css_parser_create();
    _lastStatus = lxb_css_parser_init(_parser, NULL);
    if (_lastStatus != LXB_STATUS_OK) return;

    // Create selectors
    _selectors = lxb_selectors_create();
    _lastStatus = lxb_selectors_init(_selectors);
    if (_lastStatus != LXB_STATUS_OK) return;
}

HtmlParser::~HtmlParser()
{
    lxb_html_document_destroy(_document);
    lxb_css_parser_destroy(_parser, true);
    lxb_selectors_destroy(_selectors, true);
}

lxb_status_t HtmlParser::GetStatus()
{
    return _lastStatus;
}