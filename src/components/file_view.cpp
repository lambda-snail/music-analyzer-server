#include <Wt/WAbstractListModel.h>

#include "file_view.hpp"

#include <Wt/WTemplate.h>
#include <bits/ranges_algo.h>

LambdaSnail::todo::FileView::FileView()
{
    m_FileViewComponent = addNew<Wt::WTemplate>(Wt::WString::tr("file-view-component"));


    m_Container = m_FileViewComponent->bindNew<Wt::WContainerWidget>("files");
}

void LambdaSnail::todo::FileView::addFile(Wt::WFileDropWidget::File* file)
{
    m_Files.push_back(file);
//    rebuildFileList();
    auto t = std::make_unique<Wt::WTemplate>(Wt::WString::tr("file-view-component-row"));
    //t->bindWidget("file-name", xxx);
    t->bindString("file-name", file->clientFileName());

    m_Container->addWidget(std::move(t));
}

void LambdaSnail::todo::FileView::removeFile() {}

void LambdaSnail::todo::FileView::addFiles(std::vector<Wt::WFileDropWidget::File*> const& files)
{
    //m_Files.insert(m_Files.end(), files.cbegin(), files.cend());
    //rebuildFileList();
    std::ranges::for_each(m_Files.begin(), m_Files.end(), [this](Wt::WFileDropWidget::File* file) {
        addFile(file);
    });
}

void LambdaSnail::todo::FileView::rebuildFileList()
{
    // auto container = std::make_unique<Wt::WContainerWidget>();

    for (auto const* file : m_Files) {
        auto t = std::make_unique<Wt::WTemplate>(Wt::WString::tr("file-view-component-row"));
        //t->bindWidget("file-name", xxx);
        t->bindString("file-name", file->clientFileName());

        m_Container->addWidget(std::move(t));
    }

    //m_FileViewComponent->clear();
    // m_FileViewComponent->bindWidget("files", std::move(container));
}

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