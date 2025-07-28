#pragma once

#include "models/audio.hpp"

#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WTable.h>

namespace LambdaSnail::music
{
class SongView : public Wt::WContainerWidget
{
  public:
    explicit SongView();

    void addSong(std::unique_ptr<music::AudioInformation>&& songData);
    void removeFile();

    /**
     * Iterate over the columns of the table
     * @param f A callable that accepts (int32_t columnCount, Wt::WTableColumn*) as parameter.s This is
     * invoked once per table column.
     */
    template <typename TFunc>
    void getColumns(TFunc f) const;

    /**
     * Iterate over the rows of the table. The first row will contain the table headers.
     * @param f A callable that accepts (int32_t columnCount, Wt::WTableRow*) as parameters. This is
     * invoked once per table row, or not at all if the table is empty.
     */
    template <typename TFunc>
    void getRows(TFunc f) const;

  private:
    std::vector<std::unique_ptr<music::AudioInformation>> m_Songs{};
    Wt::WTable* m_Table;
};

template <typename TFunc> void SongView::getColumns(TFunc f) const
{
    for (int32_t c = 0; c < m_Table->columnCount(); ++c) {
        f(m_Table->columnCount(), m_Table->columnAt(c));
    }
}
template <typename TFunc> void SongView::getRows(TFunc f) const
{
    for (int32_t r = 0; r < m_Table->rowCount(); ++r) {
        f(m_Table->columnCount(), m_Table->rowAt(r));
    }
}

} // namespace LambdaSnail::music