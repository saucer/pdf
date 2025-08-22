#pragma once

#include "pdf.impl.hpp"

#import <Cocoa/Cocoa.h>

@class PrintDelegate;

@interface PrintDelegate : NSObject
{
  @public
    void (^callback)();
}
- (instancetype)initWithCallback:(void (^)())closure;
- (void)printOperationDidRun:(NSPrintOperation *)printOperation success:(BOOL)success contextInfo:(void *)contextInfo;
@end
