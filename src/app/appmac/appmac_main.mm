#import <appmac_appdelegate.h>
#import <Cocoa/Cocoa.h>

int main(int argc, char *argv[])
{
    @autoreleasepool {
        [NSApplication sharedApplication];
    
        appmac_AppDelegate* appDelegate = [[[appmac_AppDelegate alloc] init] autorelease];
        [NSApp setDelegate:appDelegate];
        [NSApp run];
    }
    
    return 0;
}
