#include "pdf.impl.hpp"

#include <saucer/win32.app.impl.hpp>
#include <saucer/wv2.webview.impl.hpp>

namespace saucer::modules
{
    void pdf::impl::save(const settings &settings) // NOLINT(*-function-const)
    {
        auto *const web_view = webview->native<false>()->platform->web_view.Get();
        auto environment     = ComPtr<ICoreWebView2Environment6>{};

        if (ComPtr<ICoreWebView2Environment> env; !SUCCEEDED(web_view->get_Environment(&env)) || !SUCCEEDED(env.As(&environment)))
        {
            return;
        }

        ComPtr<ICoreWebView2PrintSettings> print_settings;

        if (!SUCCEEDED(environment->CreatePrintSettings(&print_settings)))
        {
            return;
        }

        auto [width, height] = settings.size;
        auto orientation     = settings.orientation == layout::landscape ? COREWEBVIEW2_PRINT_ORIENTATION_LANDSCAPE
                                                                         : COREWEBVIEW2_PRINT_ORIENTATION_PORTRAIT;

        print_settings->put_PageWidth(width);
        print_settings->put_PageHeight(height);
        print_settings->put_Orientation(orientation);

        std::error_code ec{};

        if (auto parent = settings.file.parent_path(); !fs::exists(parent, ec))
        {
            fs::create_directories(parent, ec);
        }

        auto finished          = false;
        auto complete_callback = [&](HRESULT, BOOL)
        {
            finished = true;
            return S_OK;
        };

        auto path     = fs::weakly_canonical(settings.file, ec);
        auto callback = Microsoft::WRL::Callback<ICoreWebView2PrintToPdfCompletedHandler>(complete_callback);

        web_view->PrintToPdf(path.wstring().c_str(), print_settings.Get(), callback.Get());

        while (!finished)
        {
            parent->native<false>()->platform->iteration();
        }
    }
} // namespace saucer::modules
