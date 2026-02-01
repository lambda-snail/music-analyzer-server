#pragma once

#include "models/audio.hpp"

#include <curl/curl.h>
#include <expected>

#include <Wt/WApplication.h>
#include <memory>
#include <string>

namespace LambdaSnail::music::services
{
class AudioFeaturesService
{
  public:
    explicit AudioFeaturesService();
    [[nodiscard]] std::expected<AudioAnalysis, std::string> getFileAnalysisResults(std::string const& buffer, Wt::WApplication* app) const;
    [[nodiscard]] std::expected<uint32_t, std::string> getSpotiyAnalysisResults(
        std::string_view const& spotifyId,
        std::vector<std::unique_ptr<AudioInformation>>& songs,
        Wt::WApplication* app) const;

    [[nodiscard]] std::string getYouTubeVideoId(std::string_view const& url) const;
    [[nodiscard]] std::string getSpotifyId(std::string_view const& url) const;

  private:
    using CurlPointer = std::unique_ptr<CURL, decltype([](CURL* c) { curl_easy_cleanup(c); })>;
    using HeaderPointer =
        std::unique_ptr<curl_slist, decltype([](curl_slist* l) { curl_slist_free_all(l); })>;
    using UrlPointer = std::unique_ptr<CURLU, decltype([](CURLU* h) { curl_url_cleanup(h); })>;

    CurlPointer m_Curl{curl_easy_init()};
    HeaderPointer m_MultiPartHeaders{};
    HeaderPointer m_OnlyJsonHeaders{};
    static char const* s_Url;

    [[nodiscard]] std::expected<int64_t, std::string>
    get(std::string_view const& url, std::string& out_buffer, Wt::WApplication* app) const;
    static size_t writeToBuffer(void* buffer, size_t size, size_t count, void* user);
};

} // namespace LambdaSnail::music::services