#include "application/MusicApplication.hpp"
#include "services/audio_features_service.hpp"
#include "services/cookie_info.hpp"

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

        // To simplify things, we look for the presence of a cookie file with hard coded name, rather than try
        // to parse the program arguments (which would give more flexibility, but right now we don't need that
        if (LambdaSnail::services::CookieInfo::hasCookieFile()) {
            server.log("info") << "Using cookies from file: " << LambdaSnail::services::CookieInfo::getCookieFile().string();
        }
        else {
            server.log("info") << "Proceeding without cookie file";
        }

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