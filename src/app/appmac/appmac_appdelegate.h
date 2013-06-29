// appmac_appdelegate.h

struct appmac_AppDelegateImpl;

@interface appmac_AppDelegate : NSObject <NSApplicationDelegate> {
    appmac_AppDelegateImpl* m_impl;
}

- (id) init;
- (void) dealloc;

@end
