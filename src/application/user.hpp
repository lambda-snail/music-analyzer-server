#pragma once

#include <Wt/Auth/Dbo/AuthInfo.h>
#include <Wt/Dbo/Types.h>
#include <Wt/WGlobal.h>

namespace LambdaSnail::todo::application
{
    typedef Wt::Auth::Dbo::AuthInfo<class user> auth_info_t;

    class user : public Wt::Dbo::Dbo<user>
    {
    public:

        //std::string name;
        //Wt::WString name;
        Wt::Dbo::weak_ptr<auth_info_t> m_auth_info;
        Wt::WDate registration_date;

        template<class Action>
        void persist(Action& a)
        {
            Wt::Dbo::field(a, registration_date,    "registration_date");
            Wt::Dbo::hasOne(a, m_auth_info,         "user");
        }
    };
}

DBO_EXTERN_TEMPLATES(LambdaSnail::todo::application::user)