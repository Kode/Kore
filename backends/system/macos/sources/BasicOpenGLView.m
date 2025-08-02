#import <kore3/backend/BasicOpenGLView.h>

#include <kore3/input/keyboard.h>
#include <kore3/input/mouse.h>
#include <kore3/input/pen.h>
#include <kore3/system.h>

#ifdef KORE_METAL
#import <AppKit/NSApplication.h>
#import <AppKit/NSText.h>
#import <AppKit/NSWindow.h>

// #include <kore3/graphics5/graphics.h>
#endif

@implementation BasicOpenGLView

static bool shift = false;
static bool ctrl  = false;
static bool alt   = false;
static bool cmd   = false;

#ifndef KORE_METAL
+ (NSOpenGLPixelFormat *)basicPixelFormat {
	// TODO (DK) pass via argument in
	int aa = 1; // Kore::Application::the()->antialiasing();
	if (aa > 0) {
		NSOpenGLPixelFormatAttribute attributes[] = {NSOpenGLPFADoubleBuffer,          NSOpenGLPFADepthSize,
		                                             (NSOpenGLPixelFormatAttribute)24, // 16 bit depth buffer
		                                             NSOpenGLPFAOpenGLProfile,         NSOpenGLProfileVersion3_2Core,
		                                             NSOpenGLPFASupersample,           NSOpenGLPFASampleBuffers,
		                                             (NSOpenGLPixelFormatAttribute)1,  NSOpenGLPFASamples,
		                                             (NSOpenGLPixelFormatAttribute)aa, NSOpenGLPFAStencilSize,
		                                             (NSOpenGLPixelFormatAttribute)8,  (NSOpenGLPixelFormatAttribute)0};
		return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
	}
	else {
		NSOpenGLPixelFormatAttribute attributes[] = {
		    NSOpenGLPFADoubleBuffer,         NSOpenGLPFADepthSize,           (NSOpenGLPixelFormatAttribute)24, // 16 bit depth buffer
		    NSOpenGLPFAOpenGLProfile,        NSOpenGLProfileVersion3_2Core,  NSOpenGLPFAStencilSize,
		    (NSOpenGLPixelFormatAttribute)8, (NSOpenGLPixelFormatAttribute)0};
		return [[NSOpenGLPixelFormat alloc] initWithAttributes:attributes];
	}
}

- (void)switchBuffers {
	[[self openGLContext] flushBuffer];
}
#endif

- (void)flagsChanged:(NSEvent *)theEvent {
	if (shift) {
		kore_internal_keyboard_trigger_key_up(KORE_KEY_SHIFT);
		shift = false;
	}
	if (ctrl) {
		kore_internal_keyboard_trigger_key_up(KORE_KEY_CONTROL);
		ctrl = false;
	}
	if (alt) {
		kore_internal_keyboard_trigger_key_up(KORE_KEY_ALT);
		alt = false;
	}
	if (cmd) {
		kore_internal_keyboard_trigger_key_up(KORE_KEY_META);
		cmd = false;
	}

	if ([theEvent modifierFlags] & NSEventModifierFlagShift) {
		kore_internal_keyboard_trigger_key_down(KORE_KEY_SHIFT);
		shift = true;
	}
	if ([theEvent modifierFlags] & NSEventModifierFlagControl) {
		kore_internal_keyboard_trigger_key_down(KORE_KEY_CONTROL);
		ctrl = true;
	}
	if ([theEvent modifierFlags] & NSEventModifierFlagOption) {
		kore_internal_keyboard_trigger_key_down(KORE_KEY_ALT);
		alt = true;
	}
	if ([theEvent modifierFlags] & NSEventModifierFlagCommand) {
		kore_internal_keyboard_trigger_key_down(KORE_KEY_META);
		cmd = true;
	}
}

- (void)keyDown:(NSEvent *)theEvent {
	if ([theEvent isARepeat])
		return;
	NSString *characters = [theEvent charactersIgnoringModifiers];
	if ([characters length]) {
		unichar ch = [characters characterAtIndex:0];
		switch (ch) { // keys that exist in keydown and keypress events
		case 59:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_SEMICOLON);
			break;
		case 91:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_OPEN_BRACKET);
			break;
		case 93:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_CLOSE_BRACKET);
			break;
		case 39:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_QUOTE);
			break;
		case 92:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_BACK_SLASH);
			break;
		case 44:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_COMMA);
			break;
		case 46:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_PERIOD);
			break;
		case 47:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_SLASH);
			break;
		case 96:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_BACK_QUOTE);
			break;
		case 32:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_SPACE);
			break;
		case 45: // we need breaks because EQUALS triggered too for some reason
			kore_internal_keyboard_trigger_key_down(KORE_KEY_HYPHEN_MINUS);
			break;
		case 61:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_EQUALS);
			break;
		}
		switch (ch) {
		case NSRightArrowFunctionKey:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_RIGHT);
			break;
		case NSLeftArrowFunctionKey:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_LEFT);
			break;
		case NSUpArrowFunctionKey:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_UP);
			break;
		case NSDownArrowFunctionKey:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_DOWN);
			break;
		case 27:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_ESCAPE);
			break;
		case NSEnterCharacter:
		case NSNewlineCharacter:
		case NSCarriageReturnCharacter:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_RETURN);
			kore_internal_keyboard_trigger_key_press('\n');
			break;
		case 0x7f:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_BACKSPACE);
			kore_internal_keyboard_trigger_key_press('\x08');
			break;
		case 9:
			kore_internal_keyboard_trigger_key_down(KORE_KEY_TAB);
			kore_internal_keyboard_trigger_key_press('\t');
			break;
		default:
			if (ch == 'x' && [theEvent modifierFlags] & NSEventModifierFlagCommand) {
				char *text = kore_internal_cut_callback();
				if (text != NULL) {
					NSPasteboard *board = [NSPasteboard generalPasteboard];
					[board clearContents];
					[board setString:[NSString stringWithUTF8String:text] forType:NSPasteboardTypeString];
				}
			}
			if (ch == 'c' && [theEvent modifierFlags] & NSEventModifierFlagCommand) {
				char *text = kore_internal_copy_callback();
				if (text != NULL) {
					NSPasteboard *board = [NSPasteboard generalPasteboard];
					[board clearContents];
					[board setString:[NSString stringWithUTF8String:text] forType:NSPasteboardTypeString];
				}
			}
			if (ch == 'v' && [theEvent modifierFlags] & NSEventModifierFlagCommand) {
				NSPasteboard *board = [NSPasteboard generalPasteboard];
				NSString     *data  = [board stringForType:NSPasteboardTypeString];
				if (data != nil) {
					char charData[4096];
					strcpy(charData, [data UTF8String]);
					kore_internal_paste_callback(charData);
				}
			}
			if (ch >= L'a' && ch <= L'z') {
				kore_internal_keyboard_trigger_key_down(ch - L'a' + KORE_KEY_A);
			}
			else if (ch >= L'A' && ch <= L'Z') {
				kore_internal_keyboard_trigger_key_down(ch - L'A' + KORE_KEY_A);
			}
			else if (ch >= L'0' && ch <= L'9') {
				kore_internal_keyboard_trigger_key_down(ch - L'0' + KORE_KEY_0);
			}
			kore_internal_keyboard_trigger_key_press(ch);
			break;
		}
	}
}

- (void)keyUp:(NSEvent *)theEvent {
	NSString *characters = [theEvent charactersIgnoringModifiers];
	if ([characters length]) {
		unichar ch = [characters characterAtIndex:0];
		switch (ch) {
		case 59:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_SEMICOLON);
			break;
		case 91:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_OPEN_BRACKET);
			break;
		case 93:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_CLOSE_BRACKET);
			break;
		case 39:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_QUOTE);
			break;
		case 92:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_BACK_SLASH);
			break;
		case 44:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_COMMA);
			break;
		case 46:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_PERIOD);
			break;
		case 47:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_SLASH);
			break;
		case 96:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_BACK_QUOTE);
			break;
		case 45:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_HYPHEN_MINUS);
			break;
		case 61:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_EQUALS);
			break;
		case NSRightArrowFunctionKey:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_RIGHT);
			break;
		case NSLeftArrowFunctionKey:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_LEFT);
			break;
		case NSUpArrowFunctionKey:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_UP);
			break;
		case NSDownArrowFunctionKey:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_DOWN);
			break;
		case 27:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_ESCAPE);
			break;
		case NSEnterCharacter:
		case NSNewlineCharacter:
		case NSCarriageReturnCharacter:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_RETURN);
			break;
		case 0x7f:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_BACKSPACE);
			break;
		case 9:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_TAB);
			break;
		case 32:
			kore_internal_keyboard_trigger_key_up(KORE_KEY_SPACE);
			break;
		default:
			if (ch >= L'a' && ch <= L'z') {
				kore_internal_keyboard_trigger_key_up(ch - L'a' + KORE_KEY_A);
			}
			else if (ch >= L'A' && ch <= L'Z') {
				kore_internal_keyboard_trigger_key_up(ch - L'A' + KORE_KEY_A);
			}
			else if (ch >= L'0' && ch <= L'9') {
				kore_internal_keyboard_trigger_key_up(ch - L'0' + KORE_KEY_0);
			}
			break;
		}
	}
}

static int getMouseX(NSEvent *event) {
	// TODO (DK) map [theEvent window] to window id instead of 0
	NSWindow *window = [[NSApplication sharedApplication] mainWindow];
	float     scale  = [window backingScaleFactor];
	return (int)([event locationInWindow].x * scale);
}

static int getMouseY(NSEvent *event) {
	// TODO (DK) map [theEvent window] to window id instead of 0
	NSWindow *window = [[NSApplication sharedApplication] mainWindow];
	float     scale  = [window backingScaleFactor];
	return (int)(kore_height() - [event locationInWindow].y * scale);
}

static bool controlKeyMouseButton = false;

- (void)mouseDown:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	if ([theEvent modifierFlags] & NSEventModifierFlagControl) {
		controlKeyMouseButton = true;
		kore_internal_mouse_trigger_press(0, 1, getMouseX(theEvent), getMouseY(theEvent));
	}
	else {
		controlKeyMouseButton = false;
		kore_internal_mouse_trigger_press(0, 0, getMouseX(theEvent), getMouseY(theEvent));
	}

	if ([theEvent subtype] == NSEventSubtypeTabletPoint) {
		kore_internal_pen_trigger_press(0, getMouseX(theEvent), getMouseY(theEvent), theEvent.pressure);
	}
}

- (void)mouseUp:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	if (controlKeyMouseButton) {
		kore_internal_mouse_trigger_release(0, 1, getMouseX(theEvent), getMouseY(theEvent));
	}
	else {
		kore_internal_mouse_trigger_release(0, 0, getMouseX(theEvent), getMouseY(theEvent));
	}
	controlKeyMouseButton = false;

	if ([theEvent subtype] == NSEventSubtypeTabletPoint) {
		kore_internal_pen_trigger_release(0, getMouseX(theEvent), getMouseY(theEvent), theEvent.pressure);
	}
}

- (void)mouseMoved:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	kore_internal_mouse_trigger_move(0, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)mouseDragged:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	kore_internal_mouse_trigger_move(0, getMouseX(theEvent), getMouseY(theEvent));

	if ([theEvent subtype] == NSEventSubtypeTabletPoint) {
		kore_internal_pen_trigger_move(0, getMouseX(theEvent), getMouseY(theEvent), theEvent.pressure);
	}
}

- (void)rightMouseDown:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	kore_internal_mouse_trigger_press(0, 1, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)rightMouseUp:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	kore_internal_mouse_trigger_release(0, 1, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)rightMouseDragged:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	kore_internal_mouse_trigger_move(0, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)otherMouseDown:(NSEvent *)theEvent {
	kore_internal_mouse_trigger_press(0, 2, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)otherMouseUp:(NSEvent *)theEvent {
	kore_internal_mouse_trigger_release(0, 2, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)otherMouseDragged:(NSEvent *)theEvent {
	kore_internal_mouse_trigger_move(0, getMouseX(theEvent), getMouseY(theEvent));
}

- (void)scrollWheel:(NSEvent *)theEvent {
	// TODO (DK) map [theEvent window] to window id instead of 0
	int delta = [theEvent deltaY];
	kore_internal_mouse_trigger_scroll(0, -delta);
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
	NSPasteboard   *pboard         = [sender draggingPasteboard];
	NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
	if ([[pboard types] containsObject:NSPasteboardTypeURL]) {
		if (sourceDragMask & NSDragOperationLink) {
			return NSDragOperationLink;
		}
	}
	return NSDragOperationNone;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
	NSPasteboard *pboard = [sender draggingPasteboard];
	// NSDragOperation sourceDragMask = [sender draggingSourceOperationMask];
	if ([[pboard types] containsObject:NSPasteboardTypeURL]) {
		NSURL   *fileURL  = [NSURL URLFromPasteboard:pboard];
		wchar_t *filePath = (wchar_t *)[fileURL.path cStringUsingEncoding:NSUTF32LittleEndianStringEncoding];
		kore_internal_drop_files_callback(filePath);
	}
	return YES;
}

#ifndef KORE_METAL
- (void)prepareOpenGL {
	const GLint swapInt = 1;
	[[self openGLContext] setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
	[super prepareOpenGL];
}
#endif

- (void)update { // window resizes, moves and display changes (resize, depth and display config change)
#ifdef KORE_OPENGL
	[super update];
#else
	[self updateDrawableSize];
#endif
}

#ifndef KORE_METAL
- (id)initWithFrame:(NSRect)frameRect {
	NSOpenGLPixelFormat *pf = [BasicOpenGLView basicPixelFormat];
	self                    = [super initWithFrame:frameRect pixelFormat:pf];

	[self prepareOpenGL];
	[[self openGLContext] makeCurrentContext];
	[self setWantsBestResolutionOpenGLSurface:YES];
	return self;
}
#else
- (void)updateDrawableSize { // This is the high DPI version of resize
	CAMetalLayer *metalLayer  = (CAMetalLayer *)self.layer;
	NSSize        size        = self.bounds.size;
	NSSize        backingSize = size;

	backingSize = [self convertSizeToBacking:size];

	metalLayer.contentsScale = backingSize.height / size.height;
	metalLayer.drawableSize  = NSSizeToCGSize(backingSize);
}

- (id)initWithFrame:(NSRect)frameRect {
	self = [super initWithFrame:frameRect];

	if (self->device == nil) {
		self->device = MTLCreateSystemDefaultDevice();
	}

	self.wantsLayer       = YES;
	self.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

	CAMetalLayer *metalLayer   = (CAMetalLayer *)self.layer;
	metalLayer.device          = self->device;
	metalLayer.pixelFormat     = MTLPixelFormatBGRA8Unorm;
	metalLayer.framebufferOnly = NO;
	// metalLayer.presentsWithTransaction = YES;

	metalLayer.opaque          = YES;
	metalLayer.backgroundColor = nil;

	[self updateDrawableSize];
	return self;
}

+ (Class)layerClass {
	return [CAMetalLayer class];
}

- (void)setBounds:(NSRect)bounds {
	[super setBounds:bounds];
	[self updateDrawableSize];
}

- (BOOL)wantsUpdateLayer {
	return YES;
}

- (CALayer *)makeBackingLayer {
	return [CAMetalLayer layer];
}

- (void)viewDidChangeBackingProperties {
	[super viewDidChangeBackingProperties];
	[self updateDrawableSize];
}

- (void)setFrame:(NSRect)frame {
	[super setFrame:frame];
	[self updateDrawableSize];
}

- (CAMetalLayer *)metalLayer {
	return (CAMetalLayer *)self.layer;
}
#endif

- (BOOL)acceptsFirstResponder {
	return YES;
}

- (BOOL)becomeFirstResponder {
	return YES;
}

- (BOOL)resignFirstResponder {
	return YES;
}

- (void)resize:(NSSize)size {
	[self setFrameSize:size];
}

@end

void kore_copy_to_clipboard(const char *text) {
	NSPasteboard *board = [NSPasteboard generalPasteboard];
	[board clearContents];
	[board setString:[NSString stringWithUTF8String:text] forType:NSPasteboardTypeString];
}
