#include <appmac_appdelegate.h>
#include <uimac_openglwindow.h>
#include <game_client.h>
#include <agtm_rect.h>
#include <agtm_point2d.h>
#include <agtm_size2d.h>

struct appmac_AppDelegateImpl
{
    uimac::OpenGLWindow* window;
    game::Client* client;
};

@implementation appmac_AppDelegate

- (id)init
{
    self = [super init];
    
    m_impl = new appmac_AppDelegateImpl();

    return self;
}

- (void)dealloc
{
    delete m_impl;
    
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
    // Insert code here to initialize your application
    NSLog(@"applicationDidFinishLaunching: %@", notification);
    
    agtm::Rect<float> frame(agtm::Point2d<float>(0.0f, 0.0f), agtm::Size2d<float>(1024.0f, 768.0f));
    m_impl->window = new uimac::OpenGLWindow("Wrangler", frame);
    [self createMainMenu];
    m_impl->client = new game::Client(*m_impl->window);

    m_impl->client->run();
}

- (void)applicationWillBecomeActive:(NSNotification *)notification
{
    NSLog(@"applicationWillBecomeActive");
}

- (void)applicationDidBecomeActive:(NSNotification *)notification
{
    NSLog(@"applicationDidBecomeActive");
}

- (void)applicationWillResignActive:(NSNotification *)notification
{
    NSLog(@"applicationWillResignActive");
}

- (void)applicationDidResignActive:(NSNotification *)notification
{
    NSLog(@"applicationDidResignActive");
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
    m_impl->client->stop();
    delete m_impl->client;
}

@end
