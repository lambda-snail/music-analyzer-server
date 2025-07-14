#include <Wt/WAbstractListModel.h>

#include "song_view.hpp"

#include "models/audio.hpp"

#include <Wt/WLineEdit.h>
#include <Wt/WTable.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <bits/ranges_algo.h>
#include <ranges>

static std::array<std::string, 10> m_Columns
    {
        "Song",
        "Acousticness",
        "Danceability",
        "Energy",
        "Instrumentalness",
        "Liveness",
        "Loudness",
        "Speechiness",
        "Tempo",
        "Valence"
    };

LambdaSnail::music::SongView::SongView()
{
    m_Table = addNew<Wt::WTable>();
    m_Table->setHeaderCount(1);

    for (auto const&& [i, column] : std::ranges::views::enumerate(m_Columns))
    {
        m_Table->elementAt(0, i)->addNew<Wt::WText>(column);
    }

    m_Table->addStyleClass("table");
    m_Table->toggleStyleClass("stripes", true);
}

void LambdaSnail::music::SongView::addSong(std::unique_ptr<music::AudioInformation>&& songData)
{
    auto const row = m_Table->rowCount();

    m_Table->elementAt(row, 0)->addNew<Wt::WText>(songData->name);
    m_Table->elementAt(row, 1)->addNew<Wt::WText>(std::to_string(songData->data.acousticness));
    m_Table->elementAt(row, 2)->addNew<Wt::WText>(std::to_string(songData->data.danceability));
    m_Table->elementAt(row, 3)->addNew<Wt::WText>(std::to_string(songData->data.energy));
    m_Table->elementAt(row, 4)->addNew<Wt::WText>(std::to_string(songData->data.instrumentalness));
    m_Table->elementAt(row, 5)->addNew<Wt::WText>(std::to_string(songData->data.liveness));
    m_Table->elementAt(row, 6)->addNew<Wt::WText>(std::to_string(songData->data.loudness));
    m_Table->elementAt(row, 7)->addNew<Wt::WText>(std::to_string(songData->data.speechiness));
    m_Table->elementAt(row, 8)->addNew<Wt::WText>(std::to_string(songData->data.tempo));
    m_Table->elementAt(row, 9)->addNew<Wt::WText>(std::to_string(songData->data.valence));

    m_Songs.push_back(std::move(songData));
}

void LambdaSnail::music::SongView::removeFile() {}