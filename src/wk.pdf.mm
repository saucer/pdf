#include "wk.pdf.impl.hpp"

#include <saucer/cocoa.utils.hpp>
#include <saucer/cocoa.app.impl.hpp>
#include <saucer/wk.webview.impl.hpp>
#include <saucer/cocoa.window.impl.hpp>

namespace saucer::modules
{
    void pdf::impl::save(const settings &settings) // NOLINT(*-function-const)
    {
        const auto guard     = utils::autorelease_guard{};
        auto *const web_view = webview->native<false>()->platform->web_view.get();
        auto *const window   = webview->parent().native<false>()->platform->window;

        auto *const info     = [NSPrintInfo sharedPrintInfo];
        auto [width, height] = settings.size;

        info.paperSize   = NSMakeSize(width * 72, height * 72);
        info.orientation = settings.orientation == layout::landscape ? NSPaperOrientationLandscape : NSPaperOrientationPortrait;

        std::error_code ec{};

        if (auto parent = settings.file.parent_path(); !fs::exists(parent, ec))
        {
            fs::create_directories(parent, ec);
        }

        auto path       = fs::weakly_canonical(settings.file, ec);
        auto *const url = [NSURL fileURLWithPath:[NSString stringWithUTF8String:path.c_str()]];

        [info.dictionary setValue:NSPrintSaveJob forKey:NSPrintJobDisposition];
        [info.dictionary setValue:url forKey:NSPrintJobSavingURL];

        auto *const operation = [web_view printOperationWithPrintInfo:info];

        operation.showsPrintPanel    = false;
        operation.showsProgressPanel = false;

        operation.view.frame = NSMakeRect(0, 0, info.paperSize.width, info.paperSize.height);

        auto finished        = false;
        auto *const delegate = [[[PrintDelegate alloc] initWithCallback:[&]
                                                       {
                                                           finished = true;
                                                       }] autorelease];

        [operation runOperationModalForWindow:window
                                     delegate:delegate
                               didRunSelector:@selector(printOperationDidRun:success:contextInfo:)
                                  contextInfo:nullptr];

        while (!finished) // NOLINT(*-infinite-loop)
        {
            parent->native<false>()->platform->iteration();
        }
    }
} // namespace saucer::modules
