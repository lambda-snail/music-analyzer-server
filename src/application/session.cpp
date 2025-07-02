#include <Wt/Dbo/backend/Sqlite3.h>

#include "session.hpp"

LambdaSnail::todo::application::Session::Session(
    std::string const& sqliteDb,
    Wt::Auth::AuthService& auth_service,
    Wt::Auth::PasswordService& password_service,
    std::vector<std::unique_ptr<Wt::Auth::OAuthService>>& oauth_services)
    :   m_auth_service(auth_service),
        m_password_service(password_service),
        m_oauth_services(oauth_services)
{
    auto connection = std::make_unique<Wt::Dbo::backend::Sqlite3>(sqliteDb);
    //connection->setProperty("show-queries", "true");
    setConnection(std::move(connection));

    mapClass<application::user>("user");
    mapClass<auth_info_t>("auth_info");
    mapClass<auth_info_t::AuthIdentityType>("auth_identity");
    mapClass<auth_info_t::AuthTokenType>("auth_token");

    try {
        createTables();
        std::cerr << "Created database.\n";
    } catch (Wt::Dbo::Exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << "Using existing database\n";
    }

    m_userdb = std::make_unique<userdb_t>(*this);
}

Wt::Auth::Login& LambdaSnail::todo::application::Session::login()
{
    return m_login;
}

Wt::Auth::AbstractUserDatabase& LambdaSnail::todo::application::Session::users() const
{
    return *m_userdb;
}
Wt::Dbo::ptr<LambdaSnail::todo::application::user>
LambdaSnail::todo::application::Session::user()
{
    if (m_login.loggedIn())
        return user(m_login.user());
    else
        return {};
}

Wt::Dbo::ptr<LambdaSnail::todo::application::user>
LambdaSnail::todo::application::Session::user(Wt::Auth::User const& db_user)
{
    Wt::Dbo::ptr<auth_info_t> auth_info = m_userdb->find(db_user);
    Wt::Dbo::ptr<application::user> user = auth_info->user();

    if (!user) {
        user = add(std::make_unique<application::user>());
        auth_info.modify()->setUser(user);
    }

    return user;
}

const Wt::Auth::AuthService& LambdaSnail::todo::application::Session::auth() const
{
    return m_auth_service;
}

const Wt::Auth::PasswordService& LambdaSnail::todo::application::Session::password_auth() const
{
    return m_password_service;
}

std::vector<const Wt::Auth::OAuthService *> LambdaSnail::todo::application::Session::oauth() const
{
    std::vector<const Wt::Auth::OAuthService*> result;
    result.reserve(m_oauth_services.size());
    for (const auto& auth : m_oauth_services) {
        result.push_back(auth.get());
    }
    return result;
}