#import <appmac_appdelegate.h>
#import <agtg_gl.h>
#import <Cocoa/Cocoa.h>
#import <exception>
#import <iostream>

int main(int argc, char *argv[])
{
    try {
        @autoreleasepool {
            NSApplication* app = [NSApplication sharedApplication];
        
            appmac_AppDelegate* appDelegate = [[[appmac_AppDelegate alloc] init] autorelease];
            [app setDelegate:appDelegate];
            [app run];
        }
    } catch (std::exception const& e) {
        std::cerr << "Exception! " << e.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception!" << std::endl;
        return -1;
    }
    
    return 0;
}
