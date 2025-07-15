#pragma once

#include <Wt/WTemplate.h>
#include <mutex>

namespace LambdaSnail::music
{

/**
 * Simple class for displaying progress and error messages to the user. Should probably be split into
 * a model and view or similar, but the project is so small that it's simply easier to have them in oone
 * class.
 */
class ProcessLog final : public Wt::WTemplate
{
  public:
    explicit ProcessLog(std::string name);

    void updateName(std::string const& name);
    void updateMessage(std::string const& message);

  private:
    std::string m_SongName;
    std::string m_CurrentMessage { "Initializing job ..." };
    bool b_IsDone{false};

    // The log will be updated from event callbacks. They should only be
    // updated by one thread at a time, but just to be sure we lock the log first
    std::mutex m_Mutex{};

    void bindName();
    void bindMessage();
};
} // namespace LambdaSnail::music