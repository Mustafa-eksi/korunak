#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define LS_BUFFER_CAP 8192



void detect_new_usbs(void);
