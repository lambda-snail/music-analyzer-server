#pragma once

#include <vector>

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>

namespace LambdaSnail::todo
{
    class FileView : public Wt::WContainerWidget
    {
    public:
        explicit FileView();

        void addFile(Wt::WFileDropWidget::File* file);
        void removeFile();

        void addFiles(std::vector<Wt::WFileDropWidget::File*> const& files);

      private:
        std::vector<Wt::WFileDropWidget::File*> m_Files{};

        Wt::WTemplate* m_FileViewComponent;
        WContainerWidget* m_Container;

        void rebuildFileList();
    };
}