#include "audio_features_service.hpp"

#include <iostream>

namespace LambdaSnail::music::services
{
char const* AudioFeaturesService::s_Url = "https://api.reccobeats.com/v1/analysis/audio-features";
} // namespace LambdaSnail::music::services

LambdaSnail::music::services::AudioFeaturesService::AudioFeaturesService()
{
    auto* headers = curl_slist_append(nullptr, "Expect:");
    headers             = curl_slist_append(headers, "Accept: application/json");
    headers             = curl_slist_append(headers, "Content-Type: multipart/form-data");

    m_Headers = HeaderPointer(headers);
}

void LambdaSnail::music::services::AudioFeaturesService::getFileAnalysisResults(
    std::string const& buffer) const
{
    curl_mime* multipart = curl_mime_init(s_Curl.get());
    curl_mimepart* part  = curl_mime_addpart(multipart);

    curl_mime_filename(part, "dummy.mp3"); // must be here even if we don't use it
    curl_mime_type(part, "application/binary");

    curl_mime_data(part, buffer.data(), buffer.size());
    curl_mime_name(part, "audioFile");

    curl_easy_setopt(s_Curl.get(), CURLOPT_MIMEPOST, multipart);

    curl_easy_setopt(s_Curl.get(), CURLOPT_URL, s_Url);
    curl_easy_setopt(s_Curl.get(), CURLOPT_HTTPHEADER, m_Headers.get());

    std::string Response;
    curl_easy_setopt(s_Curl.get(), CURLOPT_WRITEFUNCTION, LambdaSnail::music::services::AudioFeaturesService::writeToBuffer);
    curl_easy_setopt(s_Curl.get(), CURLOPT_WRITEDATA, &Response);

    CURLcode res = curl_easy_perform(s_Curl.get());

    std::cout << "Response from server --------" << std::endl;
    std::cout << res << std::endl;
    std::cout << Response << std::endl;
}

size_t LambdaSnail::music::services::AudioFeaturesService::writeToBuffer(
    void* buffer, size_t size, size_t count, void* user)
{
    size_t numBytes = size * count;
    static_cast<std::string *>(user)->append(static_cast<char *>(buffer), 0, numBytes);
    return numBytes;
}