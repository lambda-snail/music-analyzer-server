#include "pages/authentication_page.hpp"
#include "services/audio_features_service.hpp"
#include "src/application/session.hpp"
#include "src/components/process_log.hpp"
#include "src/components/song_view.hpp"
#include "src/pages/processing_page.hpp"

#include <Wt/Auth/AuthService.h>
#include <Wt/Auth/AuthWidget.h>
#include <Wt/Auth/FacebookService.h>
#include <Wt/Auth/GoogleService.h>
#include <Wt/Auth/HashFunction.h>
#include <Wt/Auth/PasswordService.h>
#include <Wt/Auth/PasswordStrengthValidator.h>
#include <Wt/Auth/PasswordVerifier.h>
#include <Wt/Dbo/Exception.h>
#include <Wt/WApplication.h>
#include <Wt/WBootstrap5Theme.h>
#include <Wt/WBreak.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WServer.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTemplate.h>
#include <curl/curl.h>

// TODO: Merge into todo application
class todo_application final : public Wt::WApplication
{
  public:
    void setup_auth_page()
    {
        auto auth_page = std::make_unique<LambdaSnail::music::pages::authentication_page>(m_session);

        auth_page->model()->addPasswordAuth(&m_session.password_auth());
        auth_page->model()->addOAuth(m_session.oauth());
        auth_page->setRegistrationEnabled(true);
        auth_page->processEnvironment();

        root()->addWidget(std::move(auth_page));
    }

    explicit todo_application(const Wt::WEnvironment& env,
                             Wt::Auth::AuthService& auth_service,
                             Wt::Auth::PasswordService& password_service,
                             std::vector<std::unique_ptr<Wt::Auth::OAuthService>>& oauth_services,
                             std::unique_ptr<LambdaSnail::music::services::AudioFeaturesService> service)
        : Wt::WApplication(env),
          m_session(appRoot() + "auth.db", auth_service, password_service, oauth_services),
          m_Service(std::move(service))
    {
        m_session.login().changed().connect(this, &todo_application::authEvent);
        WApplication::instance()->internalPathChanged().connect(
            this, &todo_application::handle_internal_path);

        if (m_session.login().loggedIn())
        {
            setInternalPath("/list", true);
        }
        else {
            setup_auth_page();
        }
    }

    void setup_authenticated_content()
    {
        auto t = std::make_unique<Wt::WTemplate>(Wt::WString::tr("main-layout"));

        t->doJavaScript("window.toggleSidebar = function() { const sidebar = "
                        "document.querySelector('.sidebar'); "
                        "sidebar.classList.toggle('collapsed'); }");

        m_widget_stack = t->bindWidget("main-content", std::make_unique<Wt::WStackedWidget>());

        m_todo_page =
            m_widget_stack->addWidget(std::move(std::make_unique<LambdaSnail::music::ProcessingPage>(m_Service.get())));

        root()->addWidget(std::move(t));
    }

    void authEvent()
    {
        if (m_session.login().loggedIn()) {
            const Wt::Auth::User& u = m_session.login().user();
            log("notice") << "User " << u.id() << " (" << u.identity(Wt::Auth::Identity::LoginName)
                          << ")"
                          << " logged in.";

            root()->clear();
            setup_authenticated_content();
            setInternalPath("/list", true);
        }
        else
        {
            root()->clear();
            setup_auth_page();
            log("notice") << "User logged out.";
        }
    }

    void handle_internal_path(std::string const& path)
    {
        if (m_session.login().loggedIn()) {
            if (path == "/list") {
                m_widget_stack->setCurrentWidget(m_todo_page);
            }
            else
            {
                //WApplication::instance()->setInternalPath("/play", true);
            }
        }
    }

  private:
    LambdaSnail::music::application::Session m_session;
    Wt::WStackedWidget* m_widget_stack{};
    LambdaSnail::music::ProcessingPage* m_todo_page{};

    std::unique_ptr<LambdaSnail::music::services::AudioFeaturesService> m_Service;
};

// TODO: Find some better place to put this
void configure_auth(Wt::Auth::AuthService& auth_service,
                    Wt::Auth::PasswordService& password_service,
                    std::vector<std::unique_ptr<Wt::Auth::OAuthService>>& oauth_services)
{
    auth_service.setAuthTokensEnabled(true, "logincookie");
    auth_service.setEmailVerificationEnabled(true);

    // TODO: Only for testing
    auth_service.setEmailVerificationRequired(false);

    auto verifier = std::make_unique<Wt::Auth::PasswordVerifier>();
    verifier->addHashFunction(std::make_unique<Wt::Auth::BCryptHashFunction>(12));
    password_service.setVerifier(std::move(verifier));
    password_service.setPasswordThrottle(std::make_unique<Wt::Auth::AuthThrottle>());
    password_service.setStrengthValidator(std::make_unique<Wt::Auth::PasswordStrengthValidator>());

    if (Wt::Auth::GoogleService::configured()) {
        oauth_services.push_back(std::make_unique<Wt::Auth::GoogleService>(auth_service));
    }

    if (Wt::Auth::FacebookService::configured()) {
        oauth_services.push_back(std::make_unique<Wt::Auth::FacebookService>(auth_service));
    }

    for (const auto& oAuthService : oauth_services) {
        oAuthService->generateRedirectEndpoint();
    }
}

// <label for="${id:favourite-pet}">Favourite pet:
//                     ${name class="Wt-info"}
// </label>
// ${favourite-pet}

int main(int argc, char** argv)
{
    Wt::Auth::AuthService auth_service;
    Wt::Auth::PasswordService password_service(auth_service);
    std::vector<std::unique_ptr<Wt::Auth::OAuthService>> oauth_services;

    curl_global_init(CURL_GLOBAL_ALL);

    try {
        Wt::WServer server{argc, argv, WTHTTP_CONFIGURATION};
        configure_auth(auth_service, password_service, oauth_services);

        server.addEntryPoint(Wt::EntryPointType::Application, [&](const Wt::WEnvironment& env) {
            // auto app = std::make_unique<TodoApplication>(env);
            auto app = std::make_unique<todo_application>(
                env,
                auth_service,
                password_service,
                oauth_services,
                std::make_unique<LambdaSnail::music::services::AudioFeaturesService>());

            app->setTheme(std::make_shared<Wt::WBootstrap5Theme>());
            app->useStyleSheet("resources/style/main-content.css");

            app->messageResourceBundle().use("resources/pages/main-layout");
            app->messageResourceBundle().use("resources/pages/auth");
            app->messageResourceBundle().use("resources/pages/processing");

            app->enableUpdates(true);

            //app->messageResourceBundle().use("resources/components/todo-components");

            // // resolve a string using the resource bundle
            // auto welcome = std::make_unique<Wt::WText>(
            //                  tr("welcome-text").arg("Bart"));

            /*
             * You could read information from the environment to decide whether
             * the user has permission to start a new application
             */
            return std::move(app);
        });

        server.run();
    } catch (Wt::WServer::Exception& e) {
        std::cerr << e.what() << '\n';
    } catch (Wt::Dbo::Exception& e) {
        std::cerr << "Dbo exception: " << e.what() << '\n';
    } catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << '\n';
    }

    curl_global_cleanup();
}