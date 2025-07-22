#include "processing_page.hpp"

#include "components/process_log.hpp"
#include "components/song_view.hpp"
#include "services/audio_features_service.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WFileDropWidget.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTemplate.h>
#include <Wt/WText.h>

#include <Wt/WApplication.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <cerrno>
#include <cstdio>
#include <fstream>
#include <future>
#include <iostream>
#include <syncstream>
#include <thread>

LambdaSnail::music::ProcessingPage::ProcessingPage(
    LambdaSnail::music::services::AudioFeaturesService* audioService)
    : m_AudioService(audioService), m_App(wApp)
{
    auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("processing-page"));

    m_UrlInput = t->bindNew<Wt::WLineEdit>("yt-input");
    m_UrlInput->addStyleClass("w-25");

    auto* button = t->bindNew<Wt::WPushButton>("yt-button", "Get from YouTube");
    button->setStyleClass("btn");
    button->addStyleClass("primary");
    // button->pre

    // signal_LogAdded.connect([this](ProcessLog* log) {
    //     std::osyncstream(std::cout) << std::this_thread::get_id() << std::endl;
    //     m_LogContainer->addWidget(std::unique_ptr<ProcessLog>(log));
    // });
    //
    // signal_MessageChanged.connect([this](std::string const& message, ProcessLog* log) {
    //     Wt::WServer::instance()->post(m_App->sessionId(),[log, &message]() {
    //         log->updateMessage(message);
    //     });
    // });

    button->clicked().connect([this]() {
        auto const& videoId = m_UrlInput->valueText().toUTF8();
        if (videoId.empty()) {
            return;
        }

        std::osyncstream(std::cout) << std::this_thread::get_id() << std::endl;

        // auto* logger = new ProcessLog(videoId);
        // signal_LogAdded.emit(logger);
        // processYouTubeId(videoId, logger);

        auto* logger = addNewLog(videoId, m_App);
        std::thread([this, logger, videoId]() {

            // app->attachThread(true);
            //Wt::WApplication::UpdateLock lock(app);
            //if (lock) {
                // signal_LogAdded.emit(logger);
            processYouTubeId(videoId, logger);
            //}

            // app->attachThread(false);
        }).detach();
    });

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

    m_LogContainer = t->bindNew<Wt::WContainerWidget>("process-log");
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
        // std::vector<Wt::WFileDropWidget::File*> uploads = m_FileDrop->uploads();
        // std::size_t idx                                 = 0;
        //  for (; idx != uploads.size(); ++idx)
        //      if (uploads[idx] == file)
        //          break;

        // Stub to test file processing

        auto* logger = addNewLog(file->clientFileName(), m_App);

        std::thread([this, logger, file]() {
            m_App->attachThread(true);
            processAudioFile(std::filesystem::path(file->uploadedFile().spoolFileName()), logger);
            m_App->attachThread(false);
        }).detach();

        // auto handle = std::async(std::launch::async, [this, file, logger]() {
        //     processAudioFile(std::filesystem::path(file->uploadedFile().spoolFileName()),
        //     logger);
        // });

        // handle.wait();
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

LambdaSnail::music::ProcessLog*
LambdaSnail::music::ProcessingPage::addNewLog(std::string const& name, Wt::WApplication* app)
{
    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        return m_LogContainer->addNew<ProcessLog>(name, app);
    }

    return nullptr;
}
void LambdaSnail::music::ProcessingPage::processYouTubeId(
    std::string const& videoId, ProcessLog* logger)
{
    logger->updateMessage("Downloading from YouTube, this may take a while ...");

    // signal_MessageChanged.emit("Downloading from YouTube, this may take a while ...", logger);
    auto const downloadResult =
        executeShellCommand(std::format("yt-dlp -o '/tmp/%(title)s.%(ext)s' -q {}", videoId));
    if (not downloadResult.has_value()) {
        // m_App->log("error") << (downloadResult.error());
        logger->updateMessage(downloadResult.error());
        return;
    }

    auto const processResult =
        executeShellCommand(
            std::format("yt-dlp -o '/tmp/%(title)s.%(ext)s' --get-filename {}", videoId))
            .transform([](std::string const&& fileName) {
                if (fileName.ends_with('\n')) {
                    return std::filesystem::path(
                        std::string_view(fileName.cbegin(), --fileName.cend()));
                }

                return std::filesystem::path(fileName);
            })
            .and_then([this, logger](std::filesystem::path&& path) {
                logger->updateAll(
                    path.stem().string(),
                    std::format(
                        "{} downloaded successfully! Converting to mp3 ...", path.filename().string())
                );

                return executeShellCommand(
                           std::format(
                               R"(ffmpeg -i "{}" "{}/{}.mp3" -n -loglevel fatal)", // If file
                                                                                   // exists, we
                                                                                   // just use
                                                                                   // that
                               path.string(),
                               path.parent_path().string(),
                               path.stem().string()))
                    .transform([&path](std::string const&& str) { return path; });
            })
            .and_then([this, logger](std::filesystem::path&& path) {
                path.replace_extension("mp3");
                processAudioFile(path, logger);
                return std::expected<std::filesystem::path, std::string>{};
            });

    if (not processResult.has_value()) {
        //m_App->log("error") << (processResult.error());
        return;
    }
}

void LambdaSnail::music::ProcessingPage::processAudioFile(
    std::filesystem::path const& filePath, ProcessLog* log)
{
    log->updateMessage("Reading file content");

    std::ifstream stream(filePath, std::ios::binary | std::ios::ate);
    if (not stream.is_open()) {
        std::string const message =
            std::format("Error when opening audio file: {}", filePath.string());
        // m_App->log("error") << "Error when opening audio file: " << filePath.string();
        log->updateMessage(message);
        return;
    }

    size_t stream_size = stream.tellg();
    std::string buffer(stream_size, '\0');
    stream.seekg(0);

    if (not stream.read(&buffer[0], static_cast<std::streamoff>(stream_size))) {
        // m_App->log("error") << "Error when reading audio file: " << filePath.string();
        std::string const message =
            std::format("Error when reading audio file: {}", filePath.string());
        log->updateMessage(message);
        return;
    }

    stream.close();

    log->updateMessage("Performing analysis, please wait ...");
    auto const analysis = m_AudioService->getFileAnalysisResults(buffer, m_App);
    if (analysis) {
        auto songInfo = std::make_unique<music::AudioInformation>();

        songInfo->data = analysis.value();
        songInfo->name = filePath.filename();

        Wt::WApplication::UpdateLock uiLock(m_App);
        if (uiLock) {
            m_FileView->addSong(std::move(songInfo));
            m_App->triggerUpdate();
        }

        log->updateMessage("Completed!"); // TODO: Add lockless version for the case where you already have the lock?
    }

    // TODO: Report error here or return std::unexpected
}

std::expected<std::string, std::string>
LambdaSnail::music::ProcessingPage::executeShellCommand(std::string const& command) const
{
    std::array<char, 128> buffer{};
    std::string result{};

    m_App->log("notice") << "Executing shell command: " << command;

    int32_t exitStatus{};
    auto deleter = [&exitStatus, &result](FILE* f) {
        exitStatus = pclose(f);
        if (exitStatus != EXIT_SUCCESS) {
            result = strerror(errno);
        }
    };

    try {
        std::unique_ptr<FILE, decltype(deleter)> pipe(popen(command.c_str(), "r"), deleter);
        while (fgets(buffer.data(), sizeof buffer, pipe.get()) != nullptr) {
            result.append(buffer.data());
        }
    } catch (...) {
        return std::unexpected("An error occurred while executing a command");
    }

    if (exitStatus != EXIT_SUCCESS) {
        return std::unexpected(result);
    }

    return result;
}