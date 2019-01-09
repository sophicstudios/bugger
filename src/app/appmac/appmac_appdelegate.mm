#import <appmac_appdelegate.h>
#import <uimac_openglwindow.h>
#import <game_client.h>
#import <util_convert.h>
#import <aftfs_localfilesystem.h>
#import <util_bundlefilesystem.h>
#import <agtm_rect.h>
#import <agtm_vector2.h>
#import <agtm_size2d.h>
#import <aftl_logger.h>
#import <CoreFoundation/CoreFoundation.h>
#import <CoreFoundation/CFURL.h>
#import <memory>

struct appmac_AppDelegateImpl
{
    std::shared_ptr<uimac::OpenGLWindow> window;
    std::shared_ptr<game::Client> client;
};

@implementation appmac_AppDelegate

- (id)init
{
    self = [super init];
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (void)createMainMenu
{
    NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"] autorelease];

    NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Wrangler"] autorelease];
    [mainMenu addItem:quitItem];

    NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""] autorelease];
    [mainMenuItem setSubmenu:mainMenu];
    
    NSMenu* menuBar = [[[NSMenu alloc] initWithTitle:@""] autorelease];
    [menuBar addItem:mainMenuItem];
    
    [NSApp setMainMenu:menuBar];
}

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillFinishLaunching" << AFTL_LOG_END;
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationDidFinishLaunhing" << AFTL_LOG_END;

    m_impl = new appmac_AppDelegateImpl();

    agtm::Rect<float> bounds(agtm::Vector2<float>(0.0f, 0.0f), agtm::Size2d<float>(568, 320));
    m_impl->window = std::shared_ptr<uimac::OpenGLWindow>(new uimac::OpenGLWindow("Bugger", bounds));

    [self createMainMenu];

    std::shared_ptr<aftfs::FileSystem> fileSystem(new util::BundleFileSystem());

    m_impl->client = std::shared_ptr<game::Client>(new game::Client(m_impl->window->glView(), fileSystem));

    m_impl->window->show();
}

- (void)applicationWillBecomeActive:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillBecomeActive" << AFTL_LOG_END;
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationDidBecomeActive" << AFTL_LOG_END;
    if (m_impl && m_impl->client) {
        m_impl->client->run();
    }
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillResignActive" << AFTL_LOG_END;
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationDidResignActive" << AFTL_LOG_END;
    if (m_impl && m_impl->client) {
        m_impl->client->stop();
    }
}

- (void)applicationWillHide:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillHide" << AFTL_LOG_END;
}

- (void)applicationDidHide:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationDidHide" << AFTL_LOG_END;
}

- (void)applicationWillUnhide:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillUnhide" << AFTL_LOG_END;
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationDidUnhide" << AFTL_LOG_END;
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    AFTL_LOG_INFO << "applicationWillTerminate" << AFTL_LOG_END;

    delete m_impl;
}

@end
