/******************************************************************************/
#include "iOS.h"
#undef super // Objective-C has its own 'super'
/******************************************************************************/
Bool DontRemoveThisOriOSAppDelegateClassWontBeLinked;
/******************************************************************************/
namespace EE{
/******************************************************************************/
void (*InitChartboostPtr)();
/******************************************************************************/
iOSAppDelegate* GetAppDelegate()
{
   return (iOSAppDelegate*)[UIApplication sharedApplication].delegate;
}
/******************************************************************************/
static void UpdateLocation(CLLocation *location, Bool gps)
{
   if(location)
      if(location.horizontalAccuracy>=0) // can be negative if coordinates are invalid
   {
      LocationLat[gps]=location.coordinate.latitude;
      LocationLon[gps]=location.coordinate.longitude;
      LocationAlt[gps]=location.altitude;
      LocationSpd[gps]=Max(0, (Flt)location.speed); // can be negative if invalid
      LocationAcc[gps]=location.horizontalAccuracy;
      LocationTim[gps].from(location.timestamp);
   }
}
static void UpdateMagnetometer(CLHeading *heading)
{
   if(heading)MagnetometerValue.set(heading.x, heading.y, -heading.z);
}
/******************************************************************************/
} // namespace EE
/******************************************************************************/
@implementation iOSAppDelegate
@synthesize window, controller;
/******************************************************************************/
-(BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
   ViewController=(MyViewController*)controller; // !! set this as first because other calls rely on this !!

   [window setRootViewController:controller]; // [window addSubview:controller.view];
   [window makeKeyAndVisible];

   // Chartboost
   if(InitChartboostPtr)InitChartboostPtr(); // call this from a pointer to function (and not directly to the advertisement codes) to avoid force linking to advertisement (which then links to chartboost lib, which uses 'advertisingIdentifier' which is prohibited when ads are not displayed in iOS, also linking to chartboost increases the app binary size)

   // Sensors
   REPA(LocationManager)if(!LocationManager[i])
   {
      LocationManager[i]=[[CLLocationManager alloc] init];
      LocationManager[i].delegate       =self;
      LocationManager[i].desiredAccuracy=kCLLocationAccuracyBest;
      LocationManager[i].distanceFilter =kCLDistanceFilterNone;
      LocationManager[i]. headingFilter =kCLHeadingFilterNone;
                               UpdateLocation    (LocationManager[i   ].location, i!=0);
   }  if(LocationManager[true])UpdateMagnetometer(LocationManager[true].heading);

#if SUPPORT_FACEBOOK
   [[FBSDKApplicationDelegate sharedInstance] application:application didFinishLaunchingWithOptions:launchOptions];
#endif

   return true;
}
-(void)applicationWillResignActive:(UIApplication*)application
{
   if(App._closed)return; // do nothing if app called 'Exit'
   App.setActive(false);
}
-(void)applicationDidBecomeActive:(UIApplication*)application
{
   if(App._closed)return; // do nothing if app called 'Exit'
   App.setActive(true);

#if SUPPORT_FACEBOOK
   [FBSDKAppEvents activateApp];
#endif
}
-(void)applicationDidEnterBackground:(UIApplication*)application
{
   if(App._closed)return; // do nothing if app called 'Exit'
   if(App.save_state)App.save_state();
}
-(void)applicationWillTerminate:(UIApplication*)application
{
   if(App._closed)return; // do nothing if app called 'Exit'
   App.del();
}
-(void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
   if(App._closed)return; // do nothing if app called 'Exit'
   App.lowMemory();
}
-(void)dealloc
{
                         [window             release]; window            =null;
                         [controller         release]; controller        =null; ViewController=null;
   REPA(LocationManager){[LocationManager[i] release]; LocationManager[i]=null;}

   [super dealloc];
}
/******************************************************************************/
-(void)locationManager:(CLLocationManager*)manager didUpdateLocations:(NSArray*)locations
{
   UpdateLocation([locations lastObject], manager==LocationManager[1]); // last is the newest
}
-(void)locationManager:(CLLocationManager*)manager didUpdateHeading:(CLHeading*)newHeading
{
   UpdateMagnetometer(newHeading);
}
/******************************************************************************/
-(void)picker:(PHPickerViewController*)picker didFinishPicking:(NSArray<PHPickerResult*>*)results
{
   [picker dismissViewControllerAnimated:YES completion:nil];
   [picker release];
   for(PHPickerResult *result in results)
   {
      [result.itemProvider loadObjectOfClass:[UIImage class] completionHandler:^(__kindof id<NSItemProviderReading> _Nullable object, NSError *_Nullable error)
      {
         if([object isKindOfClass:[UIImage class]])
         {  // this is not main thread
            UIImage *image=(UIImage*)object;
            dispatch_async(dispatch_get_main_queue(),
            ^{ // this is main thread
               NSLog(@"Selected image: %@", image);
             //[image release]; crashes
            });
         }
      }];
   }
 //[results release]; crashes
}
/******************************************************************************
// FACEBOOK
/******************************************************************************/
#if SUPPORT_FACEBOOK
-(BOOL)application:(UIApplication*)application openURL:(NSURL*)url options:(NSDictionary<UIApplicationOpenURLOptionsKey,id>*)options
{
   BOOL   handled=[[FBSDKApplicationDelegate sharedInstance] application:application openURL:url sourceApplication:options[UIApplicationOpenURLOptionsSourceApplicationKey] annotation:options[UIApplicationOpenURLOptionsAnnotationKey]];
   return handled;
}
-(void)sharer         :(id<FBSDKSharing>)sharer didCompleteWithResults:(NSDictionary*)results {if(auto callback=FB.callback)callback(Facebook::POST_SUCCESS);} // copy first to temp var to avoid multi-threading issues
-(void)sharer         :(id<FBSDKSharing>)sharer didFailWithError      :(NSError     *)error   {if(auto callback=FB.callback)callback(Facebook::POST_ERROR  );} // copy first to temp var to avoid multi-threading issues
-(void)sharerDidCancel:(id<FBSDKSharing>)sharer;                                              {if(auto callback=FB.callback)callback(Facebook::POST_CANCEL );} // copy first to temp var to avoid multi-threading issues
#endif
/******************************************************************************/
@end
/******************************************************************************/
