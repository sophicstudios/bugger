#import <appmac_appdelegate.h>
#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
    @autoreleasepool {
        NSApplication* app = [NSApplication sharedApplication];
    
        appmac_AppDelegate* appDelegate = [[[appmac_AppDelegate alloc] init] autorelease];
        [app setDelegate:appDelegate];
        [app run];
    }
    
    return 0;
}
