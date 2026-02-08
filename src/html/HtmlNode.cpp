#include "HtmlNode.h"

HtmlNode::HtmlNode(lxb_dom_node_t* node)
    : _node(node) {}

lxb_dom_node_t* HtmlNode::GetNode() const
{
    return _node;
}

std::string HtmlNode::GetText() const
{
    if (_node == nullptr)
        return "";

    size_t text_len;
    const lxb_char_t *text = lxb_dom_node_text_content(_node, &text_len);
    std::string textString = reinterpret_cast<const char*>(text);

    return textString;
}