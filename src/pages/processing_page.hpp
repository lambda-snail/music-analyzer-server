#pragma once

#include "components/file_view.hpp"

#include <Wt/WContainerWidget.h>

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
    class ProcessingPage : public Wt::WContainerWidget
    {
    public:
        explicit ProcessingPage(LambdaSnail::music::services::AudioFeaturesService* audioService);

    private:
        Wt::WFileDropWidget* m_FileDrop{};
        FileView* m_FileView;

        LambdaSnail::music::services::AudioFeaturesService* m_AudioService;
    };
}