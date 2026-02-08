#include "SpotifyAPINew.h"

#include <iostream>

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
    HtmlNode titleNode = parser.Select(R"(meta[property="og:title"])");
    HtmlNode descriptionNode = parser.Select(R"(meta[property="og:description"])");
    HtmlNode imageNode = parser.Select(R"(meta[property="og:image"])");
    HtmlNode durationNode = parser.Select(R"(meta[name="music:duration"])");
    HtmlNode releaseDateNode = parser.Select(R"(meta[name="music:release_date"])");
    HtmlNode albumNumberNode = parser.Select(R"(meta[name="music:album:track"])");
    HtmlNode artistsNamesNode = parser.Select(R"(meta[name="music:musician_description"])");
    std::vector<HtmlNode> artistsIdNodes = parser.SelectAll(R"(meta[name="music:musician"])");

    // Explicit tag is only found in the album list at the bottom of the page
    std::vector<HtmlNode> trackNodes = parser.SelectAll(R"(a[href="/track/)" + id + R"("])");
    HtmlNode explicitNode = parser.Select(trackNodes.back(), R"(span[aria-label="Explicit"])");



    return TrackData();
}