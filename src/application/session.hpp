#pragma once

#include <Wt/Auth/Dbo/UserDatabase.h>
#include <Wt/Auth/Login.h>

#include <Wt/Dbo/Session.h>

#include "user.hpp"

namespace LambdaSnail::todo::application
{
    using userdb_t = Wt::Auth::Dbo::UserDatabase<auth_info_t>;

    class Session : public Wt::Dbo::Session
    {
      public:
        explicit Session(std::string const& sqliteDb, Wt::Auth::AuthService& auth_service, Wt::Auth::PasswordService& password_service, std::vector<std::unique_ptr<Wt::Auth::OAuthService>>& oauth_services);

        Wt::Auth::AbstractUserDatabase& users() const;

        Wt::Dbo::ptr<application::user> user();
        Wt::Dbo::ptr<application::user> user(Wt::Auth::User const& user);

        Wt::Auth::AuthService const& auth() const;
        Wt::Auth::PasswordService const& password_auth() const;
        std::vector<const Wt::Auth::OAuthService*> oauth() const;
        Wt::Auth::Login& login();

      private:
        std::unique_ptr<userdb_t> m_userdb;
        Wt::Auth::Login m_login;

        Wt::Auth::AuthService& m_auth_service;
        Wt::Auth::PasswordService& m_password_service;
        std::vector<std::unique_ptr<Wt::Auth::OAuthService>>& m_oauth_services;
    };
}