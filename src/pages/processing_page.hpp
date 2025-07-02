#pragma once

#include "components/file_view.hpp"

#include <Wt/WContainerWidget.h>

namespace Wt
{
    class WFileDropWidget;
}

namespace LambdaSnail::todo
{
    class ProcessingPage : public Wt::WContainerWidget
    {
    public:
        explicit ProcessingPage();

    private:
        Wt::WFileDropWidget* m_FileDrop{};
        FileView* m_FileView;
    };
}