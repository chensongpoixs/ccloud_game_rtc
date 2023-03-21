//#include <unistd.h>
//#include <xcb/xcb.h>
//
//int
//main()
//{
//    xcb_connection_t *conn;
//    xcb_screen_t *screen;
//    xcb_window_t win;
//
//    conn = xcb_connect(NULL,NULL);
//
//    screen = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
//
//    win = xcb_generate_id(conn);
//
//    xcb_create_window(conn,
//                      XCB_COPY_FROM_PARENT,
//                      win,
//                      screen->root,
//                      0,0,
//                      150,150,
//                      10,
//                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
//                      screen->root_visual,
//                      0,NULL);
//    xcb_map_window(conn, win);
//
//    xcb_flush(conn);
//
//    pause();
//    return 0;
//}


#include <stdlib.h>
#include <stdio.h>
///usr/include/xcb/xproto.h
#include <xcb/xcb.h>

void
print_modifiers (uint32_t mask)
{
    const char **mod, *mods[] = {
            "Shift", "Lock", "Ctrl", "Alt",
            "Mod2", "Mod3", "Mod4", "Mod5",
            "Button1", "Button2", "Button3", "Button4", "Button5"
    };
    printf ("Modifier mask: ");
    for (mod = mods ; mask; mask >>= 1, mod++)
        if (mask & 1)
            printf(*mod);
    putchar ('\n');
}

int
main ()
{
    xcb_connection_t    *c;
    xcb_screen_t        *screen;
    xcb_window_t         win;
    xcb_generic_event_t *e;
    uint32_t             mask = 0;
    uint32_t             values[2];

    /* Open the connection to the X server */
    c = xcb_connect (NULL, NULL);

    /* Get the first screen */
    screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;

    /* Ask for our window's Id */
    win = xcb_generate_id (c);

    /* Create the window */
    mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    values[0] = screen->white_pixel;
    values[1] = XCB_EVENT_MASK_EXPOSURE       | XCB_EVENT_MASK_BUTTON_PRESS   |
                XCB_EVENT_MASK_BUTTON_RELEASE | XCB_EVENT_MASK_POINTER_MOTION |
                XCB_EVENT_MASK_ENTER_WINDOW   | XCB_EVENT_MASK_LEAVE_WINDOW   |
                XCB_EVENT_MASK_KEY_PRESS      | XCB_EVENT_MASK_KEY_RELEASE;
    xcb_create_window (c,                             /* Connection          */
                       0,                             /* depth               */
                       win,                           /* window Id           */
                       screen->root,                  /* parent window       */
                       0, 0,                          /* x, y                */
                       150, 150,                      /* width, height       */
                       10,                            /* border_width        */
                       XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class               */
                       screen->root_visual,           /* visual              */
                       mask, values);                 /* masks */

    /* Map the window on the screen */
    xcb_map_window (c, win);

    xcb_flush (c);

    while ((e = xcb_wait_for_event (c))) {
        switch (e->response_type & ~0x80) {
            case XCB_EXPOSE: {
                xcb_expose_event_t *ev = (xcb_expose_event_t *)e;

                printf ("Window %ld exposed. Region to be redrawn at location (%d,%d), with dimension (%d,%d)\n",
                        ev->window, ev->x, ev->y, ev->width, ev->height);
                break;
            }
            case XCB_BUTTON_PRESS: {
                xcb_button_press_event_t *ev = (xcb_button_press_event_t *)e;
                print_modifiers(ev->state);

                switch (ev->detail) {
                    case 4:
                        printf ("Wheel Button up in window %ld, at coordinates (%d,%d)\n",
                                ev->event, ev->event_x, ev->event_y);
                        break;
                    case 5:
                        printf ("Wheel Button down in window %ld, at coordinates (%d,%d)\n",
                                ev->event, ev->event_x, ev->event_y);
                        break;
                    default:
                        printf ("Button %d pressed in window %ld, at coordinates (%d,%d)\n",
                                ev->detail, ev->event, ev->event_x, ev->event_y);
                }
                break;
            }
            case XCB_BUTTON_RELEASE: {
                xcb_button_release_event_t *ev = (xcb_button_release_event_t *)e;
                print_modifiers(ev->state);

                printf ("Button %d released in window %ld, at coordinates (%d,%d)\n",
                        ev->detail, ev->event, ev->event_x, ev->event_y);
                break;
            }
            case XCB_MOTION_NOTIFY: {
                xcb_motion_notify_event_t *ev = (xcb_motion_notify_event_t *)e;

                printf ("Mouse moved in window %ld, at coordinates (%d,%d)\n",
                        ev->event, ev->event_x, ev->event_y);
                break;
            }
            case XCB_ENTER_NOTIFY: {
                xcb_enter_notify_event_t *ev = (xcb_enter_notify_event_t *)e;

                printf ("Mouse entered window %ld, at coordinates (%d,%d)\n",
                        ev->event, ev->event_x, ev->event_y);
                break;
            }
            case XCB_LEAVE_NOTIFY: {
                xcb_leave_notify_event_t *ev = (xcb_leave_notify_event_t *)e;

                printf ("Mouse left window %ld, at coordinates (%d,%d)\n",
                        ev->event, ev->event_x, ev->event_y);
                break;
            }
            case XCB_KEY_PRESS: 
            {
                /*
                typedef struct xcb_key_press_event_t {
    uint8_t         response_type;
    xcb_keycode_t   detail;
    uint16_t        sequence;
    xcb_timestamp_t time;
    xcb_window_t    root;
    xcb_window_t    event;
    xcb_window_t    child;
    int16_t         root_x;
    int16_t         root_y;
    int16_t         event_x;
    int16_t         event_y;
    uint16_t        state;
    uint8_t         same_screen;
    uint8_t         pad0;
} xcb_key_press_event_t;
                */
                xcb_key_press_event_t *ev = (xcb_key_press_event_t *)e;
                print_modifiers(ev->state);

                printf ("Key pressed in window %ld[response_type = %u][detail = %u][sequence = %u][xcb_timestamp_t = %u][root_x = %u][root_y = %u][event_x = %u][event_y = %u][state = %u][same_screen = %u][pad0 = %u]\n", 
                ev->event, ev->response_type, ev->detail, ev->sequence, ev->time, ev->root_x, ev->root_y, ev->event_x, ev->event_y, ev->state, ev->same_screen, ev->pad0);
                break;
            }
            case XCB_KEY_RELEASE: {
                xcb_key_release_event_t *ev = (xcb_key_release_event_t *)e;
                print_modifiers(ev->state);

                printf ("Key released in window %ld[response_type = %u][detail = %u][sequence = %u][xcb_timestamp_t = %u][root_x = %u][root_y = %u][event_x = %u][event_y = %u][state = %u][same_screen = %u][pad0 = %u]\n", 
                ev->event, ev->response_type, ev->detail, ev->sequence, ev->time, ev->root_x, ev->root_y, ev->event_x, ev->event_y, ev->state, ev->same_screen, ev->pad0);
                break;
            }
            default:
                /* Unknown event type, ignore it */
                printf("Unknown event: %d\n", e->response_type);
                break;
        }
        /* Free the Generic Event */
        free (e);
    }

    return 0;
}