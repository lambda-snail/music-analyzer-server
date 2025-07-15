#pragma once

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

class ProcessingPage final : public Wt::WContainerWidget
{
  public:
    explicit ProcessingPage(LambdaSnail::music::services::AudioFeaturesService* audioService);

  private:
    Wt::WFileDropWidget* m_FileDrop{};
    Wt::WLineEdit* m_UrlInput{};
    SongView* m_FileView{};

    music::services::AudioFeaturesService* m_AudioService{};

    std::mutex m_LogContainerMutex{};
    WContainerWidget* m_LogContainer{};
    ProcessLog* addNewLog(std::string const& name, Wt::WApplication* app);

    void processYouTubeId(std::string const& videoId, ProcessLog* logger);
    void processAudioFile(std::filesystem::path const& filePath, ProcessLog* log);
    [[nodiscard]] std::expected<std::string, std::string>
    executeShellCommand(std::string const& command) const;

    Wt::WApplication* m_App{};

    // Wt::Signal<std::string, ProcessLog*> signal_MessageChanged{};
    // Wt::Signal<ProcessLog*> signal_LogAdded{};
};
} // namespace LambdaSnail::music