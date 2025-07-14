#pragma once

#include "application/session.hpp"
#include "models/user_details.hpp"

#include <Wt/Auth/RegistrationWidget.h>

namespace LambdaSnail::music::pages
{
    class registration_view : public Wt::Auth::RegistrationWidget
    {
      public:
        explicit registration_view(application::Session& session, Wt::Auth::AuthWidget* authWidget = nullptr);

        std::unique_ptr<WWidget> createFormWidget(Wt::WFormModel::Field field) override;

      protected:
        bool validate() override;
        void registerUserDetails(Wt::Auth::User& user) override;

      private:
        application::Session& m_session;

        std::unique_ptr<models::user_details_model> m_details_model;
    };
}