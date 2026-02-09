#pragma once

#include "load_balancer.hpp"
#include "models/audio.hpp"

#include <curl/curl.h>
#include <expected>

#include <Wt/WApplication.h>
#include <memory>
#include <string>
#include <mutex>

namespace LambdaSnail::music::services
{
class AudioFeaturesService
{
  public:
    explicit AudioFeaturesService();
    [[nodiscard]] std::expected<AudioAnalysis, std::string> getFileAnalysisResults(std::string const& buffer, Wt::WApplication* app);
    [[nodiscard]] std::expected<uint32_t, std::string> getSpotifyAnalysisResults(
        std::string_view const& spotifyId,
        std::vector<std::unique_ptr<AudioInformation>>& songs,
        Wt::WApplication* app);

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

    utils::LoadBalancer m_LoadBalancer;

    std::mutex m_Lock{};

    [[nodiscard]] std::expected<int64_t, std::string>
    get(std::string_view const& url, std::string& out_buffer, Wt::WApplication* app);
    [[nodiscard]] static size_t writeToBuffer(void* buffer, size_t size, size_t count, void* user);

    [[nodiscard]] std::expected<int64_t, std::string>
    doRequest(CURL* curl, Wt::WApplication* app, std::string& outResponse);

    static constexpr int MaxSleepTimeSeconds = 30;
    static constexpr int MaxTokenRetryCount = 16;
};

} // namespace LambdaSnail::music::services