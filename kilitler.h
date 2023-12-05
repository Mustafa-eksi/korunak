#include <sys/mount.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define LS_BUFFER_CAP 4096
#define MAX_USB 32
#define MAX_PART 16

typedef struct usb_storage {
	char* disk_name;
	char dev_paths[MAX_PART][LS_BUFFER_CAP];
	size_t devp_index;
	bool checked;
}usb_storage;

typedef struct kilit_state {
	usb_storage storages[MAX_USB];
	size_t usb_index;
	bool kilitli;
} kilit_state;

void detect_new_usbs(void);

void dump_durum(void);

void mount_usb(usb_storage storage);
