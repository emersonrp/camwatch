#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <Imlib.h>

#include "camwatch.h"
#include "get.h"

Prefs prefs;
Url *url = NULL;
Window win;
ImlibImage *im;
ImlibData *id;
Display *disp;

void load_prefs(int argc, char *argv[]);
int save_current_image(void);
int update_image(void);
void alarm_handler(int sig);
void usage(void);
int main(int argc, char *argv[]);



int main(int argc, char *argv[]) {
  XSetWindowAttributes attr;
  XClientMessageEvent *client_event;
  Atom delwin_atom, proto_atom;
  char ch, buf[20], *windowname;

  if(argc < 2)
    usage();

  load_prefs(argc, argv);
  
  disp=XOpenDisplay(NULL);
  id=Imlib_init(disp);

  /* setup attributes for new window */
  attr.event_mask = ButtonPressMask | KeyPressMask | StructureNotifyMask;
  attr.background_pixel = BlackPixel(disp, DefaultScreen(disp));

  /* Create a Window to display in */
  win=XCreateWindow(disp,DefaultRootWindow(disp),0,0,DEFAULT_WIDTH,
      DEFAULT_HEIGHT,0,id->x.depth, InputOutput,
      id->x.visual,CWEventMask | CWBackPixel, &attr);

  /* set the windows properties so it accepts WM_DELETE_WINDOW messages */
  proto_atom = XInternAtom(disp, "WM_PROTOCOLS", False);
  delwin_atom = XInternAtom(disp, "WM_DELETE_WINDOW", False);
  if ((proto_atom != None) && (delwin_atom != None))
    XChangeProperty(disp, win, proto_atom, XA_ATOM, 32,
        PropModeReplace, (unsigned char *)&delwin_atom, 1);
  windowname = malloc (strlen(url->file) + strlen(VERSION) + 10);
  sprintf(windowname,"%s - camwatch %s",url->file, VERSION);
  XStoreName(disp, win, windowname);
  free(windowname);

  /* Actually display the window */
  XMapWindow(disp,win);

  /* Force one round of SIGALRM to get the image the first time,
   * and start the actual alarm ticking.... */
  signal(SIGALRM, alarm_handler);
  raise(SIGALRM);

  /* X event handling loop */
  while(1) {
    XEvent ev;
    XNextEvent(disp, &ev);
    switch(ev.type) {
    case ClientMessage:
      client_event = (XClientMessageEvent *) &ev;
      if(client_event->data.l[0] == delwin_atom) {
  exit(0);
      } 
      break;
    case KeyPress:
      ch = XLookupString ((XKeyEvent *)&ev, buf, 20, NULL, 0);
      switch(buf[0]) {
      case 'q': case 'x': case 'Q': case 'X':
  dprintf("User hit %c key, quitting.\n", buf[0]); fflush(stdout);
  exit(0);
  break;
      case 'r': case 'R':
  dprintf("User hit %c key, forcing refresh/reload.\n", buf[0]);fflush(stdout);
  raise (SIGALRM);
  break;
      case 's' : case 'S':
  dprintf("User hit %c key, saving image....\n",buf[0]);fflush(stdout);
  save_current_image();
  break;
      default:
  break;
      }
    }
    /* do a microsleep for 0.1 second so we don't eat cycles just in case */
    /*    usleep(100000); */
  }

  return 0;
}

void alarm_handler(int sig) {
  signal (SIGALRM, SIG_IGN);
  dprintf("caught a SIGALRM signal...\n");
  update_image();
  dprintf("setting new alarm for %d seconds.\n", prefs.delay);
  alarm(prefs.delay);
  signal (SIGALRM, alarm_handler);
}

int update_image(void) {
  char * filename;
  dprintf("getting image...\t"); fflush(stdout);
  filename = get(url);
  dprintf("got image, updating...\n");
  im=Imlib_load_image(id,filename);
  XResizeWindow(disp, win, im->rgb_width, im->rgb_height);
  Imlib_render(id,im,im->rgb_width,im->rgb_height);
  Imlib_apply_image(id,im,win);
  Imlib_kill_image(id, im);
  XClearWindow(disp, win);
  XSync(disp,False);
  return 1;
}

void load_prefs(int argc, char *argv[]) {
  extern char *optarg;
  int c;
  char buf[1000];
 
  /* set defaults first... */
  prefs.archive = 0;
  prefs.delay = DDELAY;
  prefs.tmpfile = NULL;
  
  /* call getopt() to deal with cmdline args */
  while((c = getopt(argc, argv, "d:t:as:i:u:h")) != EOF) {
    switch(c) {
    case 'd':
      prefs.delay = atoi(optarg);
      break;
    case 't':
      prefs.tmpfile = strdup(optarg);
      break;
    case 'h':
      usage();
      break;
    case 'a':
      prefs.archive = 1;
      break;
    case 's':
      if (chdir(optarg) != 0)
        perror ("Changing to specified archive directory");
      getcwd(buf,1000);
      dprintf("cwd is %s\n",buf);
      break;
    case 'u':
      url = new_url(url, optarg);
      break;
    case 'i':
      prefs.userpass = strdup(optarg);
      break;
    default:
      /* do nothing */
    }
  }
  if (url == NULL) {
    usage();
    exit(0);
  }

  if (url->file == NULL) {
    printf("ERROR:  The URL following the -u option is mandatory, and MUST point to\nan actual image file.\n\n");
    usage();
    exit(0);
  }

  if(prefs.tmpfile == NULL) {
    prefs.tmpfile = strdup(url->file);
  }

  /* prompt user for password if none given */
  if ((prefs.userpass != NULL) && strchr(prefs.userpass,':') == NULL) {
  static char prompt[] = "Password for username %s:";
  char passprompt [ strlen(prefs.userpass) + sizeof(prompt) ];
  char *temppass, *password;

    sprintf (passprompt,prompt,prefs.userpass);

  password = getpass(passprompt);
  
  temppass = malloc (strlen(prefs.userpass) + strlen(password) + 2);
  sprintf(temppass, "%s:%s",prefs.userpass, password);
  free (prefs.userpass);
  prefs.userpass = strdup(temppass);

  memset((void *) password, '\0', strlen(password));
  free(password);
  free(temppass);
  
  }

}

void usage(void) {
  printf("camwatch v%s\n", VERSION);
  printf("usage: camwatch [options] -u <url-of-picture-file>\n\n");
  printf("Options include:\n");
  printf("\t-d <secs>\tdelay in seconds between captures\n");
  printf("\t-t <filename>\ttemporary filename to use\n");
  printf("\t-i <uname:pwd>\tHTTP Auth info -- username:password\n");
  printf("\t\t\tPassword optional;  if missing, user will be prompted.\n");
  printf("\t-a\t\tArchive the captures with incrementing filenames.\n");
  printf("\t-s <dir>\tDirectory for archive and/or snapshot saves.\n");
  printf("\t-h\t\tdisplay this help text\n");
  exit(1);
}


int save_current_image(void){
/*  int tmpflag = prefs.archive;
  prefs.archive = 1;
  raise (SIGALRM);
  prefs.archive = tmpflag;*/


  FILE *tmpfile, *newfile;
  char *newfilename;
  int buf[1000], tail;

  newfilename = datestamp_filename ();
  if ((newfile = fopen(newfilename,"w")) == NULL) {
    perror("Unable to open save file");
    return 0;
  }
  if ((tmpfile = fopen(prefs.tmpfile, "r")) == NULL ) {
    perror("Unable to open temp file for reading.");
    return 0;
  }
  while(fread(&buf,sizeof(int),1000,tmpfile) == 1000) {
    tail = fwrite (&buf, sizeof(int),1000,newfile);
  }
  fwrite (&buf, sizeof(int),tail,newfile);
  fclose (newfile);
  fclose (tmpfile);
  return 1;
  
}
