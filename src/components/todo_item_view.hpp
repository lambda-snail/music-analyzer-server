// #pragma once
//
// //#include "../todo.hpp"
//
// #include <Wt/WCheckBox.h>
// #include <Wt/WContainerWidget.h>
//
// namespace LambdaSnail::todo
// {
// struct todo_item;
// }
// class todo_item_view final : public Wt::WContainerWidget
// {
//   public:
//     using id_t = size_t;
//
//     explicit todo_item_view(LambdaSnail::todo::todo_item const& item);
//     explicit todo_item_view(size_t id, std::string const& text, bool is_done = false);
//
//     template <typename TFunction>
//     void register_on_checked(TFunction const& callback) const;
//     template <typename TFunction>
//     void register_on_unchecked(TFunction const& callback) const;
//
//     [[nodiscard]] LambdaSnail::todo::id_t get_id() const { return m_id; }
//
//   private:
//     LambdaSnail::todo::id_t m_id;
//     bool m_is_done{false};
//
//     Wt::WCheckBox* m_checkbox{};
//
// };
//
// template <typename TFunction>
// void todo_item_view::register_on_checked(TFunction const& callback) const
// {
//     m_checkbox->checked().connect(callback);
// }
//
// template <typename TFunction> void todo_item_view::register_on_unchecked(TFunction const& callback) const
// {
//     m_checkbox->unChecked().connect(callback);
// }