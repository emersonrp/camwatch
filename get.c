/* grab a url to a file */
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include "camwatch.h"
#include "get.h"

int verify_image(FILE * outfile) {
  struct stat * buf;
  buf = malloc(sizeof(struct stat));
  if (fstat(fileno(outfile), buf) != 0) {
    perror("Stat of output file failed");
    return 0;
  }
  if (buf->st_size == 0) {
    free(buf);
    return 0;
  }
  /*Later, maybe shove it thru libjpeg or the like to make sure
   * that it's a correct/complete image... */
  return 1;
}

char * get(Url *u) {
  extern Prefs prefs;

  CURL *curl;
  CURLcode res;
  FILE *outfile;
  char *outfilename = prefs.tmpfile;
  
  curl = curl_easy_init();
  if (curl) {
    int success = 0;

    if (prefs.archive) {
      outfilename = datestamp_filename();
    }

    outfile = fopen(outfilename, "w");
    
    /* Really, this should have a max counter for retries so we
     * don't get all spammy with trying to get a broken image
     * over and over and over */
    while (!success) {
      curl_easy_setopt(curl, CURLOPT_URL, u->urlstring);
    if (prefs.userpass != NULL) {
      curl_easy_setopt(curl, CURLOPT_USERPWD, prefs.userpass);
    }
    curl_easy_setopt(curl, CURLOPT_FILE, outfile);
    res = curl_easy_perform(curl);
  
    success = verify_image(outfile);
    }

    fclose (outfile);
    curl_easy_cleanup(curl);
    
  }
  return outfilename;
}

char * datestamp_filename () {
  extern Prefs prefs;
  char *filebase, *filename, *suffix;
  time_t rawtime;
  struct tm * timeinfo;
  
  int suffixlength, i;
  
  filename = malloc (strlen(prefs.tmpfile) + 15);
  for (i = strlen(prefs.tmpfile); i > 0 && prefs.tmpfile[i] != '.'; i--);
  filebase = malloc(i);
  strncpy (filebase, prefs.tmpfile, i);
  filebase[i] = '\0';
  
  suffix = malloc((suffixlength = strlen(prefs.tmpfile) - i));
  strncpy (suffix, prefs.tmpfile+i+1, suffixlength);
  suffix[suffixlength] = '\0';
  
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  
  sprintf(filename,"%s-%d%02d%02d%02d%02d%02d.%s",filebase,
      timeinfo->tm_year+1900, timeinfo->tm_mon+1,
      timeinfo->tm_mday,timeinfo->tm_hour,
      timeinfo->tm_min,timeinfo->tm_sec,suffix);
  free (filebase);
  free (suffix); 
  
  return filename;
}
