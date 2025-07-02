#include "processing_page.hpp"

#include "../components/todo_view.hpp"

#include <Wt/WTemplate.h>

LambdaSnail::todo::ProcessingPage::ProcessingPage()
{
    // m_current_item = new todo(10);
    // m_current_item->items.emplace_back(1, "Hello TODO", true);
    // m_current_item->items.emplace_back(2, "Bye TODO", false);

    auto* t = addNew<Wt::WTemplate>(Wt::WString::tr("processing-page"));
    //
    t->bindString("title", "Hello World Processing");
    // t->bindString("description", "A description for this TODO");
    // t->bindString("last-updated", "A few moments ago");

    //t->bindNew<todo_view>("todo", *m_current_item);
}