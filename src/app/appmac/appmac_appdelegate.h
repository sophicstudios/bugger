#ifndef INCLUDED_ATTA_APPMAC_APPDELEGATE_H
#define INCLUDED_ATTA_APPMAC_APPDELEGATE_H

#include <AppKit/AppKit.h>

struct appmac_AppDelegateImpl;

@interface appmac_AppDelegate : NSObject <NSApplicationDelegate> {
    appmac_AppDelegateImpl* m_impl;
}

- (id) init;
- (void) dealloc;

@end

#endif // INCLUDED
