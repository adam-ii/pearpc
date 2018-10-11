/*
 *	PearPC
 *	AppDelegate.mm
 *
 *	Copyright (C) 2018 Adam Iarossi
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#import "AppDelegate.h"
#import "PearPCViewController.h"
#import "tools/except.h"
#import "main.h"

using namespace pearpc;

@interface AppDelegate ()
@property (copy) NSString *configFile;
@property (nonatomic, weak) PearPCViewController* pearPC;
@end

@implementation AppDelegate

- (BOOL)application:(NSApplication *)sender openFile:(NSString *)filename {
	if (filename.isAbsolutePath) {
		self.configFile = filename;
	} else {
		// If launched with a relative path (e.g. from Xcode) convert to an absolute
		self.configFile = [[NSFileManager defaultManager].currentDirectoryPath stringByAppendingPathComponent:filename];
	}
	return YES;
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	BOOL quit = NO;
	NSAlert *alert = nil;

	if (self.configFile == nil) {
		// Prompt for a configuration file
		NSOpenPanel *openPanel = [NSOpenPanel openPanel];
		
		if ([openPanel runModal] == NSModalResponseOK) {
			self.configFile = openPanel.URL.path;
		} else {
			quit = YES;
		}
	}

	if (!quit) {
		// Change working directory to location of configuration file
		NSString* directory = [self.configFile stringByDeletingLastPathComponent];
		
		if (![[NSFileManager defaultManager] changeCurrentDirectoryPath:directory]) {
			alert = [[NSAlert alloc] init];
			alert.messageText = [NSString stringWithFormat:@"Unable to set working directory to %@", directory];
		} else {
			NSString* storyboardFile = [[NSBundle mainBundle].infoDictionary objectForKey:@"NSMainStoryboardFile"];
			assert([storyboardFile isKindOfClass:[NSString class]]);
			
			NSStoryboard* storyboard = [NSStoryboard storyboardWithName:storyboardFile bundle:nil];
			NSWindowController* controller = [storyboard instantiateControllerWithIdentifier:@"ClientWindowController"];
			
			if ([controller.contentViewController isKindOfClass:[PearPCViewController class]]) {
				PearPCViewController* ppc = (PearPCViewController*)controller.contentViewController;
				
				try {
					ppc.clientConfig = main::loadConfig(self.configFile.UTF8String);
					self.pearPC = ppc;
					[controller showWindow:self];
				} catch (Exception &e) {
					String reason;
					e.reason(reason);
					
					alert = [[NSAlert alloc] init];
					alert.messageText = [NSString stringWithFormat:@"Failed to load %@", self.configFile.lastPathComponent];
					alert.informativeText = [NSString stringWithUTF8String:reason.contentChar()];
				}
			} else {
				alert = [[NSAlert alloc] init];
				alert.messageText = @"Failed to load the Pear PC view controller";
			}
		}
	}

	if (alert) {
		alert.alertStyle = NSAlertStyleCritical;
		[alert runModal];
		quit = YES;
	}
	
	if (quit) {
		[NSApp terminate:nil];
	}
}

- (void)applicationWillTerminate:(NSNotification *)aNotification {
	[self.pearPC shutdown:YES];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
	return YES;
}

@end
