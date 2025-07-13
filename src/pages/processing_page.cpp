#include "processing_page.hpp"

#include "components/song_view.hpp"
#include "services/audio_features_service.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include <fstream>

LambdaSnail::todo::ProcessingPage::ProcessingPage(
    LambdaSnail::music::services::AudioFeaturesService* audioService)
    : m_AudioService(audioService)
{
    auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("processing-page"));

    m_UrlInput = t->bindNew<Wt::WLineEdit>("yt-input");
    m_UrlInput->addStyleClass("w-25");

    m_FileView = t->bindNew<SongView>("file-list");

    m_FileDrop = t->bindNew<Wt::WFileDropWidget>("file-drop");
    m_FileDrop->addNew<Wt::WText>("Drop Files Here");
    m_FileDrop->setToolTip(
        "Drop files here or click this component to upload from your hard drive");
    m_FileDrop->removeStyleClass("Wt-filedropzone");
    m_FileDrop->addStyleClass("ls-filedrop");
    m_FileDrop->addStyleClass("d-flex");
    m_FileDrop->addStyleClass("justify-content-center");
    m_FileDrop->setAcceptDirectories(false);

    // m_FileDrop->drop().connect([this](std::vector<Wt::WFileDropWidget::File*> const& files) {
    //     int const maxFiles   = 5;
    //     unsigned prevNbFiles = m_FileDrop->uploads().size() - files.size();
    //     for (unsigned i = 0; i < files.size(); i++) {
    //         if (prevNbFiles + i >= maxFiles) {
    //             m_FileDrop->cancelUpload(files[i]);
    //         }
    //     }
    //
    //     if (m_FileDrop->uploads().size() >= maxFiles)
    //         m_FileDrop->setAcceptDrops(false);
    // });

    m_FileDrop->uploaded().connect([this](Wt::WFileDropWidget::File* file) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;

        // Stub to test file processing

        processAudioFile(std::filesystem::path(file->uploadedFile().spoolFileName()));
    });

    m_FileDrop->tooLarge().connect([this](Wt::WFileDropWidget::File* file, uint64_t size) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
        // m_FileDrop->widget(idx)->removeStyleClass("spinner");
        // m_FileDrop->widget(idx)->addStyleClass("failed");
    });

    m_FileDrop->uploadFailed().connect([this](Wt::WFileDropWidget::File* file) {
        std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        std::size_t idx                                 = 0;
        for (; idx != uploads.size(); ++idx)
            if (uploads[idx] == file)
                break;
        // m_FileDrop->widget(idx)->removeStyleClass("spinner");
        // m_FileDrop->widget(idx)->addStyleClass("failed");
    });
}

void LambdaSnail::todo::ProcessingPage::processAudioFile(std::filesystem::path const& filePath)
{
    std::ifstream stream(filePath, std::ios::binary | std::ios::ate);
    if (not stream.is_open()) {
        std::cout << "I/O error while reading\n";
        return;
    }

    size_t stream_size = stream.tellg();
    std::string buffer(stream_size, '\0');
    stream.seekg(0);

    if (not stream.read(&buffer[0], static_cast<std::streamoff>(stream_size))) {
        std::cout << "Error while reading file" << std::endl;
        return;
    }

    stream.close();

    auto analysis = m_AudioService->getFileAnalysisResults(buffer);
    if (analysis) {
        auto songInfo = std::make_unique<music::AudioInformation>();

        songInfo->data = analysis.value();
        songInfo->name = filePath.filename();

        m_FileView->addSong(std::move(songInfo));
    }
}