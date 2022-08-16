#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/composite.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
xcb_get_image_reply_t *img;
xcb_get_image_cookie_t iq;
xcb_generic_error_t *e = NULL;
xcb_connection_t *conn;
xcb_screen_t *screen;
int m_iDesktopWidth = 1280;
int m_iDesktopHeight = 1024;
bool InitVideoInput()
{
    int screen_num = 1;

    conn = xcb_connect(NULL, &screen_num);

    if (xcb_connection_has_error(conn))
    {
        printf("xcb_connection_has_error faild!\n");
    }

    printf("xcb_connection_has_error success! = %u\n" , screen_num);

    const xcb_setup_t * setup = xcb_get_setup(conn);
    xcb_screen_iterator_t iter = xcb_setup_roots_iterator (setup);

    for (; iter.rem > 0; xcb_screen_next(&iter))
    {
        
    }
    // // for (int i = 0; i < screen_num; ++i)
    // {
    //     xcb_screen_next (&iter);
    // }


    screen = iter.data;

    // qDebug() << "Informations of screen " << screen->root << screen->width_in_pixels << screen->height_in_pixels;
    m_iDesktopWidth = screen->width_in_pixels;
    m_iDesktopHeight = screen->height_in_pixels;

    // qDebug() << "xcb success!";
}
int main(int argc, char *argv[])
{

    InitVideoInput();

    while(1)
    {
            //xgrab方法
            iq = xcb_get_image(conn, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root, 0, 0, m_iDesktopWidth, m_iDesktopHeight, ~0);
            img = xcb_get_image_reply(conn, iq, &e);
            if (e)
            {
                // qDebug() << "xcb_get_image_reply faild!";
                continue;
            }
            uint8_t *data;
            if (!img)
            {
                // qDebug() << "img = NULL faild!";
                continue;
            }
            data   = xcb_get_image_data(img);
            int length = xcb_get_image_data_length(img);
    }

}



