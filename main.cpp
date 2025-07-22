#include "application/MusicApplication.hpp"
#include "services/audio_features_service.hpp"

#include <Wt/Auth/AuthWidget.h>
#include <Wt/Dbo/Exception.h>
#include <Wt/WApplication.h>
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WServer.h>
#include <curl/curl.h>

int main(int argc, char** argv)
{
    curl_global_init(CURL_GLOBAL_ALL);

    try {
        Wt::WServer server{argc, argv, WTHTTP_CONFIGURATION};

        server.addEntryPoint(Wt::EntryPointType::Application, [&](const Wt::WEnvironment& env) {
            auto app = std::make_unique<LambdaSnail::music::MusicApplication>(env, std::make_unique<LambdaSnail::music::services::AudioFeaturesService>());

            app->setTheme(std::make_shared<Wt::WBootstrap5Theme>());
            app->useStyleSheet("resources/style/main-content.css");

            app->messageResourceBundle().use("resources/pages/main-layout");
            app->messageResourceBundle().use("resources/pages/processing");

            app->enableUpdates(true);

            return std::move(app);
        });

        server.run();
    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << '\n';
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
    }

    curl_global_cleanup();
}