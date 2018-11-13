/*
 *	PearPC
 *	PearPCViewController.mm
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

#import "PearPCViewController.h"
#import "AppDelegate.h"
#import "sysmacos.h"
#import "rendermetal.h"
#import "system/display.h"
#import "system/mouse.h"
#import "cpu/cpu.h"
#import "tools/except.h"
#import "keyglyphs.h"
#import "clientconfig.h"
#import "main.h"

#import <MetalKit/MetalKit.h>
#import <thread>
#import <bitset>

using namespace pearpc;

namespace
{
	SystemEvent fillMouseEvent(SystemMouseEventType type, NSEvent* event, NSView* view)
	{
		const NSPoint windowLoc = [event locationInWindow];
		const NSPoint viewLoc = [view convertPoint:windowLoc fromView:nil];
		const NSUInteger pressedMouseButtons = [NSEvent pressedMouseButtons];

		 // TODO: mouse.x, mouse.y, mouse.dbutton are unused - deprecate
		SystemEvent ev;
		ev.type = sysevMouse;
		ev.mouse.type = type;
		ev.mouse.button1 = (pressedMouseButtons & (1 << 0)) != 0;
		ev.mouse.button2 = (pressedMouseButtons & (1 << 1)) != 0;;
		ev.mouse.button3 = false;
		ev.mouse.x = viewLoc.x;
		ev.mouse.y = viewLoc.y;
		ev.mouse.relx = event.deltaX;
		ev.mouse.rely = event.deltaY;
		return ev;
	}
}

@interface PearPCViewController() <MTKViewDelegate>
@property (nonatomic, strong) NSTrackingArea* trackingArea;
@property (nonatomic, assign) NSEventModifierFlags modifierFlags;
@end


@implementation PearPCViewController
{
	std::shared_ptr<RenderMetal> m_renderer;
	BOOL m_initClient;
	BOOL m_startCPU;
	BOOL m_cpuRunning;
	BOOL m_cursorHide;
}

- (void)dealloc {
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (void)viewDidLoad {
	[super viewDidLoad];
	
	MTKView* metalView = (MTKView*)self.view;
	metalView.device = MTLCreateSystemDefaultDevice();
	metalView.delegate = self;
	
	m_renderer = std::make_shared<RenderMetal>(metalView);
	m_renderer->loadResources(metalView);
	
	__block auto uiThread = std::this_thread::get_id();
	
	m_renderer->onClientResolutionChange(^void (const DisplayCharacteristics& chr) {
		if (std::this_thread::get_id() == uiThread) {
			self.preferredContentSize = CGSizeMake(chr.width, chr.height);
		} else {
			dispatch_async(dispatch_get_main_queue(), ^{
				self.preferredContentSize = CGSizeMake(chr.width, chr.height);
			});
		}
	});
	
	[NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown | NSEventMaskKeyUp | NSEventMaskFlagsChanged
										  handler:^NSEvent* (NSEvent* event) {
											  if ([self handleKeyEvent:event]) {
												  return nil;
											  } else {
												  return event;
											  }
										  }];
	
	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(windowDidResignKey:)
												 name:NSWindowDidResignKeyNotification
											   object:nil];

	self.modifierFlags = [NSEvent modifierFlags];
}

- (void)viewWillAppear {
	[super viewWillAppear];
	
	if (!m_initClient) {
		m_initClient = YES;
		
		try {
			main::initClient(self.clientConfig, [&] (const char* title) {
				// initUI
				auto display = new MetalSystemDisplay(title, self.clientConfig->getDisplayConfig(), self.clientConfig->getRedrawInterval(), m_renderer);
				gDisplay = display;
				gMouse = new MacSystemMouse();
				gKeyboard = new MacSystemKeyboard();
				gKeyboard->setKeyConfig(self.clientConfig->getKeyConfig());
				m_renderer->clientResolutionChanged(gDisplay->mClientChar);
				
				[self initMouseGrabDisplay:display];
				
				self.view.window.title = [NSString stringWithUTF8String:title];
			});
		} catch (Exception& e) {
			String reason;
			e.reason(reason);
			
			NSAlert* alert = [[NSAlert alloc] init];
			alert.messageText = [NSString stringWithFormat:@"Failed to start virtual machine"];
			alert.informativeText = [NSString stringWithUTF8String:reason.contentChar()];
			alert.alertStyle = NSAlertStyleCritical;
			[alert runModal];
			[NSApp terminate:nil];
		}
	}
	
	m_renderer->drawableSizeWillChange(self.view.bounds.size);
}

- (void)viewDidAppear {
	[super viewDidAppear];
	
	[self trackMouseMove:YES];
	
	if (!m_startCPU) {
		m_startCPU = YES;
		
		main::startCPU(self.clientConfig);
		m_cpuRunning = YES;
	}
	
	// this was your last chance to visit the config..
	self.clientConfig = nil;
}

- (void)trackMouseMove:(BOOL)track {
	if (self.trackingArea || track == NO) {
		[self.view removeTrackingArea:self.trackingArea];
		self.trackingArea = nil;
	}
	
	if (track) {
		self.trackingArea = [[NSTrackingArea alloc] initWithRect:self.view.bounds
														 options:NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved
														   owner:self
														userInfo:nil];
		[self.view addTrackingArea:self.trackingArea];
	}
}

- (void)initMouseGrabDisplay:(MetalSystemDisplay*)display {
	
	// Title bar accessory to display mouse ungrab
	if (self.titleRightView) {
		NSTitlebarAccessoryViewController* accessory = [[NSTitlebarAccessoryViewController alloc] init];
		accessory.view = self.titleRightView;
		accessory.layoutAttribute = NSLayoutAttributeRight;
		[self.view.window addTitlebarAccessoryViewController:accessory];
	
		self.titleRightView.stringValue = [NSString string];
	}
		
	display->setMouseGrabWillChange(^bool (bool grab) {
		return [self canGrabMouse:grab];
	});
	
	display->setMouseGrabDidChange(^(bool grab) {
		[self updateMouseGrab:grab inTextField:self.titleRightView];
	});
}

- (bool)canGrabMouse:(bool)grab {
	if (grab) {
		// Ensure cursor is inside our window
		const auto mousePos = [NSEvent mouseLocation];

		if ([NSWindow windowNumberAtPoint:mousePos belowWindowWithWindowNumber:0] != self.view.window.windowNumber) {
			return false;
		}
	}
	
	return true;
}

- (void)updateMouseGrab:(bool)grab inTextField:(NSTextField*)textField {
	// Hide/unhide the cursor
	if (grab) {
		if (!m_cursorHide) {
			m_cursorHide = true;
			[NSCursor hide];
		}
	} else {
		if (m_cursorHide) {
			m_cursorHide = false;
			[NSCursor unhide];
		}
	}

	CGAssociateMouseAndMouseCursorPosition(!grab);
	
	// Update mouse release display
	if (grab) {
		KeyBinding key = gKeyboard->getKeyConfig().key_toggle_mouse_grab;
		
		NSMutableArray<NSString*>* keyNames = [NSMutableArray arrayWithCapacity:5];
		if (key.modifiers & KEYCODE_MASK_CTRL) {
			[keyNames addObject:keyGlyphFromQKeyCode(Q_KEY_CODE_CTRL)];
		}
		if (key.modifiers & KEYCODE_MASK_ALT) {
			[keyNames addObject:keyGlyphFromQKeyCode(Q_KEY_CODE_ALT)];
		}
		if (key.modifiers & KEYCODE_MASK_SHIFT) {
			[keyNames addObject:keyGlyphFromQKeyCode(Q_KEY_CODE_SHIFT)];
		}
		if (key.modifiers & KEYCODE_MASK_META) {
			[keyNames addObject:keyGlyphFromQKeyCode(Q_KEY_CODE_META_L)];
		}
		
		if (NSString* glyph = keyGlyphFromQKeyCode(key.qkeycode)) {
			[keyNames addObject:glyph];
		} else {
			// Remove modifier to look up the key name
			key.modifiers = 0;
			String str;
			SystemKeyboard::convertKeyBindingToString(str, key);
			
			[keyNames addObject:[[NSString stringWithUTF8String:str.contentChar()] uppercaseString]];
		}
			
		textField.stringValue = [NSString stringWithFormat:@"%@ Disables Mouse", [keyNames componentsJoinedByString:@""]];
	} else {
		textField.stringValue = [NSString string];
		
	}
	
	[textField sizeToFit];
}

- (std::shared_ptr<class RenderMetal>)renderer {
	return m_renderer;
}

- (void)shutdown:(BOOL)stopCPU {
	if (stopCPU && cpu_thread_is_running(0)) {
		ppc_cpu_stop();
	}
	
	// wait for cpu thread to complete
	main::waitForCPU();
	main::doneClient();
}

#pragma mark - Mouse

- (void)mouseMoved:(NSEvent *)event {
	if (gMouse) {
		auto ev = fillMouseEvent(sme_motionNotify, event, self.view);
		gMouse->handleEvent(ev);
	}
}

- (void)mouseDragged:(NSEvent *)event {
	if (gMouse) {
		auto ev = fillMouseEvent(sme_motionNotify, event, self.view);
		gMouse->handleEvent(ev);
	}
}

- (void)mouseDown:(NSEvent *)event {
	if (gMouse) {
		auto ev = fillMouseEvent(sme_buttonPressed, event, self.view);
		gMouse->handleEvent(ev);
	}
}

- (void)mouseUp:(NSEvent *)event {
	if (gMouse) {
		auto ev = fillMouseEvent(sme_buttonReleased, event, self.view);
		gMouse->handleEvent(ev);
	}
}

#pragma mark - Keyboard

- (NSEvent*)handleKeyEvent:(NSEvent*)event
{
	if (NSWindow* window = self.view.window) {
		if (NSApplication.sharedApplication.keyWindow == window) {
			if (event.type == NSEventTypeKeyDown) {
				[self keyDown:event];
				return event;
			} else if (event.type == NSEventTypeKeyUp) {
				[self keyUp:event];
				return event;
			} else if (event.type == NSEventTypeFlagsChanged) {
				[self flagsChanged:event];
				return event;
			}
		}
	}
	
	return nil;
}

- (void)keyDown:(NSEvent *)event {
	if (gKeyboard) {
		SystemEvent ev;
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.keyCode);
		ev.type = sysevKey;
		ev.key.pressed = true;
		gKeyboard->handleEvent(ev);
	}
}

- (void)keyUp:(NSEvent *)event {
	if (gKeyboard) {
		SystemEvent ev;
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.keyCode);
		ev.type = sysevKey;
		ev.key.pressed = false;
		gKeyboard->handleEvent(ev);
	}
}

- (void)flagsChanged:(NSEvent *)event {
	const NSEventModifierFlags curr = event.modifierFlags;
	
	if (gKeyboard) {
		const NSEventModifierFlags prev = self.modifierFlags;
		
		// If more bits are set than last time, interpret this as a key down
		const auto prevSet = std::bitset<sizeof(NSEventModifierFlags) * 8>(prev).count();
		const auto currSet = std::bitset<sizeof(NSEventModifierFlags) * 8>(curr).count();
		
		SystemEvent ev;
		ev.key.qkeycode = gKeyboard->convertKeycodeToQKeyCode(event.keyCode);
		ev.type = sysevKey;
		ev.key.pressed = currSet > prevSet;
		gKeyboard->handleEvent(ev);
	}
	
	self.modifierFlags = curr;
}

#pragma mark - Notifications

- (void)windowDidResignKey:(NSNotification*)notification {
	if (notification.object == self.view.window) {
		if (gDisplay && gDisplay->isMouseGrabbed()) {
			gDisplay->setMouseGrab(false);
		}
	}
}

#pragma mark - MTKViewDelegate

- (void)drawInMTKView:(nonnull MTKView *)view {
	if (m_cpuRunning) {
		if (main::isCPURunning()) {
			m_renderer->draw(view);
		} else {
			// TODO: display that machine is shut down
			NSLog(@"CPU halted");
			m_cpuRunning = NO;
		}
	}
}

- (void)mtkView:(nonnull MTKView *)view drawableSizeWillChange:(CGSize)size {
	m_renderer->drawableSizeWillChange(size);
	[self trackMouseMove:YES];
}

@end
