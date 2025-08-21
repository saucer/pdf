#include "pdf.impl.hpp"

#include <saucer/gtk.utils.hpp>
#include <saucer/gtk.app.impl.hpp>
#include <saucer/wkg.webview.impl.hpp>

namespace saucer::modules
{
    using paper_size_handle = utils::handle<GtkPaperSize *, gtk_paper_size_free>;

    void pdf::impl::save(const settings &settings) // NOLINT(*-function-const)
    {
        auto *web_view = webview->native<false>()->platform->web_view;

        auto operation      = utils::g_object_ptr<WebKitPrintOperation>{webkit_print_operation_new(web_view)};
        auto print_settings = utils::g_object_ptr<GtkPrintSettings>{gtk_print_settings_new()};

        gtk_print_settings_set_printer(print_settings.get(), "Print to File");
        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_FILE_FORMAT, "pdf");

        std::error_code ec{};

        if (auto parent = settings.file.parent_path(); !fs::exists(parent, ec))
        {
            fs::create_directories(parent, ec);
        }

        const auto canonical = fs::weakly_canonical(settings.file.parent_path(), ec);
        const auto filename  = settings.file.filename().replace_extension();

        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_DIR, canonical.c_str());
        gtk_print_settings_set(print_settings.get(), GTK_PRINT_SETTINGS_OUTPUT_BASENAME, filename.c_str());

        webkit_print_operation_set_print_settings(operation.get(), print_settings.get());

        auto [width, height] = settings.size;
        auto paper_size      = paper_size_handle{gtk_paper_size_new_custom("", "", width, height, GTK_UNIT_INCH)};
        auto setup           = utils::g_object_ptr<GtkPageSetup>{gtk_page_setup_new()};

        gtk_page_setup_set_top_margin(setup.get(), 0, GTK_UNIT_INCH);
        gtk_page_setup_set_bottom_margin(setup.get(), 0, GTK_UNIT_INCH);

        gtk_page_setup_set_left_margin(setup.get(), 0, GTK_UNIT_INCH);
        gtk_page_setup_set_right_margin(setup.get(), 0, GTK_UNIT_INCH);

        gtk_page_setup_set_paper_size(setup.get(), paper_size.get());
        webkit_print_operation_set_page_setup(operation.get(), setup.get());

        gtk_page_setup_set_orientation(setup.get(), settings.orientation == layout::landscape ? GTK_PAGE_ORIENTATION_LANDSCAPE
                                                                                              : GTK_PAGE_ORIENTATION_PORTRAIT);

        auto finished = false;
        auto callback = [](void *, bool *finished)
        {
            *finished = true;
        };

        g_signal_connect(operation.get(), "finished", G_CALLBACK(+callback), &finished);
        webkit_print_operation_print(operation.get());

        while (!finished)
        {
            parent->native<false>()->platform->iteration();
        }
    }
} // namespace saucer::modules
