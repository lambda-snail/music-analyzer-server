#pragma once

#include <Wt/WTemplate.h>
#include <expected>

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

    void updateName(std::string const& name) const;
    void updateMessage(std::string const& message) const;
    /**
     * If you need to update both the name and message, then this function allows to do this without
     * acquiring the application lock twice.
     */
    void updateAll(std::string const& name, std::string const& message) const;

    void setSuccessState() const;
    void setErrorState(std::string const& error) const;

  private:
    bool b_IsDone { false };

    Wt::WText* m_Title;
    Wt::WText* m_Message;

    Wt::WApplication* m_App;
};
} // namespace LambdaSnail::music