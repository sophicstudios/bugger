#import <Cocoa/Cocoa.h>
#import <appmac_appdelegate.h>

int main(int argc, char *argv[])
{
    [NSApplication sharedApplication];
    
    @autoreleasepool {
        appmac_AppDelegate* appDelegate = [[appmac_AppDelegate alloc] init];
        [NSApp setDelegate:appDelegate];
        [NSApp run];
        [appDelegate release];
    }
    
    return 0;
}
