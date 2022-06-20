#include "xdo_cmd.h"
#include <string.h>

int cmd_click(context_t *context) 
{
  printf("===================================================================\n");
  printf("[prog = %p][argc = %d]\n", context->prog, context->argc);
  for (int i = 0; i < context->argc; ++i)
  {
    printf("argv[%d]= %s," , i, context->argv[i]);
    
  }
  printf("\n");
  printf("[debug = %d][window = %p][nwindows = %d]\n", context->debug, context->windows, context->nwindows);
  printf("[last_mouse_x = %d][last_mouse_y = %d][last_mouse_screen = %d][have_last_mouse = %d]\n", context->last_mouse_x, context->last_mouse_y, context->last_mouse_screen, context->have_last_mouse);


  printf("===================================================================\n");
  int button;
  char *cmd = context->argv[0];
  int ret = 0;
  int clear_modifiers = 0;
  charcodemap_t *active_mods = NULL;
  int active_mods_n;
  char *window_arg = NULL;
  useconds_t delay = 100000; /* 100ms */
  int repeat = 1;

  int c;
  enum { 
    opt_unused, opt_help, opt_clearmodifiers, opt_window, opt_delay,
    opt_repeat
  };
  static struct option longopts[] = {
    { "clearmodifiers", no_argument, NULL, opt_clearmodifiers },
    { "help", no_argument, NULL, opt_help },
    { "window", required_argument, NULL, opt_window },
    { "delay", required_argument, NULL, opt_delay },
    { "repeat", required_argument, NULL, opt_repeat },
    { 0, 0, 0, 0 },
  };
  static const char *usage = 
            "Usage: %s [options] <button>\n"
            "--clearmodifiers       - reset active modifiers (alt, etc) while typing\n"
            "--window WINDOW        - specify a window to send click to\n"
            "--repeat REPEATS       - number of times to click. Default is 1\n"
            "--delay MILLISECONDS   - delay in milliseconds between clicks.\n"
            "    This has no effect if you do not use --repeat.\n"
            "    Default is 100ms\n"
            "\n"
            "Button is a button number. Generally, left = 1, middle = 2, \n"
            "right = 3, wheel up = 4, wheel down = 5\n";
  int option_index;
  printf("wait 3 s!!\n");
  sleep(3);
  while ((c = getopt_long_only(context->argc, context->argv, "+cw:h",
                               longopts, &option_index)) != -1) 
                               {
    printf("[c = %c]\n", c);
    switch (c) {
      case 'h':
      case opt_help:
        printf(usage, cmd);
        consume_args(context, context->argc);
        return EXIT_SUCCESS;
        break;
      case 'c':
      case opt_clearmodifiers:
        clear_modifiers = 1;
        break;
      case 'w':
      case opt_window:
        clear_modifiers = 1;
        window_arg = strdup(optarg);
        break;
      case 'd':
      case opt_delay:
        delay = strtoul(optarg, NULL, 0) * 1000; /* convert ms to usec */
        break;
      case 'r':
      case opt_repeat:
        repeat = atoi(optarg);
        if (repeat <= 0) { 
          fprintf(stderr, "Invalid repeat value '%s' (must be >= 1)\n", optarg);
          fprintf(stderr, usage, cmd);
          return EXIT_FAILURE;
        }
        break;
      default:
        fprintf(stderr, usage, cmd);
        return EXIT_FAILURE;
    }
  }
  printf("[%s][%s][%d][optind = %d]\n", __FILE__, __FUNCTION__, __LINE__, optind);
  sleep(1);
  consume_args(context, optind);
 printf("[%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__);
  if (context->argc < 1) {
    fprintf(stderr, usage, cmd);
    fprintf(stderr, "You specified the wrong number of args.\n");
    return EXIT_FAILURE;
  }
 printf("[%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__);
 sleep(3);
  button = atoi(context->argv[0]);
  
 printf("[%s][%s][%d][button = %d]\n", __FILE__, __FUNCTION__, __LINE__, button);
 sleep(3);
  window_each(context, window_arg, 
  {
     sleep(3);
    printf("[%s][%s][%d]=============\n", __FILE__, __FUNCTION__, __LINE__);
    if (clear_modifiers) 
    {
      xdo_get_active_modifiers(context->xdo, &active_mods, &active_mods_n);
      sleep(3);
    printf("[%s][%s][%d]^_^_^_^__^_^__^_^_^\n", __FILE__, __FUNCTION__, __LINE__);
      xdo_clear_active_modifiers(context->xdo, window, active_mods, active_mods_n);
    }
    sleep(3);
    printf("[%s][%s][%d]$$$$$$$$$$$$$$$$$$$$$$$\n", __FILE__, __FUNCTION__, __LINE__);
    ret = xdo_click_window_multiple(context->xdo, window, button, repeat, delay);
    if (ret != XDO_SUCCESS) 
    {
      fprintf(stderr, "xdo_click_window failed on window %ld\n", window);
      return ret;
    }
      sleep(3);
    printf("[%s][%s][%d]!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", __FILE__, __FUNCTION__, __LINE__);
  
    if (clear_modifiers) 
    {
      xdo_set_active_modifiers(context->xdo, window, active_mods, active_mods_n);
      free(active_mods);
    }
      sleep(3);
      printf("[%s][%s][%d]exit ----->n", __FILE__, __FUNCTION__, __LINE__);
  }); /* window_each(...) */
  sleep(3);
 printf("[%s][%s][%d]\n", __FILE__, __FUNCTION__, __LINE__);
  consume_args(context, 1);
  return ret;
}
