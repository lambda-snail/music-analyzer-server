#pragma once

#include "models/audio.hpp"

#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>

namespace LambdaSnail::todo
{
    class SongView : public Wt::WContainerWidget
    {
    public:
        explicit SongView();

        void addSong(std::unique_ptr<music::AudioInformation>&& songData);
        void removeFile();

      private:
        std::vector<std::unique_ptr<music::AudioInformation>> m_Songs{};
        Wt::WTable* m_Table;
    };
}