#include "wk.pdf.impl.hpp"

@implementation PrintDelegate
- (instancetype)initWithCallback:(void (^)())closure
{
    self           = [super init];
    self->callback = closure;

    return self;
}
- (void)printOperationDidRun:(NSPrintOperation *)printOperation success:(BOOL)success contextInfo:(void *)contextInfo
{
    std::invoke(callback);
}
@end
