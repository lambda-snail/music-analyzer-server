#include "process_log.hpp"

#include <Wt/WApplication.h>
#include <Wt/WText.h>
#include <utility>

LambdaSnail::music::ProcessLog::ProcessLog(std::string const& name, Wt::WApplication* app)
    : Wt::WTemplate(Wt::WString::tr("process-update-log-entry")), m_App(app)
{
    m_Title = bindNew<Wt::WText>("title", name);
    m_Message = bindNew<Wt::WText>("message", "Starting process");

    static_cast<Wt::WTemplate*>(this)->bindString("progress-display", "block");
    static_cast<Wt::WTemplate*>(this)->bindString("success-display", "none");
    static_cast<Wt::WTemplate*>(this)->bindString("error-display", "none");
}

void LambdaSnail::music::ProcessLog::updateName(std::string const& name) const
{
    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Title->setText(name);
        m_App->triggerUpdate();
    }
}

void LambdaSnail::music::ProcessLog::updateMessage(std::string const& message) const
{
    m_App->log("notice") << message;

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Message->setText(message);
        m_App->triggerUpdate();
    }
}
void LambdaSnail::music::ProcessLog::updateAll(std::string const& name,
                                               std::string const& message) const
{
    m_App->log("notice") << message;

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Title->setText(name);
        m_Message->setText(message);
        m_App->triggerUpdate();
    }
}
void LambdaSnail::music::ProcessLog::showProgress()
{
    bindString("progress-display", "block");
    bindString("success-display", "none");
    bindString("error-display", "none");
}

void LambdaSnail::music::ProcessLog::showSuccess()
{
    bindString("progress-display", "none");
    bindString("success-display", "block");
    bindString("error-display", "none");
}

void LambdaSnail::music::ProcessLog::showError()
{
    bindString("progress-display", "none");
    bindString("success-display", "none");
    bindString("error-display", "block");
}

void LambdaSnail::music::ProcessLog::setSuccessState(std::string_view const& message)
{
    m_App->log("notice") << message.data();

    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Message->setText(message.data());
        showSuccess();
        m_App->triggerUpdate();
    }
}

void LambdaSnail::music::ProcessLog::setErrorState(std::string const& error)
{
    m_App->log("error") << error;
    Wt::WApplication::UpdateLock uiLock(m_App);
    if (uiLock) {
        m_Message->setText(error);
        showError();
        m_App->triggerUpdate();
    }
}