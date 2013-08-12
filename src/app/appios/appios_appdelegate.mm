#import <appios_appdelegate.h>
#import <uiios_openglwindow.h>
#import <game_client.h>
#import <aftt_date.h>
#import <iostream>

struct appios_AppDelegateImpl
{
    uiios::OpenGLWindow* window;
    game::Client* client;
};

@implementation appios_AppDelegate

- (id)init
{
    self = [super init];
    
    m_impl = new appios_AppDelegateImpl();

    return self;
}

- (void)dealloc
{
    delete m_impl;
    
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    std::cout << "appios_AppDelegate::didFinishLaunchingWithOptions" << std::endl;
    
    m_impl->window = new uiios::OpenGLWindow("Wrangler");
    m_impl->client = new game::Client(*m_impl->window);

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
    std::cout << "appios_AppDelegate::applicationWillResignActive" << std::endl;

    m_impl->client->pause();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.

    std::cout << "appios_AppDelegate::applicationDidEnterBackground" << std::endl;
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.

    std::cout << "appios_AppDelegate::applicationWillEnterForeground" << std::endl;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.

    std::cout << "appios_AppDelegate::applicationDidBecomeActive" << std::endl;
    m_impl->client->run();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.

    std::cout << "appios_AppDelegate::applicationWillTerminate" << std::endl;
    
    if (m_impl->client) {
        m_impl->client->stop();
        delete m_impl->client;
    }

    if (m_impl->window) {
        delete m_impl->window;
    }
}

@end
