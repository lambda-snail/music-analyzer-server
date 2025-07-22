#include "process_log.hpp"

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <utility>

LambdaSnail::music::ProcessLog::ProcessLog(std::string const& name, Wt::WApplication* app)
    : Wt::WTemplate(Wt::WString::tr("process-update-log-entry")), m_App(app)
{
    m_Title = bindNew<Wt::WText>("title", name);
    m_Message = bindNew<Wt::WText>("message", "Starting process");
}

void LambdaSnail::music::ProcessLog::updateName(std::string const& name)
{
    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Title->setText(name);
        m_App->triggerUpdate();
    }
}

void LambdaSnail::music::ProcessLog::updateMessage(std::string const& message)
{
    m_App->log("notice") << message;

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Message->setText(message);
        m_App->triggerUpdate();
    }
}