#include "pdf.hpp"

#include "wkg.webview.impl.hpp"
#include "handle.hpp"

namespace saucer::modules
{
    using paper_size_handle = utils::handle<GtkPaperSize *, gtk_paper_size_free>;

    void pdf::save(const fs::path &file, std::pair<double, double> size)
    {
        if (!m_parent->parent().thread_safe())
        {
            return m_parent->parent().dispatch([this, file, size] { return save(file, size); });
        }

        auto *webview = m_parent->native<false>()->web_view;

        auto operation      = utils::g_object_ptr<WebKitPrintOperation>{webkit_print_operation_new(webview)};
        auto print_settings = utils::g_object_ptr<GtkPrintSettings>{gtk_print_settings_new()};

        gtk_print_settings_set_printer(print_settings.get(), "Print to File");
        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT, "pdf");

        const auto parent    = file.parent_path();
        const auto file_name = file.filename().replace_extension();

        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_DIR, parent.c_str());
        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_BASENAME, file_name.c_str());

        webkit_print_operation_set_print_settings(operation.get(), print_settings.get());

        auto [width, height] = size;
        auto paper_size      = paper_size_handle{gtk_paper_size_new_custom("", "", width, height, GTK_UNIT_INCH)};
        auto setup           = utils::g_object_ptr<GtkPageSetup>{gtk_page_setup_new()};

        gtk_page_setup_set_paper_size(setup.get(), paper_size.get());
        webkit_print_operation_set_page_setup(operation.get(), setup.get());

        webkit_print_operation_print(operation.get());
    }
} // namespace saucer::modules
