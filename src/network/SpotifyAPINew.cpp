#include "SpotifyAPINew.h"

#include <iostream>

#include <lexbor/html/parser.h>
#include <lexbor/css/css.h>
#include <lexbor/selectors/selectors.h>

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

    HtmlParser parser(responseHtml);
    lxb_dom_node_t* node = parser.Select(R"(div[data-testid="track-view"] > :first-child h1)");

    size_t text_len;
    const lxb_char_t *text = lxb_dom_node_text_content(node, &text_len);
    printf("%.*s\n", (int)text_len, text);

    return TrackData();
}