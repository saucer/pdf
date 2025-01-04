#include "pdf.hpp"

#include "wv2.webview.impl.hpp"

#include <atomic>

namespace saucer::modules
{
    void pdf::save(const fs::path &file, std::pair<double, double> size)
    {
        if (!m_parent->parent().thread_safe())
        {
            return m_parent->parent().dispatch([this, file, size] { return save(file, size); });
        }

        ComPtr<ICoreWebView2_7> webview;
        ComPtr<ICoreWebView2Environment6> environment;

        if (!SUCCEEDED(m_parent->native<false>()->web_view.As(&webview)))
        {
            return;
        }

        if (ComPtr<ICoreWebView2Environment> env;
            !SUCCEEDED(webview->get_Environment(&env)) || !SUCCEEDED(env.As(&environment)))
        {
            return;
        }

        ComPtr<ICoreWebView2PrintSettings> settings;

        if (!SUCCEEDED(environment->CreatePrintSettings(&settings)))
        {
            return;
        }

        settings->put_PageWidth(size.first);
        settings->put_PageHeight(size.second);

        std::error_code ec{};

        auto path     = fs::absolute(file, ec);
        auto finished = std::atomic_bool{false};

        auto callback = [&](HRESULT, BOOL)
        {
            finished.store(true);
            return S_OK;
        };

        webview->PrintToPdf(path.wstring().c_str(), settings.Get(),
                            Microsoft::WRL::Callback<ICoreWebView2PrintToPdfCompletedHandler>(callback).Get());

        while (!finished)
        {
            m_parent->parent().run<false>();
        }
    }
} // namespace saucer::modules
