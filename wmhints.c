#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>

#include "camwatch.h"

void usage(void);

int main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  Display *disp;
  ImlibData *id;
  XSetWindowAttributes attr;
  Window win;
  KeySym ksym;
  ImlibImage *im;
  Pixmap p,m;
  char ch;
  char buf[20];
  int w,h,i;

  int rv;
  char *title = "camwatch";
  XTextProperty *xtitle = NULL;
  Atom delwin_atom, proto_atom;
  XClassHint classhint;
  XClientMessageEvent *client_event;
  int event_mask;

  disp=XOpenDisplay(NULL);
  id=Imlib_init(disp);

  /* Load the image specified as the first argument */
  im=Imlib_load_image(id,"tst.gif");
  w=im->rgb_width;h=im->rgb_height;

  /* setup attributes to use for new window */
  attr.event_mask = ButtonPressMask | KeyPressMask | StructureNotifyMask;

  /* Create a Window to display in */
  win=XCreateWindow(disp,DefaultRootWindow(disp),0,0,w,h,0,id->x.depth,
        InputOutput,id->x.visual,CWEventMask,&attr);

  /*  event_mask = StructureNotifyMask | ExposureMask | 
      ButtonPressMask | KeyPressMask | KeyReleaseMask;
      XSelectInput(disp,win,event_mask); */

  /* set the windows properties so it accepts WM_DELETE_WINDOW messages */
  proto_atom = XInternAtom(disp, "WM_PROTOCOLS", False);
  delwin_atom = XInternAtom(disp, "WM_DELETE_WINDOW", False);
  if ((proto_atom != None) && (delwin_atom != None))
    XChangeProperty(disp, win, proto_atom, XA_ATOM, 32,
        PropModeReplace, (unsigned char *)&delwin_atom, 1);

  /* Put the Image in the window, at the window's size */
  Imlib_apply_image(id,im,win);
  /* Actually display the window */
  XMapWindow(disp,win);
  /* Synchronise with the Xserver */
  XSync(disp,False);


  
  Imlib_kill_image(id, im);

#define HINTS 1
#if HINTS
  /* setup WM_HINTS stuff */
  /*  rv = XmbTextListToTextProperty(disp, "foof", 0, 
      XStringStyle, xtitle); */

  XStoreName(disp, win, "frank");
  /*  printf("rv = %d\n", rv); */
  fflush(stdout);
#endif /* HINTS */

  /* Event loop to handle resizes */   
  for(;;)
    {
      XEvent ev;
      
      /* Sit and wait for an event to happen */ 
      XNextEvent(disp,&ev);
      switch(ev.type) {
      case ClientMessage:
  printf("ClientMessage\n");
  client_event = (XClientMessageEvent *) &ev;
  if(client_event->data.l[0] == delwin_atom) {
    printf("WM_DELETE_WINDOW\n");
    exit(0);
  } 
  break;
      case MapNotify:
  printf("MapNotify\n"); fflush(stdout);
  XSync(disp,False);
  break;
      case DestroyNotify:
  printf("DestroyNotify\n"); fflush(stdout);
  exit(0);
      case ConfigureNotify:
  /* do nothing and sync server */
  printf("ConfigureNotify\n"); fflush(stdout);
  XSync(disp,False);
  break;
      case ButtonPress:
  printf("ButtonPress\n"); fflush(stdout);
  XSync(disp,False);
  break;
      case KeyPress:
  printf("KeyPress\n"); fflush(stdout);
  XSync(disp,False);
  printf("Key was: %d\n", ev.xkey.keycode);
  ch = XLookupString ((XKeyEvent *)&ev, buf, 20, NULL, 0);
  printf("ch: %c, buf: %s\n", ch, buf);
  switch(buf[0]) {
  case 'q': case 'x': case 'Q': case 'X':
    printf("Quitting.\n"); fflush(stdout);
    exit(0);
  }
  break;
      case Expose:
  printf("Expose\n"); fflush(stdout);
  XSync(disp,False);
  break;
      case UnmapNotify:
  printf("UnmapNotify\n"); fflush(stdout);
  XSync(disp,False);
  break;
      default:
  printf("default: %d\n", ev.type);
  break;
      }
    }
  
#if 0
  sleep(prefs.delay);

  while(get(url)) {
    printf("got image, updating...\n");
    im=Imlib_load_image(id,prefs.tmpfile);
    Imlib_apply_image(id,im,win);
    Imlib_kill_image(id, im);
    /*    XSync(disp, False); */
    printf("image updated, sleeping...\n");
    sleep(prefs.delay);
  }
#endif

  return 0;
}

void usage(void) {
  printf("usage: camwatch [-d delay] [-t tmpfile]"
   " http://some.host/dir/pic.jpg\n");
  
  exit(1);
}
