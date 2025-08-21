#include "pdf.impl.hpp"

#include <saucer/qt.app.impl.hpp>
#include <saucer/qt.webview.impl.hpp>

namespace saucer::modules
{
    void pdf::impl::save(const settings &settings) // NOLINT(*-function-const)
    {
        using enum QPageLayout::Orientation;

        auto *const web_view = webview->native<false>()->platform->web_view;
        auto *const page     = web_view->page();

        auto [width, height] = settings.size;
        auto page_size       = QPageSize{{width, height}, QPageSize::Unit::Inch};
        auto orientation     = settings.orientation == layout::landscape ? Landscape : Portrait;

        std::error_code ec{};

        if (auto parent = settings.file.parent_path(); !fs::exists(parent, ec))
        {
            fs::create_directories(parent, ec);
        }

        auto path   = fs::weakly_canonical(settings.file, ec);
        auto layout = QPageLayout{page_size, orientation, QMarginsF{}};

        page->printToPdf(QString::fromStdString(path.string()), layout);

        bool finished{false};
        page->connect(page, &QWebEnginePage::pdfPrintingFinished, [&finished]() { finished = true; });

        while (!finished)
        {
            parent->native<false>()->platform->iteration();
        }
    }
} // namespace saucer::modules
