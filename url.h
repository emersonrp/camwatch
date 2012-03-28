/* url structures and function prototypes */
#ifndef __URL_H__
#define __URL_H__

typedef struct {
  char *host;
  char *path;
  char *file;
  char *urlstring;
} Url;

Url * new_url(Url *u, char *arg);

#endif /* __URL_H__ */
