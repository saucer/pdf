#include "pdf.impl.hpp"

#include <saucer/invoke.hpp>

namespace saucer::modules
{
    pdf::pdf(webview &webview) : m_impl(std::make_unique<impl>())
    {
        m_impl->parent  = std::addressof(webview.parent().parent());
        m_impl->webview = std::addressof(webview);
    }

    pdf::~pdf() = default;

    void pdf::save(const settings &settings)
    {
        return utils::invoke<&impl::save>(m_impl.get(), settings);
    }
} // namespace saucer::modules
