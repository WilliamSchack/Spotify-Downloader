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

    std::cout << responseHtml << std::endl;

    // Get details from html
    HtmlParser parser(responseHtml);
    HtmlNode trackViewNode = parser.Select(R"(div[data-testid="track-view"] > :first-child)");
    HtmlNode titleNode = parser.Select(trackViewNode, R"(:first-child h1)");
    HtmlNode artistNode = parser.Select(trackViewNode, R"(div.encore-text-body-small a)");
    
    HtmlNode bottomSectionNode = parser.Select(trackViewNode, R"(div[data-testid="entity-bottom-section"] > :first-child)");
    HtmlNode albumNode = parser.Select(bottomSectionNode, R"(:nth-child(1))");
    HtmlNode yearNode = parser.Select(bottomSectionNode, R"(:nth-child(2))");
    HtmlNode durationNode = parser.Select(bottomSectionNode, R"(:nth-child(3))");

    return TrackData();
}