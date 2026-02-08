#ifndef HTMLNODE_H
#define HTMLNODE_H

#include <lexbor/html/parser.h>

#include <string>

struct HtmlNode
{
    public:
        HtmlNode(lxb_dom_node_t* node);

        lxb_dom_node_t* GetNode();
        std::string GetText();
    private:
        lxb_dom_node_t* _node;

};

#endif