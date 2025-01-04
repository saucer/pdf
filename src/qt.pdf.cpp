#include "pdf.hpp"

#include "qt.webview.impl.hpp"

#include <atomic>

namespace saucer::modules
{
    void pdf::save(const fs::path &file, std::pair<double, double> size)
    {
        if (!m_parent->parent().thread_safe())
        {
            return m_parent->parent().dispatch([this, file, size] { return save(file, size); });
        }

        auto &webview = m_parent->native<false>()->web_view;
        auto *page    = webview->page();

        auto page_size = QPageSize{{size.first, size.second}, QPageSize::Unit::Inch};
        auto layout    = QPageLayout{page_size, QPageLayout::Orientation::Portrait, QMarginsF{}};

        page->printToPdf(QString::fromStdString(file.string()), layout);

        std::atomic_bool finished{false};
        page->connect(page, &QWebEnginePage::pdfPrintingFinished, [&finished]() { finished.store(true); });

        while (!finished)
        {
            m_parent->parent().run<false>();
        }
    }
} // namespace saucer::modules
