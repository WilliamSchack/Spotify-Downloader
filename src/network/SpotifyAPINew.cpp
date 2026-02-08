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

    // Get details from html
    HtmlParser parser(responseHtml);
    HtmlNode titleNode = parser.Select(R"(meta[property="og:title"])");
    HtmlNode descriptionNode = parser.Select(R"(meta[property="og:description"])");
    HtmlNode durationNode = parser.Select(R"(meta[name="music:duration"])");
    HtmlNode releaseDateNode = parser.Select(R"(meta[name="music:release_date"])");
    HtmlNode albumNumberNode = parser.Select(R"(meta[name="music:album:track"])");

    return TrackData();
}