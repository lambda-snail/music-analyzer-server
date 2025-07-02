#pragma once

#include <Wt/WFormModel.h>

#include "application/session.hpp"

namespace LambdaSnail::todo::models
{
    class user_details_model : public Wt::WFormModel
    {
    public:
        static const Field s_name_field;

        explicit user_details_model(application::Session& session);

        void save(const Wt::Auth::User& user);

    private:
        application::Session& m_session;
    };
}