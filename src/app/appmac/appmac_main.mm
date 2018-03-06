#import <appmac_appdelegate.h>
#import <agtg_gl.h>
#import <aftl_logger.h>
#import <aftl_stdoutobserver.h>
#import <Cocoa/Cocoa.h>
#import <exception>
#import <sstream>
#import <string>

std::string args(int argc, char* argv[])
{
    std::stringstream s;
    for (int i = 1; i < argc; ++i)
    {
        s << (i == 1 ? "" : ", ") << argv[i];
    }

    return s.str();
}

int main(int argc, char *argv[])
{
    // initialize the logger with the default log observer
     aftl::Logger& logger = aftl::Logger::instance();
    aftl::Logger::LogObserverPtr observer(new aftl::StdoutObserver());
    logger.addObserver(observer);
    logger.logLevel(aftl::LogLevel_TRACE);

    AFTL_LOG_INFO << "Starting application ["
        << " args: " << args(argc, argv)
        << " ]" << AFTL_LOG_END;

    try {
        @autoreleasepool {
            NSApplication* app = [NSApplication sharedApplication];
        
            appmac_AppDelegate* appDelegate = [[[appmac_AppDelegate alloc] init] autorelease];
            [app setDelegate:appDelegate];
            [app run];
        }
    } catch (std::exception const& e) {
        AFTL_LOG_ERROR << "Exception! " << e.what() << AFTL_LOG_END;
        return -1;
    } catch (...) {
        AFTL_LOG_ERROR << "Unknown exception!" << AFTL_LOG_END;
        return -1;
    }
    
    return 0;
}
