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
    HtmlNode trackViewNode = parser.Select(R"(div[data-testid="track-view"] > :first-child)");
    HtmlNode titleNode = parser.Select(trackViewNode, R"(:first-child h1)");
    HtmlNode artistNode = parser.Select(trackViewNode, R"(div.encore-text-body-small a)");
    std::cout << titleNode.GetText() << std::endl;
    std::cout << artistNode.GetText() << std::endl;

    return TrackData();
}