//
// //#include <Wt/WAbstractListModel.h>
//
// #include "todo_view.hpp"
// #include "todo_item_view.hpp"
//
// #include <Wt/WTemplate.h>
//
// LambdaSnail::todo::todo_view::todo_view(todo& item) : m_item(item)
// {
//     auto container = std::make_unique<Wt::WContainerWidget>();
//
//     for (auto const& todo : item.items) {
//         auto* view = container->addNew<todo_item_view>(todo.id, todo.text, todo.is_done);
//
//         view->register_on_checked([]() { std::cout << "checked" << std::endl; });
//         view->register_on_unchecked([]() { std::cout << "UNchecked" << std::endl; });
//
//         m_todo_views.push_back( view );
//     }
//
//     auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("todo-list"));
//     t->bindWidget("items", std::move(container));
// }
//
// void LambdaSnail::todo::todo_view::add_item(todo_item_view* item)
// {
//
// }
//
// void LambdaSnail::todo::todo_view::remove_item(todo_item_view::id_t id)
// {
//     auto const model_it = std::ranges::find_if(m_item.items.begin(), m_item.items.end(), [id](todo_item const& item) { return item.id == id; });
//     auto const view_it = std::ranges::find_if(m_todo_views.begin(), m_todo_views.end(), [id](todo_item_view const* view) { return true; });
//
//     if (model_it == m_item.items.end() or view_it == m_todo_views.end()) {
//         return;
//     }
//
//     m_item.items.erase(model_it);
//     m_todo_views.erase(view_it);
//
//     refresh();
// }