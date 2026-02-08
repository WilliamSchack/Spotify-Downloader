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

    size_t textLen;
    const lxb_char_t *text = lxb_dom_node_text_content(_node, &textLen);
    std::string textString = reinterpret_cast<const char*>(text);

    return textString;
}

std::string HtmlNode::GetAttribute(const std::string& attribute) const
{
    if (_node == nullptr)
        return "";

    const lxb_char_t* lxbAttribute = reinterpret_cast<const lxb_char_t*>(attribute.c_str());

    size_t textLen;
    lxb_dom_element* element = lxb_dom_interface_element(_node);
    const lxb_char_t* value = lxb_dom_element_get_attribute(element, lxbAttribute, attribute.size(), &textLen);
    std::string valueString = reinterpret_cast<const char*>(value);

    return valueString;
}