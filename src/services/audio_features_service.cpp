#include "audio_features_service.hpp"

#include <Wt/WApplication.h>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

char const* LambdaSnail::music::services::AudioFeaturesService::s_Url =
    "https://api.reccobeats.com/v1/analysis/audio-features";

LambdaSnail::music::services::AudioFeaturesService::AudioFeaturesService()
{
    auto* headers = curl_slist_append(nullptr, "Expect:");
    headers       = curl_slist_append(headers, "Accept: application/json");
    headers       = curl_slist_append(headers, "Content-Type: multipart/form-data");

    m_Headers = HeaderPointer(headers);

    curl_easy_setopt(m_Curl.get(), CURLOPT_SSL_VERIFYPEER, 0);
}

std::expected<LambdaSnail::music::AudioAnalysis, std::string>
LambdaSnail::music::services::AudioFeaturesService::getFileAnalysisResults(
    std::string const& buffer) const
{
    curl_mime* multipart = curl_mime_init(m_Curl.get());
    curl_mimepart* part  = curl_mime_addpart(multipart);

    curl_mime_filename(part, "dummy.mp3"); // must be here even if we don't use it
    curl_mime_type(part, "application/binary");

    curl_mime_data(part, buffer.data(), buffer.size());
    curl_mime_name(part, "audioFile");

    curl_easy_setopt(m_Curl.get(), CURLOPT_MIMEPOST, multipart);

    curl_easy_setopt(m_Curl.get(), CURLOPT_URL, s_Url);
    curl_easy_setopt(m_Curl.get(), CURLOPT_HTTPHEADER, m_Headers.get());

    std::string response;
    curl_easy_setopt(m_Curl.get(),
                     CURLOPT_WRITEFUNCTION,
                     LambdaSnail::music::services::AudioFeaturesService::writeToBuffer);
    curl_easy_setopt(m_Curl.get(), CURLOPT_WRITEDATA, &response);

    CURLcode code = curl_easy_perform(m_Curl.get());

    if (code == CURLcode::CURLE_OK) {
        wApp->log("notice") << "Request successful.";

        auto obj = nlohmann::json::parse(response);
        return AudioAnalysis{
            .acousticness     = obj["acousticness"].get<double>(),
            .danceability     = obj["danceability"].get<double>(),
            .energy           = obj["energy"].get<double>(),
            .instrumentalness = obj["instrumentalness"].get<double>(),
            .liveness         = obj["liveness"].get<double>(),
            .loudness         = obj["loudness"].get<double>(),
            .speechiness      = obj["speechiness"].get<double>(),
            .tempo            = obj["tempo"].get<double>(),
            .valence          = obj["valence"].get<double>()
        };
    }

    wApp->log("error") << "Error when sending request to the analysis server: " << static_cast<size_t>(code);
    wApp->log("error") << curl_easy_strerror(code);

    return std::unexpected( std::format("Received error code: {}", static_cast<size_t>(code)) );
}

size_t LambdaSnail::music::services::AudioFeaturesService::writeToBuffer(
    void* buffer, size_t size, size_t count, void* user)
{
    size_t numBytes = size * count;
    static_cast<std::string*>(user)->append(static_cast<char*>(buffer), 0, numBytes);
    return numBytes;
}