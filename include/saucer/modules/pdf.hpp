#pragma once

#include <saucer/webview.hpp>

#include <utility>
#include <filesystem>

namespace saucer::modules
{
    namespace fs = std::filesystem;

    class pdf
    {
        saucer::webview *m_parent;

      public:
        pdf(saucer::webview *parent);

      public:
        void save(const fs::path &file, std::pair<double, double> size = {8.3, 11.7});
    };
} // namespace saucer::modules
