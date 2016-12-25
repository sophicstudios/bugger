#import <appios_appdelegate.h>
#import <uiios_openglwindow.h>
#import <game_client.h>
#import <util_convert.h>
#import <util_bundlefilesystem.h>
#import <aftt_date.h>
#import <iostream>

struct appios_AppDelegateImpl
{
    std::shared_ptr<uiios::OpenGLWindow> window;
    std::shared_ptr<game::Client> client;
    bool paused;
};

@implementation appios_AppDelegate

- (id)init
{
    self = [super init];
    
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    std::cout << "appios_AppDelegate::didFinishLaunchingWithOptions" << std::endl;
    
    m_impl = new appios_AppDelegateImpl();

    m_impl->window = std::shared_ptr<uiios::OpenGLWindow>(new uiios::OpenGLWindow("Wrangler"));
    
    std::shared_ptr<util::BundleFileSystem> filesystem(new util::BundleFileSystem());
    m_impl->client = std::shared_ptr<game::Client>(new game::Client(m_impl->window->glView(), filesystem));

    m_impl->window->show();
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state.
    // This can occur for certain types of temporary interruptions (such as an
    // incoming phone call or SMS message) or when the user quits the application
    // and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down
    // OpenGL ES frame rates. Games should use this method to pause the game.

    std::cout << "appios_AppDelegate::applicationWillResignActive" << std::endl;

    m_impl->client->stop();
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate
    // timers, and store enough application state information to restore your
    // application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called
    // instead of applicationWillTerminate: when the user quits.

    std::cout << "appios_AppDelegate::applicationDidEnterBackground" << std::endl;
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the active
    // state; here you can undo many of the changes made on entering the background.

    std::cout << "appios_AppDelegate::applicationWillEnterForeground" << std::endl;
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the
    // application was inactive. If the application was previously in the
    // background, optionally refresh the user interface.

    std::cout << "appios_AppDelegate::applicationDidBecomeActive" << std::endl;

    m_impl->client->run();
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if
    // appropriate. See also applicationDidEnterBackground:.

    std::cout << "appios_AppDelegate::applicationWillTerminate" << std::endl;

    delete m_impl;
}

@end
