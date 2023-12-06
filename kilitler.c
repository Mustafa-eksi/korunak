#include "kilitler.h"

void mount_usb(usb_storage storage) {

}

void dump_durum(const kilit_state current_state) {
	for(size_t i = 0; i < current_state.usb_index; i++) {
		printf("device_name: %s\n", current_state.storages[i].disk_name);
		for(size_t j = 0; j < current_state.storages[i].devp_index; j++){
			printf("	dev path %ld: %s mountpoint: %s\n",
				j,
				current_state.storages[i].mount_devs[j].dev_path,
				current_state.storages[i].mount_devs[j].mount_point);
		}
	}
}

void add_to_storage(kilit_state *current_state, const char* link_path, const char* device_name) {
	// current_state->storages'da zaten var olan bir usb ise tüm diğer usb'lere bak 
	for(size_t i = 0; i < current_state->usb_index; i++) {
		if(strcmp(current_state->storages[i].disk_name, device_name) == 0) {
			// Aynı usb'nin başka bir kısmıysa ona ekle
			strncpy(
				current_state->storages[i].mount_devs[current_state->storages[i].devp_index].dev_path,
				link_path, strlen(link_path)
			);
			current_state->storages[i].devp_index += 1;
			return;
		}
	}
	// Kod buraya geliyorsa yukarda eklememiş demektir. Bu da ayrı eklenmesi
	// gerektiği anlamına geliyor.
	current_state->storages[current_state->usb_index].devp_index = 1;
	current_state->storages[current_state->usb_index].checked = false;
	strncpy(current_state->storages[current_state->usb_index].disk_name, device_name, strlen(device_name));
	strncpy(current_state->storages[current_state->usb_index].mount_devs[0].dev_path, link_path, strlen(link_path));
	current_state->usb_index += 1;
	return;
}

struct two_indexes get_index_from_devname(kilit_state durum, char dev_name[PATH_CAP]) {
	char path[PATH_CAP] = {0};
	strncpy(path, "/dev/", 5);
	strcpy(path+5, dev_name);
	//printf("hmm: %s\n", path);
	for(size_t i = 0; i < durum.usb_index; i++) {
		for(size_t j = 0; j < durum.storages[i].devp_index; j++) {
			if(strcmp(durum.storages[i].mount_devs[j].dev_path, path) == 0) {
				return (struct two_indexes) {
					.i = i, .j = j
				};
			}
		}
	}
	return (struct two_indexes) {.i = -1, .j = -1};
}

void trim_left(char* destination, const char str[PATH_CAP]) {
	for(size_t i = 0; i < strlen(str); i++) {
		if(str[i] != ' ') {
			strncpy(destination, str+i, strlen(str)-i);
			return;
		}
	}
}

void get_mountpoints(kilit_state *current_state) {
	char devices_first_paths[PATH_CAP] = {0};
	size_t str_index = 0;
	for(size_t i = 0; i < current_state->usb_index; i++) {
		strcpy(devices_first_paths+str_index,
			current_state->storages[i].mount_devs[0].dev_path);
		str_index += strlen(current_state->storages[i].mount_devs[0].dev_path);
		devices_first_paths[str_index] = ' ';
		str_index++;
	}
	char command[COMMAND_OUTPUT_CAP] = {0};
	sprintf(command, "lsblk -l -o name,mountpoint %s\n", devices_first_paths);
	//printf("first: %s\n command: %s\n", devices_first_paths, command);
	char buff[COMMAND_OUTPUT_CAP] = {0};
	FILE *fl = popen(command, "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return;
	}
	size_t line_index = 0;
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		if(line_index == 0) {
			line_index++;
			continue;
		}
		//printf("line_index: %ld, buff: '%s'\n", line_index, buff);
		// split and take first
		char name[COMMAND_OUTPUT_CAP] = {0};
		char mount_point[PATH_CAP] = {0};
		struct two_indexes ij = {0};
		// "sda1 /media/mustafa/Ventoy\n"
		for(size_t i = 0; i < strlen(buff)-1; i++) {
			if(buff[i] == ' ') {
				strncpy(name, buff, i); // sorun yok
				ij = get_index_from_devname(*current_state,
								name);
				if(strlen(buff)-(i+2) > 0) { // sorun yok
					// bunu açıklayamam, test ederek buldum:
					strncpy(
						mount_point,
						buff+i+1, strlen(buff)-(i+2)
					);
					trim_left(
						current_state->storages[ij.i].mount_devs[ij.j].mount_point,
						mount_point
					);
					current_state->storages[ij.i].mount_devs[ij.j].is_mounted = true;
					current_state->storages[ij.i].does_have_mounted_part = true;
				}
				break;
			}
		}
		
		printf("%ld, %ld, devname: %s mountpoint: baş'%s'son mountpoint: %s\n", 
			ij.i, ij.j, name,
			current_state->storages[ij.i].mount_devs[ij.j].mount_point,
			mount_point);
		line_index++;
	}
}

bool check_for_password(char non_hashed[NON_HASHED_CAP], const char salt[BCRYPT_HASHSIZE]) {
	char result[HASHED_CAP] = {0};
	bcrypt_hashpw(non_hashed, salt, result);
	printf("non_hashed: %s, hashed: %s\n", non_hashed, result);
	return true;
}

bool search_for(char mountpoint[PATH_CAP], const char salt[BCRYPT_HASHSIZE]) {
	char buff[COMMAND_OUTPUT_CAP] = {0};
	char command[PATH_CAP] = {0};
	sprintf(command, "find %s -name "PASS_FILE_NAME, mountpoint);
	FILE *fl = popen(command, "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return false;
	}
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		char clean_path[COMMAND_OUTPUT_CAP] = {0};
		strncpy(clean_path, buff, strlen(buff)-1); // trim the last '\n'
		int non_hashed = open(clean_path, O_RDONLY);
		char non_hashed_content[NON_HASHED_CAP] = {0};
		read(non_hashed, non_hashed_content, NON_HASHED_CAP);
		return check_for_password(non_hashed_content, salt);
		//printf("-- %s\n", non_hashed_content);
	}
	return true;
}

void search_non_hashed(kilit_state *current_state) {
	for(size_t i = 0; i < current_state->usb_index; i++) {
		if(!current_state->storages[i].does_have_mounted_part)
			continue;
		for(size_t j = 0; j < current_state->storages[i].devp_index; j++) {
			if(!current_state->storages[i].mount_devs[j].is_mounted)
				continue;
			printf("%s", current_state->storages[i].mount_devs[j].mount_point);
			search_for(current_state->storages[i].mount_devs[j].mount_point,
				current_state->salt);
		} 
	}
}

void parse_file(kilit_state *current_state, char passwords[HASH_FILE_CAP], ssize_t count) {
	int line_index = 0;
	char salt[BCRYPT_HASHSIZE];
	int last_index = 0;
	for(ssize_t i = 0; i < count; i++) {
		char one_line[HASHED_CAP] = {0};
		if(passwords[i] == '\n') {
			if(line_index == 0) {
				strncpy(salt, passwords, i);
			}else {
				strncpy(one_line, passwords+last_index, i-last_index);
				printf("one_line: %s\n", one_line);
			}
			line_index++;
			last_index = i;
			continue;
		}
	}
}

void sync_with_file(kilit_state *current_state) {
	if (access(HASHED_PATH, F_OK) == 0) {
		int hashed_fd = open(HASHED_PATH, O_RDONLY);
		if(hashed_fd == -1) {
			exit(-1);
		}
		char passwords[HASH_FILE_CAP];
		ssize_t count = read(hashed_fd, passwords, HASH_FILE_CAP);
		parse_file(current_state, passwords, count);
	} else {
		exit(-1);
	}
}

void init_kilit(kilit_state *current_state) {
	sync_with_file(current_state);
	if(bcrypt_gensalt(5, current_state->salt) != 0) {
		printf("GENSALT FAILED\n");
	}
	char buff[COMMAND_OUTPUT_CAP] = {0};
	FILE *fl = popen("ls /dev/disk/by-id/usb-* ", "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return;
	}
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		buff[strlen(buff)-1] = 0;
		//printf("%s\n", buff);
		char link_buffer[PATH_CAP] = {0};
		ssize_t length = 0;
		if((length = readlink(buff, link_buffer, strlen(buff)-1)) == -1) {
			printf("Error at readlink: %s\n", strerror(errno));
			return;
		}
		link_buffer[length] = '\0';
		strncpy(link_buffer, "/dev", 4);
		for(size_t i = 4; i < strlen(link_buffer); i++) {
			link_buffer[i] = link_buffer[i+1];
			if(i == strlen(link_buffer)-1) {
				link_buffer[i] = 0;
				break;
			}
		}
		char device_name[PATH_CAP] = {0};
		for(size_t i = 20; i < strlen(buff); i++) {
			if(buff[i] == '-') {
				strncpy(device_name, buff+20, i-20);
				break;
			}
		}
		
		add_to_storage(current_state, link_buffer, device_name);
		
		memset(buff, 0, strlen(buff));
	}
	if(current_state->usb_index != 0)
		get_mountpoints(current_state);
	pclose(fl);
}
