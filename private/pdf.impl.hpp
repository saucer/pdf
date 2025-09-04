#pragma once

#include <saucer/modules/pdf.hpp>

namespace saucer::modules
{
    struct pdf::impl
    {
        application *parent;

      public:
        webview *webview;

      public:
        void save(const settings &);
    };
} // namespace saucer::modules
