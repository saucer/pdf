#pragma once

#include <saucer/webview.hpp>

#include <cstdint>
#include <filesystem>

namespace saucer::modules
{
    namespace fs = std::filesystem;

    struct pdf
    {
        struct impl;

      public:
        enum class layout : std::uint8_t;

      public:
        struct size;
        struct settings;

      private:
        std::unique_ptr<impl> m_impl;

      public:
        pdf(webview &);

      public:
        ~pdf();

      public:
        void save(const settings &);
    };

    enum class pdf::layout : std::uint8_t
    {
        portrait,
        landscape,
    };

    struct pdf::size
    {
        double w;
        double h;
    };

    struct pdf::settings
    {
        fs::path file;

      public:
        pdf::size size{.w = 8.3, .h = 11.7};
        layout orientation{layout::portrait};
    };
} // namespace saucer::modules
