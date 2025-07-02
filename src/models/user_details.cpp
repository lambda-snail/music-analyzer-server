#include "user_details.hpp"


#include "application/user.hpp"
#include "user_details.hpp"

#include <Wt/Dbo/Dbo.h>

// https://www.webtoolkit.eu/wt/doc/tutorial/auth.html
// https://github.com/emweb/wt/blob/master/examples/feature/auth1
// https://github.com/emweb/wt/blob/master/examples/feature/auth2

// const Wt::WFormModel::Field
// LambdaSnail::todo::models::user_details_model::s_name_field = "name";

LambdaSnail::todo::models::user_details_model::user_details_model(application::Session& session)
  : WFormModel(),
    m_session(session)
{
    //addField(s_name_field, Wt::WString("Name")); //WString::tr("favourite-pet-info"));
}

void LambdaSnail::todo::models::user_details_model::save(Wt::Auth::User const& auth_user)
{
    Wt::Dbo::ptr<application::user> user = m_session.user(auth_user);
    user.modify()->registration_date = Wt::WDate::currentDate();  //valueText(s_name_field).toUTF8();
}