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

HtmlNode HtmlParser::Select(const std::string& selector)
{
    const lxb_char_t* lxbSelector = reinterpret_cast<const lxb_char_t*>(selector.c_str());

    lxb_css_selector_list_t *list = lxb_css_selectors_parse(_parser, lxbSelector, selector.size());
    _lastStatus = _parser->status;
    if (_lastStatus != LXB_STATUS_OK) return nullptr;

    _searching = true;
    _lastStatus = lxb_selectors_find(_selectors, lxb_dom_interface_node(_document), list, FindCallback, this);
    if (_lastStatus != LXB_STATUS_OK) {
        lxb_css_selector_list_destroy_memory(list);
        return nullptr;
    }

    lxb_css_selector_list_destroy_memory(list);

    // Wait for selector find, done on another thread so this one can be blocked
    while (_searching) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    HtmlNode node(_lastNode);
    return node;
}

lxb_status_t HtmlParser::GetStatus()
{
    return _lastStatus;
}

// Assumes the parser object is passed to ctx
lxb_status_t HtmlParser::FindCallback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void *ctx)
{
    HtmlParser* parser = (HtmlParser*)ctx;
    parser->_lastNode = node;
    parser->_searching = false;

    return LXB_STATUS_OK;
}