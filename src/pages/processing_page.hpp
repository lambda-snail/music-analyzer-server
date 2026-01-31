#pragma once

#include "application/lambda_resource.hpp"
#include "components/song_view.hpp"

#include <Wt/WContainerWidget.h>
#include <expected>
#include <filesystem>

namespace Wt
{
class WFileDropWidget;
}

namespace LambdaSnail::music
{
class ProcessLog;
namespace services
{
class AudioFeaturesService;
}

/**
 * The class responsible for coordinating the processing of music files. In a larger
 * application this should probably be split up into smaller classes, but this application
 * is so small that it's much easier to simply have everything related to processing in
 * here.
 */
class ProcessingPage final : public Wt::WContainerWidget
{
  public:
    void setupYoutubeProcessing(Wt::WTemplate* t);
    void setupFileDrop(Wt::WTemplate* t);
    void setupCsvConversion(Wt::WTemplate* t);
    explicit ProcessingPage(LambdaSnail::music::services::AudioFeaturesService* audioService);

  private:
    Wt::WFileDropWidget* m_FileDrop{};
    Wt::WLineEdit* m_UrlInput{};
    SongView* m_SongView{};

    enum struct MusicSource
    {
        Spotify,
        YouTube
    };
    Wt::WComboBox* m_SourceSelector{};

    music::services::AudioFeaturesService* m_AudioService{};

    WContainerWidget* m_LogContainer{};

    std::shared_ptr<application::LambdaResource> m_DataFile;

    ProcessLog* addNewLog(std::string const& name, Wt::WApplication* app);

    void processYouTubeId(std::string const& videoId, ProcessLog* logger);
    void processSpotifyId(std::string const& spotifyId, ProcessLog* logger);

    void processAudioFile(std::filesystem::path const& filePath, ProcessLog* log);
    [[nodiscard]] std::expected<std::string, std::string>
    executeShellCommand(std::string const& command) const;

    Wt::WApplication* m_App{};

    // Wt::Signal<std::string, ProcessLog*> signal_MessageChanged{};
    // Wt::Signal<ProcessLog*> signal_LogAdded{};
};
} // namespace LambdaSnail::music