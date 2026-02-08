#include "SpotifyAPINew.h"

#include <iostream>

#include <lexbor/html/parser.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

lxb_status_t findCallback(lxb_dom_node_t *node, lxb_css_selector_specificity_t spec, void *ctx)
{
    size_t text_len;
    const lxb_char_t *text = lxb_dom_node_text_content(node, &text_len);
    printf("%.*s\n", (int)text_len, text);
}

TrackData SpotifyAPINew::GetTrack(const std::string& id)
{
    // Get embed page
    std::string embedUrl = "https://open.spotify.com/track/" + id;
    
    NetworkRequest request;
    request.Url = embedUrl;
    request.SetHeader("User-Agent", "Mozilla/5.0");
	request.SetHeader("Accept", "*/*");
	request.SetHeader("DNT", "1");
	request.SetHeader("Referer", "https://open.spotify.com");

    NetworkResponse response = request.Get();
    std::string responseHtml = response.Body;

    //std::cout << responseHtml << std::endl;

    // Get track details from html
    const lxb_char_t* lxbHtml = reinterpret_cast<const lxb_char_t*>(responseHtml.c_str());
    lxb_html_document* document = lxb_html_document_create();
    lxb_status_t status = lxb_html_document_parse(document, lxbHtml, responseHtml.size());
    if (status != LXB_STATUS_OK) {
        lxb_html_document_destroy(document);
        std::cout << "Failed to parse html";
        return TrackData();
    }

    lxb_css_parser_t *parser = lxb_css_parser_create();
    status = lxb_css_parser_init(parser, NULL);
    if (status != LXB_STATUS_OK) {
        // cleanup
        return TrackData();
    }

    const lxb_char_t selector[] = R"(div[data-testid="track-view"] > :first-child h1)";
    lxb_selectors_t* selectors = lxb_selectors_create();
    status = lxb_selectors_init(selectors);
    if (status != LXB_STATUS_OK) {
        // cleanup
        return TrackData();
    }

    lxb_css_selector_list_t *list = lxb_css_selectors_parse(parser, selector, sizeof(selector) - 1);
    if (parser->status != LXB_STATUS_OK) {
        // cleanup
        return TrackData();
    }

    status = lxb_selectors_find(selectors, lxb_dom_interface_node(document), list, findCallback, NULL);
    if (status != LXB_STATUS_OK) {
        // cleanup
        return TrackData();
    }

    lxb_selectors_destroy(selectors, true);
    lxb_css_parser_destroy(parser, true);
    lxb_css_selector_list_destroy_memory(list);
    lxb_html_document_destroy(document);

    return TrackData();
}