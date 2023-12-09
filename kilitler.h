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
#include <bcrypt.c>

#define COMMAND_OUTPUT_CAP 4096
#define PATH_CAP 1024
#define MAX_USB 32
#define MAX_PART 16
#define PASS_FILE_NAME ".non_hashed"
#define NON_HASHED_CAP 8192
#define HASHED_CAP 1024
#define MAX_PASSWORDS 64
#define HASH_FILE_CAP HASHED_CAP*MAX_PASSWORDS
#define HASHED_PATH "/etc/korunak_hashed"

typedef struct mount_dev {
	char dev_path[PATH_CAP];
	bool is_mounted;
	char mount_point[PATH_CAP];
}mount_dev;

typedef struct usb_storage {
	char uuid[PATH_CAP];
	mount_dev mount_devs[MAX_PART];
	size_t devp_index;
	bool does_have_mounted_part, checked;
}usb_storage;

typedef struct password_device {
	char uuid[HASHED_CAP];
	char hashed_password[HASHED_CAP];
}password_device;

typedef struct kilit_state {
	usb_storage storages[MAX_USB];
	size_t usb_index;
	bool kilitli;
	password_device stored[MAX_PASSWORDS];
	size_t stored_count;
} kilit_state;

struct two_indexes {
	size_t i, j;
};

void get_uuid(char dev_path[PATH_CAP], char uuid[PATH_CAP]);

void add_to_storage(kilit_state *current_state, const char* link_path);

void trim_left(char* destination, const char str[PATH_CAP]);

void get_mountpoints(kilit_state *current_state);

void detect_new_usbs(void);

void dump_durum(const kilit_state current_state);

bool search_password(const kilit_state current_state, password_device devpass);

bool check_for_password(char non_hashed[NON_HASHED_CAP], const char salt[BCRYPT_HASHSIZE]);

bool search_non_hashed(kilit_state *current_state);


void split_line_in_two(char line[HASHED_CAP], char first[HASHED_CAP], char second[HASHED_CAP]);
void parse_file(kilit_state *current_state, char passwords[HASH_FILE_CAP], ssize_t count);
void sync_with_file(kilit_state *current_state);
void init_kilit(kilit_state *current_state);

