#pragma once

#include "models/audio.hpp"

#include <curl/curl.h>
#include <expected>

#include <memory>
#include <string>

namespace LambdaSnail::music::services
{
class AudioFeaturesService
{
  public:
    explicit AudioFeaturesService();
    [[nodiscard]] std::expected<AudioAnalysis, std::string> getFileAnalysisResults(std::string const& buffer) const;

  private:
    using CurlPointer = std::unique_ptr<CURL, decltype([](CURL* c) { curl_easy_cleanup(c); })>;
    using HeaderPointer =
        std::unique_ptr<curl_slist, decltype([](curl_slist* l) { curl_slist_free_all(l); })>;

    CurlPointer s_Curl{curl_easy_init()};
    HeaderPointer m_Headers;
    static char const* s_Url;

    static size_t writeToBuffer(void* buffer, size_t size, size_t count, void* user);
};

} // namespace LambdaSnail::music::services