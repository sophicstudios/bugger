#import <appmac_appdelegate.h>

#include <game_client.h>

using namespace aegis;

struct appmac_AppDelegateImpl
{
    wrangler::game::Client* client;
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
    
    m_impl->client = new wrangler::game::Client();
    [self createMainMenu];
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
    // Insert code here to initialize your application
    NSLog(@"applicationDidFinishLaunching: %@", notification);
    
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
    delete m_impl->client;
}

@end
