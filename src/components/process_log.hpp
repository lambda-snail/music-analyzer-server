#pragma once

#include <Wt/WTemplate.h>
#include <mutex>

namespace LambdaSnail::music
{

/**
 * Simple class for displaying progress and error messages to the user. Should probably be split
 * into a model and view or similar, but the project is so small that it's simply easier to have
 * them in oone class.
 */
class ProcessLog final : public Wt::WTemplate
{
  public:
    explicit ProcessLog(std::string const& name, Wt::WApplication* app);

    void updateName(std::string const& name);
    void updateMessage(std::string const& message);

  private:
    bool b_IsDone { false };

    Wt::WText* m_Title;
    Wt::WText* m_Message;

    Wt::WApplication* m_App;
};
} // namespace LambdaSnail::music