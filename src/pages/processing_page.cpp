#include "processing_page.hpp"

#include "../components/file_view.hpp"
#include "services/audio_features_service.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>
#include <fstream>

LambdaSnail::todo::ProcessingPage::ProcessingPage(LambdaSnail::music::services::AudioFeaturesService* audioService) : m_AudioService(audioService)
{
    // m_current_item = new todo(10);
    // m_current_item->items.emplace_back(1, "Hello TODO", true);
    // m_current_item->items.emplace_back(2, "Bye TODO", false);

    auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("processing-page"));
    //
    // t->bindString("title", "Hello World Processing");
    // t->bindString("description", "A description for this TODO");
    // t->bindString("last-updated", "A few moments ago");

    // t->bindNew<todo_view>("todo", *m_current_item);


    m_FileView = t->bindNew<FileView>("file-list");

    m_FileDrop = t->bindNew<Wt::WFileDropWidget>("file-drop");
    m_FileDrop->addNew<Wt::WText>("Drop Files Here");
    m_FileDrop->setToolTip("Drop files here or click this component to upload from your hard drive");
    m_FileDrop->removeStyleClass("Wt-filedropzone");
    m_FileDrop->addStyleClass("ls-filedrop");
    m_FileDrop->addStyleClass("d-flex");
    m_FileDrop->addStyleClass("justify-content-center");
    m_FileDrop->setAcceptDirectories(false);


    m_FileDrop->drop().connect([this](std::vector<Wt::WFileDropWidget::File*> const& files) {
        int const maxFiles   = 5;
        unsigned prevNbFiles = m_FileDrop->uploads().size() - files.size();
        for (unsigned i = 0; i < files.size(); i++) {
            if (prevNbFiles + i >= maxFiles) {
                m_FileDrop->cancelUpload(files[i]);
                continue;
            }


            m_FileView->addFile(files[i]);

            // Wt::WContainerWidget* block = m_FileDrop->addNew<Wt::WContainerWidget>();
            // block->setToolTip(files[i]->clientFileName());
            // block->addStyleClass("upload-block spinner");
        }


        if (m_FileDrop->uploads().size() >= maxFiles)
            m_FileDrop->setAcceptDrops(false);
    });

    m_FileDrop->uploaded().connect([this](Wt::WFileDropWidget::File* file) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
        //m_FileDrop->widget(idx)->removeStyleClass("spinner");
        //m_FileDrop->widget(idx)->addStyleClass("ready");

        // Stub to test file processing

        std::string const& serverFileName = file->uploadedFile().spoolFileName();

        std::ifstream stream(serverFileName, std::ios::binary | std::ios::ate);
        if (not stream.is_open())
        {
            std::cout << "I/O error while reading\n";
            return;
        }

        size_t stream_size = stream.tellg();
        std::string buffer(stream_size, '\0');
        stream.seekg(0);

        if (not stream.read(&buffer[0], static_cast<std::streamoff>(stream_size)))
        {
            std::cout << "Error while reading file" << std::endl;
            return;
        }

        stream.close();

        auto analysis = m_AudioService->getFileAnalysisResults(buffer);
        if (analysis) {
            auto const& value = analysis.value();
            std::cout << value.acousticness << std::endl;
            std::cout << value.danceability << std::endl;
            std::cout << value.energy << std::endl;
            std::cout << value.instrumentalness << std::endl;
            std::cout << value.liveness << std::endl;
            std::cout << value.loudness << std::endl;
            std::cout << value.speechiness << std::endl;
            std::cout << value.tempo << std::endl;
            std::cout << value.valence << std::endl;
        }

    });

    m_FileDrop->tooLarge().connect([this](Wt::WFileDropWidget::File* file, uint64_t size) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
        //m_FileDrop->widget(idx)->removeStyleClass("spinner");
        //m_FileDrop->widget(idx)->addStyleClass("failed");
    });

    m_FileDrop->uploadFailed().connect([this](Wt::WFileDropWidget::File* file) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
        //m_FileDrop->widget(idx)->removeStyleClass("spinner");
        //m_FileDrop->widget(idx)->addStyleClass("failed");
    });
}