#include "Ytdlp.h"

std::string Ytdlp::GetVersion()
{
    Process process = Process::GetRelativeProcess(YTDLP_PATH_RELATIVE);
    process.AddArgument("--version");

    std::string output = process.Execute();
    return output;
}

// Assumes the path has no extension
void Ytdlp::Download(const std::string& url, const std::filesystem::path& pathNoExtension)
{
    // Check file path
    std::filesystem::path pathM4a = pathNoExtension.string() + ".m4a";
    std::filesystem::path pathWebm = pathNoExtension.string() + ".webm";

    // Get dependencies paths (move to their own files)
    std::filesystem::path executablePath = FileUtils::GetExecutablePath();
    std::filesystem::path ffmpegPath = executablePath.parent_path() / FFMPEG_PATH_RELATIVE;
    std::filesystem::path quickJsPath = executablePath.parent_path() / QUICKJS_PATH_RELATIVE;

    // Setup new line callback, get details from the download
    unsigned int bitrate = 0;
    unsigned int durationSeconds = 0;
    std::string extension = "";
    std::filesystem::path downloadedPath = pathNoExtension;

    std::string error = "";

    std::function<void(std::string)> newLineCallback = [&](std::string line) {
        // Get the download progress
        if (StringUtils::Contains(line, "[download]") && !StringUtils::Contains(line, pathNoExtension.filename())) {
            std::smatch matches;
            if (!std::regex_search(line, matches, std::regex(R"(]\s*(.*)%)")))
                return;

            float progressPercent = std::atof(matches[1].str().c_str()) / 100;

            // Output this to a progress callback
            std::cout << progressPercent << std::endl;
            
            return;
        }

        // Get the details
        if (StringUtils::Contains(line, "DETAILS: ") && !StringUtils::Contains(line, "debug")) {
            std::smatch matches;

            // Bitrate
            if (std::regex_search(line, matches, std::regex(R"(\[bitrate\](\d+))"))) {
                bitrate = std::stoi(matches[1]);
            }

            // Duration
            if (std::regex_search(line, matches, std::regex(R"(\[duration\](.*?)\[)"))) {
                std::string durationString = matches[1];
                durationSeconds = StringUtils::TimeToSeconds(durationString);
            }

            // Extension
            if (std::regex_search(line, matches, std::regex(R"(\[extension\](\w+))"))) {
                extension = matches[1];
            }
        }

        // Error
        if (StringUtils::Contains(line, "ERROR:"))
            error = line;
    };

    // Download
    Process process = Process::GetRelativeProcess(YTDLP_PATH_RELATIVE);
    process.AddArgument("--ffmpeg-location", "\"" + ffmpegPath.string() + "\"");
    process.AddArgument("--js-runtimes", "quickjs:\"" + quickJsPath.string() + "\"");
    process.AddArgument("--print", "\"DETAILS: [bitrate]%(abr)s[duration]%(duration_string)s[extension]%(ext)s\"");
    process.AddArgument("--extractor-args", "\"youtube:player_client=default\"");
    process.AddArgument("-v");
    process.AddArgument("--no-part");
    process.AddArgument("--progress");
    process.AddArgument("--no-simulate");
    process.AddArgument("-f", "ba");
    process.AddArgument("--audio-quality", "0");
    process.AddArgument("-o", "\"" + pathNoExtension.string() + ".%(ext)s\"");
    process.AddArgument("\"" + url + "\"");

    std::string output = process.Execute(newLineCallback);

    // Parse errors
}