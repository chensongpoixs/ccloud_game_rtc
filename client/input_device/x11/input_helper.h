

#ifndef _included_input_helper
#define _included_input_helper

#include <stdint.h>
#include <X11/Xlib.h>
#include <xcb/xcb.h>
#ifdef USE_XKB_COMMON
#include <X11/Xlib-xcb.h>
#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-x11.h>
#endif


// Virtual button codes that are not defined by X11.
#define Button1     1
#define Button2     2
#define Button3     3
#define WheelUp     4
#define WheelDown   5
#define WheelLeft   6
#define WheelRight  7
#define XButton1    8
#define XButton2    9

// Helper display used by input helper, properties and post event.
extern Display *helper_disp;

extern xcb_connection_t * g_connection;

extern  Window  g_window;
extern int g_win_h;
extern int g_win_w;
extern int g_win_d;
extern xcb_pixmap_t g_win_pixmap;
/* Converts a X11 key symbol to a single Unicode character.  No direct X11
 * functionality exists to provide this information.
 */
 size_t keysym_to_unicode(KeySym keysym, uint16_t *buffer, size_t size);

/* Convert a single Unicode character to a X11 key symbol.  This function
 * provides a better translation than XStringToKeysym() for Unicode characters.
 */
 KeySym unicode_to_keysym(uint16_t unicode);

/* Converts a X11 key code to the appropriate keyboard scan code.
 */
 uint16_t keycode_to_scancode(KeyCode keycode);

/* Converts a keyboard scan code to the appropriate X11 key code.
 */
 KeyCode scancode_to_keycode(uint16_t scancode);


#ifdef USE_XKB_COMMON

/* Converts a X11 key code to a Unicode character sequence.  libXKBCommon support
 * is required for this method.
 */
 size_t keycode_to_unicode(struct xkb_state* state, KeyCode keycode, uint16_t *buffer, size_t size);

/* Create a xkb_state structure and return a pointer to it.
 */
 struct xkb_state * create_xkb_state(struct xkb_context *context, xcb_connection_t *connection);

/* Release xkb_state structure created by create_xkb_state().
 */
 void destroy_xkb_state(struct xkb_state* state);

#else

/* Converts a X11 key code and event mask to the appropriate X11 key symbol.
 * This functions in much the same way as XKeycodeToKeysym() but allows for a
 * faster and more flexible lookup.
 */
 KeySym keycode_to_keysym(KeyCode keycode, unsigned int modifier_mask);

#endif

/* Lookup a X11 buttons possible remapping and return that value.
 */
 unsigned int button_map_lookup(unsigned int button);

/* Initialize items required for KeyCodeToKeySym() and KeySymToUnicode()
 * functionality.  This method is called by OnLibraryLoad() and may need to be
 * called in combination with UnloadInputHelper() if the native keyboard layout
 * is changed.
 */
 void load_input_helper();

/* De-initialize items required for KeyCodeToKeySym() and KeySymToUnicode()
 * functionality.  This method is called by OnLibraryUnload() and may need to be
 * called in combination with LoadInputHelper() if the native keyboard layout
 * is changed.
 */
 void unload_input_helper();


#endif
