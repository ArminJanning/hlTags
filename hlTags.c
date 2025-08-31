#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXTAGS 64
#define MAXTAGNAMELENGTH 10
bool skipEmpty = false;

int main(int argc, char **argv) {
  if (argc > 1) {
    if (strncmp(argv[1], "--skip-empty", 13) == 0 ||
        strncmp(argv[1], "-e", 3) == 0) {
      skipEmpty = true;
    } else if (strncmp(argv[1], "--help", 7) == 0 ||
               strncmp(argv[1], "-h", 3) == 0) {
      printf("usage: hlTags [OPTION]\n");
      printf("-e, --skip-empty\tSkips empty tags. May still include tags "
             "that are empty but viewed\n");
      printf("-h, --help  \t\tDisplays this message\n");
      return 0;
    } else {
      printf("invalid parameter. See --help for more info\n");
      return 1;
    }
  }

  // get tags via hc tag_status
  // example output:    -0   #1   .2   .3   .4   .5   .6   .7
  char tagStatus[128];
  FILE *fp = popen("herbstclient tag_status", "r");
  if (!fp) { // NULL if popen failed
    printf("Could not get list of tags\n");
    return 1;
  }
  // hc tag_status only outputs one line
  fgets(tagStatus, sizeof(tagStatus), fp);
  pclose(fp);
  // printf("%s", tagStatus);

  // format ouput into simpler format and ignore empty tags

  /* ouput format:
    separated by /t

    . empty
    : not empty
    ! urgent window
    + viewed on specified MONITOR, but this monitor is not focused.
    # viewed on specified MONITOR and it is focused.
    - viewed on different MONITOR, but this monitor is not focused.
    % viewed on different MONITOR it is focused.
  */

  char tagName[MAXTAGS][MAXTAGNAMELENGTH];
  short j = 0;
  short k = 0;
  short i = 0;
  while (tagStatus[i] != '\0') {
    j = 0;
    switch (tagStatus[i]) {
    case '\t':
    case '\n':
      break;
    case '.': // empty tags
    case ':':
    case '!':
    case '+':
    case '#':
    case '-':
    case '%':
      if (tagStatus[i] == '.' && skipEmpty) {
        while (tagStatus[i] != '\t') {
          i++;
        }
        break;
      }
      j = 0;
      while (tagStatus[i + 1] != '\t') {
        i++;
        // printf("%c", tagStatus[i]);
        if (j < MAXTAGNAMELENGTH) {
          tagName[k][j] = tagStatus[i];
          j++;
          tagName[k][j] = '\0';
        } else {
          printf("tag name too long\n");
          exit(1);
        }
      }
      if (k >= MAXTAGS) {
        printf("Too many tags\n");
        exit(1);
      }
      k++;
      break;
    default:
      printf("Encountered unexpected char '%c'\n", tagStatus[i]);
      return 1;
    }
    i++;
  }

  // tagNAme[k] now contains a list of tag names.
  short maxTags = k;
  for (k = 0; k < maxTags; k++) {
    printf("%s\n", tagName[k]);
  }
  return 0;
}
