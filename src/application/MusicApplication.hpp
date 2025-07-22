#pragma once

#include "components/song_view.hpp"
#include "pages/processing_page.hpp"
#include "services/audio_features_service.hpp"

#include <Wt/WContainerWidget.h>
#include <Wt/WStackedWidget.h>
#include <Wt/WTemplate.h>

namespace LambdaSnail::music
{
class MusicApplication final : public Wt::WApplication
{
  public:
    explicit MusicApplication(
        const Wt::WEnvironment& env,
        std::unique_ptr<LambdaSnail::music::services::AudioFeaturesService> service)
        : Wt::WApplication(env), m_Service(std::move(service))
    {
        auto t = std::make_unique<Wt::WTemplate>(Wt::WString::tr("main-layout"));

        t->doJavaScript("window.toggleSidebar = function() { const sidebar = "
                        "document.querySelector('.sidebar'); "
                        "sidebar.classList.toggle('collapsed'); }");

        m_WidgetStack = t->bindWidget("main-content", std::make_unique<Wt::WStackedWidget>());

        m_MainPage = m_WidgetStack->addWidget(
            std::move(std::make_unique<LambdaSnail::music::ProcessingPage>(m_Service.get())));

        root()->addWidget(std::move(t));
    }

  private:
    Wt::WStackedWidget* m_WidgetStack{};
    LambdaSnail::music::ProcessingPage* m_MainPage{};

    std::unique_ptr<LambdaSnail::music::services::AudioFeaturesService> m_Service;
};
} // namespace LambdaSnail::music