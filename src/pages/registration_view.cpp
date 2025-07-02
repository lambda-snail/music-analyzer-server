#include "registration_view.hpp"

#include "models/user_details.hpp"

#include <Wt/WLineEdit.h>

LambdaSnail::todo::pages::registration_view::registration_view(
    application::Session& session, Wt::Auth::AuthWidget* authWidget)
    : Wt::Auth::RegistrationWidget(authWidget), m_session(session)
{
    setTemplateText(tr("template.registration"));
    m_details_model = std::make_unique<models::user_details_model>(m_session);

    updateView(m_details_model.get());
}

std::unique_ptr<Wt::WWidget>
LambdaSnail::todo::pages::registration_view::createFormWidget(Wt::WFormModel::Field field)
{
    return Wt::Auth::RegistrationWidget::createFormWidget(field);

    // if (field == models::user_details_model::s_name_field) {
    //     return std::make_unique<Wt::WLineEdit>();
    // }
    // else
    // {
    //     return Wt::Auth::RegistrationWidget::createFormWidget(field);
    // }
}

bool LambdaSnail::todo::pages::registration_view::validate()
{
    bool result = Wt::Auth::RegistrationWidget::validate();

    updateModel(m_details_model.get());
    if (!m_details_model->validate())
        result = false;
    updateView(m_details_model.get());

    return result;
}

void LambdaSnail::todo::pages::registration_view::registerUserDetails(Wt::Auth::User& user)
{
    m_details_model->save(user);
}