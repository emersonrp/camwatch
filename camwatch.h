#ifndef __CAMWATCH_H__
#define __CAMWATCH_H__

#include <errno.h>
#include "url.h"

#define DDELAY   15  /* default delay */
#define VERSION  "0.2"
#define DEFAULT_WIDTH 320
#define DEFAULT_HEIGHT 240

#if 0
#define DEBUG
#endif

typedef struct {
  int  delay;
  char *tmpfile;
  char *userpass;
  int archive;
} Prefs;

#ifdef DEBUG
#define dprintf printf
#else
static void dprintf(char* txt, ...) {}
#endif

#endif /* __CAMWATCH_H__ */
