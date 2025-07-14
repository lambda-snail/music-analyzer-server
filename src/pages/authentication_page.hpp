#pragma once

#include <Wt/Auth/AuthWidget.h>

#include "application/session.hpp"


namespace LambdaSnail::music::pages
{
    class authentication_page : public Wt::Auth::AuthWidget
    {
    public:
        explicit authentication_page(application::Session& session);

        std::unique_ptr<WWidget> createRegistrationView(const Wt::Auth::Identity& id) override;

    private:
        application::Session& m_session;
    };
}