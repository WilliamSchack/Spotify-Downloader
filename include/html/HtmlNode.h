#ifndef HTMLNODE_H
#define HTMLNODE_H

#include <lexbor/dom/interfaces/element.h>
#include <lexbor/html/parser.h>

#include <string>

struct HtmlNode
{
    public:
        HtmlNode(lxb_dom_node_t* node);

        lxb_dom_node_t* GetNode() const;
        std::string GetText() const;
        std::string GetAttribute(const std::string& attribute) const;
    private:
        lxb_dom_node_t* _node;

};

#endif