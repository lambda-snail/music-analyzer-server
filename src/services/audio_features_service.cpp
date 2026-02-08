#include "audio_features_service.hpp"

#include <Wt/WApplication.h>
#include <bits/this_thread_sleep.h>
#include <boost/url.hpp>
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

    m_MultiPartHeaders = HeaderPointer(headers);

    headers           = curl_slist_append(nullptr, "Accept: application/json");
    m_OnlyJsonHeaders = HeaderPointer(headers);

    // Disables verification of SSL cert - dangerous but may simplify during development
    // curl_easy_setopt(m_Curl.get(), CURLOPT_SSL_VERIFYPEER, 0);
}

std::expected<LambdaSnail::music::AudioAnalysis, std::string>
LambdaSnail::music::services::AudioFeaturesService::getFileAnalysisResults(
    std::string const& buffer, Wt::WApplication* app)
{
    std::scoped_lock<std::mutex> lock(m_Lock);

    curl_mime* multipart = curl_mime_init(m_Curl.get());
    curl_mimepart* part  = curl_mime_addpart(multipart);

    curl_mime_filename(part, "dummy.mp3"); // must be here even if we don't use it
    curl_mime_type(part, "application/binary");

    curl_mime_data(part, buffer.data(), buffer.size());
    curl_mime_name(part, "audioFile");

    curl_easy_setopt(m_Curl.get(), CURLOPT_MIMEPOST, multipart);

    curl_easy_setopt(m_Curl.get(), CURLOPT_URL, s_Url);
    curl_easy_setopt(m_Curl.get(), CURLOPT_HTTPHEADER, m_MultiPartHeaders.get());

    std::string response;
    curl_easy_setopt(m_Curl.get(),
                     CURLOPT_WRITEFUNCTION,
                     LambdaSnail::music::services::AudioFeaturesService::writeToBuffer);
    curl_easy_setopt(m_Curl.get(), CURLOPT_WRITEDATA, &response);

    //std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    CURLcode code = curl_easy_perform(m_Curl.get());
    //std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (code == CURLcode::CURLE_OK) {
        int64_t httpCode;
        curl_easy_getinfo(m_Curl.get(), CURLINFO_RESPONSE_CODE, &httpCode);

        app->log("notice") << "HTTP status code is " << httpCode;

        // An http call can succeed in more ways than 2xx, but in this case it's all we're
        // interested in.
        if (httpCode >= 200 and httpCode < 300) {
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
                .valence          = obj["valence"].get<double>()};
        }

        // Example error result
        // {
        //   "timestamp" : "2026-02-08T14:14:23.943+00:00",
        //   "error" : "Too many request, retry after 5 seconds",
        //   "path" : "uri=/v1/analysis/audio-features",
        //   "status" : 4291
        // }

        app->log("error") << response;
        auto error = nlohmann::json::parse(response);
        return std::unexpected(error["error"].get<std::string>());
    }

    app->log("error") << "Error when sending request to the analysis server: "
                      << static_cast<size_t>(code);
    app->log("error") << curl_easy_strerror(code);

    return std::unexpected(std::format("Received error code: {}", static_cast<size_t>(code)));
}

std::expected<int64_t, std::string> LambdaSnail::music::services::AudioFeaturesService::get(
    std::string_view const& url, std::string& out_buffer, Wt::WApplication* app) const
{
    if (not m_Curl) {
        return std::unexpected("CURL client has not been initialized yet.");
    }

    curl_easy_setopt(m_Curl.get(), CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(m_Curl.get(), CURLOPT_URL, url.data());
    curl_easy_setopt(m_Curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(m_Curl.get(), CURLOPT_DEFAULT_PROTOCOL, "https");

    curl_easy_setopt(m_Curl.get(), CURLOPT_HTTPHEADER, m_OnlyJsonHeaders.get());

    curl_easy_setopt(m_Curl.get(),
                     CURLOPT_WRITEFUNCTION,
                     LambdaSnail::music::services::AudioFeaturesService::writeToBuffer);
    curl_easy_setopt(m_Curl.get(), CURLOPT_WRITEDATA, &out_buffer);

    CURLcode code = curl_easy_perform(m_Curl.get());

    if (code != CURLcode::CURLE_OK)
    {
        return std::unexpected(
            std::format("Curl indicated error: CURLcode::{}", static_cast<uint32_t>(code)));
    }

    int64_t httpCode;
    curl_easy_getinfo(m_Curl.get(), CURLINFO_RESPONSE_CODE, &httpCode);

    app->log("notice") << "HTTP status code is " << httpCode;
    return httpCode;
}

std::expected<uint32_t, std::string>
LambdaSnail::music::services::AudioFeaturesService::getSpotiyAnalysisResults(
    std::string_view const& spotifyId,
    std::vector<std::unique_ptr<AudioInformation>>& songs,
    Wt::WApplication* app)
{
    std::scoped_lock lock(m_Lock);

    std::string response{};
    auto result = get(std::format("https://api.reccobeats.com/v1/audio-features?ids={}", spotifyId),
                      response,
                      app);
    return result
        .and_then([&songs, &spotifyId, &response, this, app](int64_t httpStatusCode) {
            if (httpStatusCode < 200 or httpStatusCode >= 300) {
                return std::expected<uint32_t, std::string>(
                    std::unexpect,
                    std::format("Response does not indicate success: {}", httpStatusCode));
            }

            auto data               = nlohmann::json::parse(response);
            auto const contentArray = data["content"];
            for (auto const& obj : contentArray) {
                auto song  = std::make_unique<AudioInformation>();
                song->name = spotifyId; // TODO: Won't work if we have several ids

                song->data.acousticness     = obj["acousticness"].get<double>();
                song->data.danceability     = obj["danceability"].get<double>();
                song->data.energy           = obj["energy"].get<double>();
                song->data.instrumentalness = obj["instrumentalness"].get<double>();
                song->data.liveness         = obj["liveness"].get<double>();
                song->data.loudness         = obj["loudness"].get<double>();
                song->data.speechiness      = obj["speechiness"].get<double>();
                song->data.tempo            = obj["tempo"].get<double>();
                song->data.valence          = obj["valence"].get<double>();

                std::string resp{};
                auto songNameResult =
                    get(std::format("https://api.reccobeats.com/v1/track/{}",
                                    obj["id"].get<std::string>()),
                        resp,
                        app);
                if (songNameResult.has_value() and songNameResult.value() >= 200 and
                    songNameResult.value() < 300) {
                    auto trackInfo = nlohmann::json::parse(resp);
                    song->name     = trackInfo["trackTitle"].get<std::string>();
                }

                songs.push_back(std::move(song));
            }

            return std::expected<uint32_t, std::string>(songs.size());
        })
        .or_else([](std::string const& error) {
            return std::expected<uint32_t, std::string>(std::unexpect, error);
        });

    // int64_t httpCode;
    // curl_easy_getinfo(m_Curl.get(), CURLINFO_RESPONSE_CODE, &httpCode);
    //
    // app->log("notice") << "HTTP status code is " << httpCode;
    //
    // auto obj = nlohmann::json::parse(response);
    //
    // if (httpCode >= 200 and httpCode < 300) {
    //     return AudioAnalysis{
    //         .acousticness     = obj["acousticness"].get<double>(),
    //         .danceability     = obj["danceability"].get<double>(),
    //         .energy           = obj["energy"].get<double>(),
    //         .instrumentalness = obj["instrumentalness"].get<double>(),
    //         .liveness         = obj["liveness"].get<double>(),
    //         .loudness         = obj["loudness"].get<double>(),
    //         .speechiness      = obj["speechiness"].get<double>(),
    //         .tempo            = obj["tempo"].get<double>(),
    //         .valence          = obj["valence"].get<double>()
    //     };
    // }
}

std::string LambdaSnail::music::services::AudioFeaturesService::getYouTubeVideoId(
    std::string_view const& url) const
{
    if (url.empty()) {
        return {};
    }

    if (not url.contains('/')) {
        return std::string(url);
    }

    boost::system::result<boost::url_view> r = boost::urls::parse_uri(url);
    if (r.has_error()) {
        return {};
    }

    boost::url_view parsedUrl = r.value();
    auto qp                   = parsedUrl.params();
    if (not qp.contains("v")) {
        return {};
    }

    auto videoIterator = qp.find("v");
    return (*videoIterator).has_value ? (*videoIterator).value : std::string();
}

std::string
LambdaSnail::music::services::AudioFeaturesService::getSpotifyId(std::string_view const& url) const
{
    if (url.empty()) {
        return {};
    }

    if (not url.contains('/')) {
        return std::string(url);
    }

    if (not url.contains("track")) {
        return {};
    }

    boost::system::result<boost::url_view> r = boost::urls::parse_uri(url);
    if (r.has_error()) {
        return {};
    }

    boost::url_view parsedUrl = r.value();
    if (parsedUrl.segments().empty()) {
        return {};
    }

    auto maybeId = parsedUrl.segments().back();
    return maybeId.c_str();
}

size_t LambdaSnail::music::services::AudioFeaturesService::writeToBuffer(
    void* buffer, size_t size, size_t count, void* user)
{
    size_t numBytes = size * count;
    static_cast<std::string*>(user)->append(static_cast<char*>(buffer), 0, numBytes);
    return numBytes;
}