#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "camwatch.h"
#include "url.h"

Url * new_url(Url *u, char *arg) {
  char *host;
  char *path;
  char *file;
  short i;

  /* set url struct up with null values */
  u = malloc(sizeof(Url));
  u->host = NULL;
  u->path = NULL;
  u->file = NULL;
  u->urlstring = arg;


  /* if addr starts with http://, skip that part */
  if(strncmp("http://", arg, 7) == 0)
    arg += 7;

  /* until first slash or end of string... */
  for(i = 0; arg[i] && arg[i] != '/'; i++);

  host = malloc(i + 1);
  memcpy(host, arg, i);
  host[i] = '\0';

  u->host = host;

  arg += i;

  /* find last slash */
  for(i = strlen(arg); i > 0 && arg[i] != '/'; i--);
  if(arg[i] != '/')
    return u;

  path = malloc(i + 2);
  memcpy(path, arg, i + 1);
  path[i] = '/';
  path[i+1] = '\0';
  /* strncat(path, "/", 2); */

  arg += i + 1;

  u->path = path;

  file = malloc(strlen(arg) + 1);
  memcpy(file, arg, strlen(arg) + 1);
  u->file = file;

  dprintf("host: %s\n", u->host);
  dprintf("path: %s\n", u->path);
  dprintf("file: %s\n", u->file);
  fflush(stdout);

  return u;
}
