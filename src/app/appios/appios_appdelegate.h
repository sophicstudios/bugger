#ifndef INCLUDED_ATTA_APPIOS_APPDELEGATE_H
#define INCLUDED_ATTA_APPIOS_APPDELEGATE_H

#import <UIKit/UIKit.h>

struct appios_AppDelegateImpl;

@interface appios_AppDelegate : UIResponder <UIApplicationDelegate> {
    appios_AppDelegateImpl* m_impl;
}

@end

#endif // INCLUDED
