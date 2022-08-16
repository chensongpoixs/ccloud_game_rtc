/*************************************************************************************************************************
 https://blog.csdn.net/weixin_36466165/article/details/116657231

 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <erro>
//#include <io.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>


#define KEYBOARD "/dev/input/event1"
#define MOUSE "/dev/input/event3"


void move_mouse(int fd,int x,int y);
void keydown(int fd,int key);
void keyup(int fd,int key);
void mousekeydown(int fd,int key);
void mousekeyup(int fd,int key);
int initkeyboard(const char* keyboard);
int initmouse(const char* mouse);







void move_mouse(int fd,int x,int y)
{
    struct input_event event,ev;
    memset(&event, 0, sizeof(event));
    gettimeofday(&event.time, NULL);
    event.type = EV_REL;
    event.code = REL_X;
    event.value = x;
    int len = write(fd, &event, sizeof(event));
    if(len != sizeof(struct input_event))
    {
        printf("error to write\n");
        exit(1);
    }
    event.type = EV_REL;
    event.code = REL_Y;
    event.value = y;
    len = write(fd, &event, sizeof(event));
    if(len != sizeof(struct input_event))
    {
        printf("error to write2\n");
        exit(1);
    }
    event.type = EV_SYN;
    event.code = SYN_REPORT;
    event.value = 0;
    len = write(fd, &event, sizeof(event));
    if(len != sizeof(struct input_event))
    {
        printf("error to write3\n");
        exit(1);
    }

}

void keydown(int fd,int key)
{
    struct input_event event;
    event.type = EV_MSC;
    event.code = 4;
    event.value = key;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }

    event.type = EV_KEY;
    event.code = key;
    event.value = 1;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
//        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }
    event.type = EV_SYN;
    event.code = 0;
    event.value = 0;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
//        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }

}
void keyup(int fd,int key)
{
    struct input_event event;
    event.type = EV_MSC;
    event.code = 4;
    event.value = key;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
//        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }

    event.type = EV_KEY;
    event.code = key;
    event.value = 0;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
//        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }
    event.type = EV_SYN;
    event.code = 0;
    event.value = 0;
    gettimeofday(&event.time,0);
    if(write(fd,&event,sizeof(event)) < 0)
    {
//        printf("error open keyboard:%s\n",strerror(errno));
        exit(1);
    }
}
void mousekeydown(int fd,int key)
{
    struct input_event event,ev;
    memset(&event, 0, sizeof(event));
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = key;
    event.value = 1;
    int len = write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write1\n");
        exit(1);
    }
    event.type = EV_REL;
    event.code = 0;
    event.value = 1;
    write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write2\n");
        exit(1);
    }
    event.type = EV_REL;
    event.code = 1;
    event.value = 1;
    write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write2\n");
        exit(1);
    }

    write(fd,(char *)&ev,sizeof(struct input_event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write\n");
        exit(1);
    }


}
void mousekeyup(int fd,int key)
{
    struct input_event event,ev;
    memset(&event, 0, sizeof(event));
    memset(&ev, 0, sizeof(ev));
    gettimeofday(&event.time, NULL);
    event.type = EV_KEY;
    event.code = key;
    event.value = 0;
    int len = write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write1\n");
        exit(1);
    }
    event.type = EV_REL;
    event.code = 0;
    event.value = 1;
    write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write\n");
        exit(1);
    }
    event.type = EV_REL;
    event.code = 1;
    event.value = 1;
    write(fd, &event, sizeof(event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write\n");
        exit(1);
    }

    write(fd,(char *)&ev,sizeof(struct input_event));
    if(len  != sizeof(struct input_event))
    {
        printf("error in write3\n");
        exit(1);
    }
}
int initkeyboard(const char* keyboard)
{
    int fd;
    fd = open(keyboard,O_RDWR);
    if(fd) return fd;
    return 0;
}
int initmouse(const char* mouse)
{
    int fd;
    fd = open(mouse,O_RDWR);
    if(fd) return fd;
    return 0;
}




int main()
{
    int fd_keyboard = initkeyboard(KEYBOARD);
    sleep(3);
    keydown(fd_keyboard,KEY_A);
    keyup(fd_keyboard,KEY_A);
    int fd_mouse = initmouse(MOUSE);
    move_mouse(fd_mouse,200,-200);
    printf("read to press key\n");
    printf("read to press mouse left key\n");
    sleep(3);
    mousekeydown(fd_mouse,BTN_LEFT);//左键按下
    mousekeyup(fd_mouse,BTN_LEFT);  //左键弹起
    printf("read to press mouse right key\n");
    sleep(3);
    mousekeydown(fd_mouse,BTN_RIGHT);//右键按下
    mousekeyup(fd_mouse,BTN_RIGHT);  //右键弹起
    //这里没有写close(fd);等操作。其实我不知道是自己系统的问题还是什么，一旦有这些关闭fd操作的鼠标键盘便会失去控制，希望大家看完这些代码后测试一下，如果可以的额话可以评论，可以邮件，谢谢。
    return 0;
}

