#import <appmac_appdelegate.h>
#import <uimac_openglwindow.h>
#import <game_client.h>
#import <util_convert.h>
#import <aftfs_localfilesystem.h>
#import <util_bundlefilesystem.h>
#import <agtm_rect.h>
#import <agtm_point2d.h>
#import <agtm_size2d.h>
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
    NSLog(@"applicationWillFinishLaunching: %@", notification);
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    NSLog(@"applicationDidFinishLaunching: %@", notification);
    
    m_impl = new appmac_AppDelegateImpl();

    agtm::Rect<float> bounds(agtm::Point2d<float>(0.0f, 0.0f), agtm::Size2d<float>(568, 320));
    m_impl->window = std::shared_ptr<uimac::OpenGLWindow>(new uimac::OpenGLWindow("Bugger", bounds));

    [self createMainMenu];

    std::shared_ptr<aftfs::Filesystem> filesystem(new util::BundleFilesystem());

    m_impl->client = std::shared_ptr<game::Client>(new game::Client(m_impl->window, filesystem));
}

- (void)applicationWillBecomeActive:(NSNotification *)notification
{
    NSLog(@"applicationWillBecomeActive");
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    NSLog(@"applicationDidBecomeActive");
    m_impl->client->run();
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    NSLog(@"applicationWillResignActive");
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    NSLog(@"applicationDidResignActive");
    m_impl->client->stop();
}

- (void)applicationWillHide:(NSNotification *)notification
{
    NSLog(@"applicationWillHide");
}

- (void)applicationDidHide:(NSNotification *)notification
{
    NSLog(@"applicationDidHide");
}

- (void)applicationWillUnhide:(NSNotification *)notification
{
    NSLog(@"applicationWillUnhide");
}

- (void)applicationDidUnhide:(NSNotification *)notification
{
    NSLog(@"applicationDidUnhide");
}

- (void)applicationWillTerminate:(NSNotification *)notification
{
    NSLog(@"applicationWillTerminate: %@", notification);

    delete m_impl;
}

@end
