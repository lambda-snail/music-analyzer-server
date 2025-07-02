// #include "todo_item_view.hpp"
//
// #include <Wt/WTemplate.h>
//
// todo_item_view::todo_item_view(LambdaSnail::todo::todo_item const& item) :todo_item_view(item.id, item.text, item.is_done) {}
// todo_item_view::todo_item_view(size_t id, std::string const& text, bool is_done) : m_id(id), m_is_done(is_done)
// {
//   auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("todo-item"));
//
//   // <input class="form-check-input me-1" type="checkbox" value="" id="$checkbox-{id}" />
//   m_checkbox = t->bindNew<Wt::WCheckBox>("input-element");
//   m_checkbox->setChecked(is_done);
//   m_checkbox->addStyleClass("form-check-input");
//   m_checkbox->addStyleClass("me-1");
//   m_checkbox->setId("checkbox" + id);
//   m_checkbox->setInline(true);
//
//   t->bindString("text", text);
//   t->bindInt("id", static_cast<int32_t>(id));
// }