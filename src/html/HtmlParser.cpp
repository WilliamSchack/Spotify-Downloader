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

std::vector<HtmlNode> HtmlParser::SelectAll(lxb_dom_node_t* node, const std::string& selector)
{
    _lastNodes.clear();
    if (node == nullptr) return _lastNodes;

    const lxb_char_t* lxbSelector = reinterpret_cast<const lxb_char_t*>(selector.c_str());

    lxb_css_selector_list_t *list = lxb_css_selectors_parse(_parser, lxbSelector, selector.size());
    _lastStatus = _parser->status;
    if (_lastStatus != LXB_STATUS_OK) return _lastNodes;

    _lastStatus = lxb_selectors_find(_selectors, node, list, FindCallback, this);
    if (_lastStatus != LXB_STATUS_OK) return _lastNodes;

    return _lastNodes;
}

std::vector<HtmlNode> HtmlParser::SelectAll(const HtmlNode& node, const std::string& selector)
{
    return SelectAll(node.GetNode(), selector);
}

std::vector<HtmlNode> HtmlParser::SelectAll(const std::string& selector)
{
    return SelectAll(lxb_dom_interface_node(_document), selector);
}

HtmlNode HtmlParser::Select(lxb_dom_node_t* node, const std::string& selector)
{
    SelectAll(node, selector);
    if (_lastNodes.size() > 0)
        return _lastNodes[0];
    
    return HtmlNode(nullptr);
}

HtmlNode HtmlParser::Select(const HtmlNode& node, const std::string& selector)
{
    return Select(node.GetNode(), selector);
}

HtmlNode HtmlParser::Select(const std::string& selector)
{
    return Select(lxb_dom_interface_node(_document), selector);
}

lxb_status_t HtmlParser::GetStatus()
{
    return _lastStatus;
}

lxb_status_t HtmlParser::FindCallback(lxb_dom_node_t* node, lxb_css_selector_specificity_t spec, void *ctx)
{
    HtmlParser* parser = (HtmlParser*)ctx;
    parser->_lastNodes.push_back(HtmlNode(node));

    return LXB_STATUS_OK;
}