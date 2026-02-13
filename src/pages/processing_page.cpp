#include "processing_page.hpp"

#include "application/lambda_resource.hpp"
#include "components/process_log.hpp"
#include "components/song_view.hpp"
#include "services/audio_features_service.hpp"
#include "services/cookie_info.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WLink.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <algorithm>

#include <Wt/Http/Request.h>
#include <Wt/Http/Response.h>
#include <Wt/WApplication.h>
#include <Wt/WComboBox.h>
#include <Wt/WPushButton.h>
#include <cerrno>
#include <cstdio>
#include <fstream>
#include <future>
#include <numeric>
#include <ranges>
#include <syncstream>
#include <thread>

const std::string LambdaSnail::music::ProcessingPage::s_ServerGenericErrorMessage =
    "An error occurred while processing. This usually indicates that the server is busy - please try again in a litle while.";

void LambdaSnail::music::ProcessingPage::setupYoutubeProcessing(Wt::WTemplate* t)
{
    m_UrlInput = t->bindNew<Wt::WLineEdit>("yt-input");
    m_UrlInput->addStyleClass("w-50");

    m_SourceSelector = t->bindNew<Wt::WComboBox>("music-source-dropdown");
    m_SourceSelector->addStyleClass("w-25");
    m_SourceSelector->addItem("Spotify");
    //m_SourceSelector->addItem("YouTube"); // TODO: Add this to re-enable yt-dlp support
    m_SourceSelector->setCurrentIndex(static_cast<size_t>(MusicSource::Spotify));

    auto* button = t->bindNew<Wt::WPushButton>("yt-button", "Analyze");
    button->setStyleClass("btn");
    button->addStyleClass("btn-primary");

    button->clicked().connect([this]() {
        auto const& songInput = m_UrlInput->valueText().toUTF8();
        if (songInput.empty()) {
            return;
        }

        auto* logger = addNewLog(songInput, m_App);
        if (static_cast<int32_t>(MusicSource::Spotify) ==  m_SourceSelector->currentIndex())
        {
            std::thread([this, logger, songInput] { processSpotifyId(songInput, logger); }).detach();
        }
        else
        {
            std::thread([this, logger, songInput] { processYouTubeId(songInput, logger); }).detach();
        }
    });
}
void LambdaSnail::music::ProcessingPage::setupFileDrop(Wt::WTemplate* t)
{
    m_FileDrop = t->bindNew<Wt::WFileDropWidget>("file-drop");
    m_FileDrop->addNew<Wt::WText>("Drop Files Here");
    m_FileDrop->setToolTip(
        "Drop files here or click this component to upload from your hard drive");
    m_FileDrop->removeStyleClass("Wt-filedropzone");
    m_FileDrop->addStyleClass("ls-filedrop");
    m_FileDrop->addStyleClass("d-flex");
    m_FileDrop->addStyleClass("justify-content-center");
    m_FileDrop->setAcceptDirectories(false);

    m_FileDrop->uploaded().connect([this](Wt::WFileDropWidget::File* file) {
        // Stub to test file processing

        auto* logger = addNewLog(file->clientFileName(), m_App);

        std::thread([this, logger, file]() {
            processAudioFile(std::filesystem::path(file->uploadedFile().spoolFileName()));
            logger->setSuccessState();
        }).detach();
    });

    m_FileDrop->tooLarge().connect([this](Wt::WFileDropWidget::File* file, uint64_t size) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
    });

    m_FileDrop->uploadFailed().connect([this](Wt::WFileDropWidget::File* file) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
    });
}
void LambdaSnail::music::ProcessingPage::setupCsvConversion(Wt::WTemplate* t)
{
    m_DataFile = std::make_shared<application::LambdaResource>(
        [this](Wt::Http::Request const& request, Wt::Http::Response& response) {
            m_SongView->getRows([&response](int32_t columnCount, Wt::WTableRow* r) {
                for (int32_t c = 0; c < columnCount; ++c) {
                    if (auto const* text = dynamic_cast<Wt::WText*>(r->elementAt(c)->widget(0))) {
                        response.out() << text->text() << (c != columnCount - 1 ? "," : "\n");
                    }
                }
            });
        },
        "music-analysis.csv");

    Wt::WLink link(m_DataFile);
    link.setTarget(Wt::LinkTarget::NewWindow);

    auto* downloadButton = t->bindNew<Wt::WAnchor>("get-data-button", link, "Download Data");
    downloadButton->setStyleClass("btn");
    downloadButton->addStyleClass("btn-success");
}
LambdaSnail::music::ProcessingPage::ProcessingPage(
    LambdaSnail::music::services::AudioFeaturesService* audioService)
    : m_AudioService(audioService), m_App(wApp)
{
    auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("processing-page"));

    setupYoutubeProcessing(t);

    setupCsvConversion(t);

    // TODO: Comment back in to re-enable file-drop support (requires yt-dlp)
    t->bindEmpty("file-drop");
    //setupFileDrop(t);

    m_LogContainer = t->bindNew<Wt::WContainerWidget>("process-log");
    m_SongView = t->bindNew<SongView>("file-list");
}

LambdaSnail::music::ProcessLog*
LambdaSnail::music::ProcessingPage::addNewLog(std::string const& name, Wt::WApplication* app)
{
    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        return m_LogContainer->addNew<ProcessLog>(name, app);
    }

    return nullptr;
}

void LambdaSnail::music::ProcessingPage::processYouTubeId(
    std::string const& urlOrId, ProcessLog* logger)
{
    try {
        logger->updateMessage("Downloading from YouTube, this may take a while ...");

        std::string const videoId = m_AudioService->getYouTubeVideoId(urlOrId);
        if (videoId.empty())
        {
            logger->setErrorState("Unable to find video id or url.");
            return;
        }

        std::string cookieFileArgument{};
        if (LambdaSnail::services::CookieInfo::hasCookieFile()) {
            cookieFileArgument = std::format(
                "--cookies {}", LambdaSnail::services::CookieInfo::getCookieFile().string());
        }

        std::string const outFolder = std::format("/tmp/music/{}", videoId);

        auto const processResult =
            executeShellCommand(
                std::format("mkdir -p {}; yt-dlp -o '/tmp/music/%(title)s.%(ext)s' {} --restrict-filenames -q -t mp3 {} --exec 'ffmpeg -i {{}} -c copy -map 0 -segment_time 00:00:25 -f segment {}/output%03d.mp3'",
                            outFolder,
                            cookieFileArgument,
                            videoId,
                            outFolder))
                .and_then(
                    [this, &cookieFileArgument, &videoId]([[maybe_unused]] std::string const& result) {
                        // --get-filename returns .webm even if we use '-t mp3' so hard-code the format
                        return executeShellCommand(std::format(
                            "yt-dlp -o '/tmp/music/%(title)s.mp3' {} --restrict-filenames  --get-filename {}",
                            cookieFileArgument,
                            videoId));
                })
                .and_then(
                    [this, logger, &outFolder](std::filesystem::path const&& fileName) {
                        // Files are in /tmp/music/[videoid]/outputXYZ.mp3

                        std::string const name = fileName.stem();
                        logger->updateMessage(std::format("Found name of song: {}", name ));
                        logger->updateName(name);

                        // need to maintain file count so we can show progress in the UI
                        size_t numFiles = 0;
                        for (auto& p : std::filesystem::directory_iterator(outFolder))
                        {
                           ++numFiles;
                        }

                        int i = 0;
                        std::vector<AudioAnalysis> songParts{};
                        auto it = std::filesystem::directory_iterator{outFolder};
                        for (std::filesystem::directory_entry const& entry : it)
                        {
                            logger->updateMessage(std::format("Processing: {}%", 100 * ++i / numFiles ));
                            auto const path = entry.path();

                            auto result = processAudioFile(path);
                            if (result.has_value())
                            {
                                songParts.push_back(result.value());
                            }
                            else
                            {
                                return std::expected<std::pair<AudioAnalysis, std::filesystem::path>, std::string>(std::unexpect, result.error());
                            }
                        }

                        auto sum = std::accumulate(songParts.begin(), songParts.end(), AudioAnalysis{},
                            [](AudioAnalysis a, AudioAnalysis const& b) {
                                a.acousticness += b.acousticness;
                                a.danceability += b.danceability;
                                a.energy += b.energy;
                                a.instrumentalness += b.instrumentalness;
                                a.liveness += b.liveness;
                                a.loudness += b.loudness;
                                a.speechiness += b.speechiness;
                                a.tempo += b.tempo;
                                a.valence += b.valence;

                                return a;
                            });

                        sum.acousticness /= static_cast<double>(songParts.size());
                        sum.danceability /= static_cast<double>(songParts.size());
                        sum.energy /= static_cast<double>(songParts.size());
                        sum.instrumentalness /= static_cast<double>(songParts.size());
                        sum.liveness /= static_cast<double>(songParts.size());
                        sum.loudness /= static_cast<double>(songParts.size());
                        sum.speechiness /= static_cast<double>(songParts.size());
                        sum.tempo /= static_cast<double>(songParts.size());
                        sum.valence /= static_cast<double>(songParts.size());

                        return std::expected<std::pair<AudioAnalysis, std::filesystem::path>, std::string>{ std::make_pair<AudioAnalysis, std::string> (std::move(sum), std::move(fileName)) };
                    });

        if (not processResult.has_value()) {
            logger->setErrorState(processResult.error());
            return;
        }

        // TODO: Verify aggregation calculations are correct

        std::pair<AudioAnalysis, std::filesystem::path> const& result = processResult.value();

        auto songInfo = std::make_unique<music::AudioInformation>();
        songInfo->data = result.first;
        songInfo->name = result.second.stem();

        Wt::WApplication::UpdateLock uiLock(m_App);
        if (uiLock) {
            m_SongView->addSong(std::move(songInfo));
            logger->setSuccessState();
            m_App->triggerUpdate();
        }
    }
    catch (std::exception e)
    {
        m_App->log("error") << e.what();
        logger->setErrorState(s_ServerGenericErrorMessage);
    }
}
void LambdaSnail::music::ProcessingPage::processSpotifyId(
    std::string const& urlOrId, ProcessLog* logger)
{
    logger->updateMessage("Performing analysis, please wait ...");

    if (urlOrId.contains("playlist"))
    {
        logger->setErrorState("Playlists are not supported, please try again with a song.");
        return;
    }

    if (urlOrId.contains("album"))
    {
        logger->setErrorState("Albums are not supported, please try again with a song.");
        return;
    }

    if (urlOrId.contains("artist"))
    {
        logger->setErrorState("That's an artist, not a song :)");
        return;
    }

    std::string const spotifyId = m_AudioService->getSpotifyId(urlOrId);
    if (spotifyId.empty())
    {
        logger->setErrorState("Unable to find spotify id.");
        return;
    }

    // TODO: Support multiple ids

    try
    {
        std::vector<std::unique_ptr<AudioInformation>> songs {};
        auto const analysis = m_AudioService->getSpotifyAnalysisResults(spotifyId, songs, m_App);

        if (not analysis)
        {
            logger->setErrorState("An error occurred while processing");
            return;
        }

        if (songs.empty())
        {
            logger->setSuccessState("Spotify id does not have song data stored. Please use YouTube instead.");
            return;
        }

        Wt::WApplication::UpdateLock uiLock(m_App);
        if (uiLock) {
            for (auto& song : songs) {
                m_SongView->addSong(std::move(song));
            }

            logger->setSuccessState();
            m_App->triggerUpdate();
        }
    }
    catch (std::exception e)
    {
        m_App->log("error") << e.what();
        logger->setErrorState(s_ServerGenericErrorMessage);
    }
}

std::expected<LambdaSnail::music::AudioAnalysis, std::string>
LambdaSnail::music::ProcessingPage::processAudioFile(std::filesystem::path const& filePath)
{
    try
    {
        std::ifstream stream(filePath, std::ios::binary | std::ios::ate);
        if (not stream.is_open()) {
            return std::unexpected(std::format("Error when opening audio file: {}", filePath.string()));
        }

        size_t stream_size = stream.tellg();
        std::string buffer(stream_size, '\0');
        stream.seekg(0);

        if (not stream.read(&buffer[0], static_cast<std::streamoff>(stream_size))) {
            return std::unexpected(std::format("Error when reading audio file: {}", filePath.string()));
        }

        stream.close();

        return m_AudioService->getFileAnalysisResults(buffer, m_App);
    }
    catch (std::exception e)
    {
        m_App->log("error") << e.what();
        return std::unexpected(s_ServerGenericErrorMessage);
    }
}

std::expected<std::string, std::string>
LambdaSnail::music::ProcessingPage::executeShellCommand(std::string const& command) const
{
    std::array<char, 128> buffer{};
    std::string result{};

    m_App->log("notice") << "Executing shell command: " << command;

    int32_t exitStatus{};
    auto deleter = [&exitStatus, &result](FILE* f) {
        exitStatus = pclose(f);
        if (exitStatus != EXIT_SUCCESS) {
            result = strerror(errno);
        }
    };



    try
    {
        std::unique_ptr<FILE, decltype(deleter)> pipe(popen(command.c_str(), "r"), deleter);
        while (fgets(buffer.data(), sizeof buffer, pipe.get()) != nullptr)
        {
            result.append(buffer.data());
        }
    }
    catch (...)
    {
        return std::unexpected("An error occurred while executing a command");
    }

    if (exitStatus != EXIT_SUCCESS) {
        return std::unexpected(result);
    }

    return result;
}