#include "process_log.hpp"

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <utility>

LambdaSnail::music::ProcessLog::ProcessLog(std::string name, Wt::WApplication* app)
    : Wt::WTemplate(Wt::WString::tr("process-update-log-entry")), m_SongName(std::move(name)), m_App(app)
{
    m_Title = bindNew<Wt::WText>("title", m_SongName);
    m_Message = bindNew<Wt::WText>("message", m_CurrentMessage);

    // bindName();
    // bindMessage();
}

void LambdaSnail::music::ProcessLog::updateName(std::string const& name)
{
    std::unique_lock lock { m_Mutex };

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Title->setText(name);
        m_App->triggerUpdate();
    }

    //m_Message->refresh();
    //m_Title->setText(name);
    //refresh();
    //m_SongName = name;
    //bindName();
}

void LambdaSnail::music::ProcessLog::updateMessage(std::string const& message)
{
    std::unique_lock lock { m_Mutex };

    m_App->log("notice") << "New message: " << message;

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Message->setText(message);
        m_App->triggerUpdate();
    }

    //m_Message->refresh();
    //refresh();
    ///wApp->refresh();
    // m_CurrentMessage = message;
    // bindMessage();
}

void LambdaSnail::music::ProcessLog::bindName()
{
    bindString("title", m_SongName);
}

void LambdaSnail::music::ProcessLog::bindMessage()
{
    bindString("message", m_CurrentMessage);
}