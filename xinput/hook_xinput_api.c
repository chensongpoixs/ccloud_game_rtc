#define _GNU_SOURCE
#include <sys/types.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h> 
#include <stdio.h>
#include <stdlib.h>  
#include <X11/Xlib.h>  
#include <xcb/xinput.h>


#include <xcb/xproto.h>
#define gettid() syscall(__NR_gettid)


static uint32_t *(*hook_xcb_input_touch_begin_button_mask) (const xcb_input_touch_begin_event_t *R) = NULL;


uint32_t * xcb_input_touch_begin_button_mask  (const xcb_input_touch_begin_event_t *R)
{
    if (!hook_xcb_input_touch_begin_button_mask)
    {
        hook_xcb_input_touch_begin_button_mask  = dlsym(RTLD_NEXT, "xcb_input_touch_begin_button_mask");
    }

     char buffer[1024] = {0};
    (void)sprintf(buffer, "[xcb_input_touch_begin_button_mask] ======================>\n" );
    (void)write(1, buffer, strlen(buffer));
    return hook_xcb_input_touch_begin_button_mask(R);
}