#pragma once

#include "components/song_view.hpp"

#include <Wt/WContainerWidget.h>
#include <expected>
#include <filesystem>

namespace LambdaSnail::music::services
{
class AudioFeaturesService;
}

namespace Wt
{
    class WFileDropWidget;
}

namespace LambdaSnail::todo
{
    class ProcessingPage final : public Wt::WContainerWidget
    {
    public:
        explicit ProcessingPage(LambdaSnail::music::services::AudioFeaturesService* audioService);

    private:
        Wt::WFileDropWidget* m_FileDrop{};
        Wt::WLineEdit* m_UrlInput{};
        SongView* m_FileView{};

        music::services::AudioFeaturesService* m_AudioService{};

        void processAudioFile(std::filesystem::path const& filePath);
        [[nodiscard]] std::expected<std::string, std::string>
        executeShellCommand(std::string const& command) const;
    };
}