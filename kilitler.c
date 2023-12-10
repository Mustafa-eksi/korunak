#include "kilitler.h"

void mount_usb(usb_storage storage) {

}

void dump_durum(const kilit_state current_state) {
	for(size_t i = 0; i < current_state.usb_index; i++) {
		printf("uuid: %s\n", current_state.storages[i].uuid);
		for(size_t j = 0; j < current_state.storages[i].devp_index; j++){
			printf("	dev path %ld: %s mountpoint: %s\n",
				j,
				current_state.storages[i].mount_devs[j].dev_path,
				current_state.storages[i].mount_devs[j].mount_point);
		}
	}
	for(size_t i = 0; i < current_state.stored_count; i++) {
		printf("stored uuid: %s\n", current_state.stored[i].uuid);
		printf("stored password: %s\n", current_state.stored[i].hashed_password);
	}
}


void add_to_storage(kilit_state *current_state, const char* link_path) {
	// current_state->storages'da zaten var olan bir usb ise tüm diğer usb'lere bak 
	for(size_t i = 0; i < current_state->usb_index; i++) {
		// "/dev/sda"
		if(strncmp(current_state->storages[i].mount_devs[0].dev_path, link_path, 8) == 0) {
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
	//strncpy(current_state->storages[current_state->usb_index].uuid, uuid, strlen(uuid));
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

void split_line_in_two(char line[HASHED_CAP], char first[HASHED_CAP], char second[HASHED_CAP]) {
	for(size_t i = 0; i < strlen(line); i++) {
		if(line[i] == ' ') {
			strncpy(first, line, i);
			// ab cde
			strncpy(second, line+(i+1), strlen(line)-(i+1));
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
	sprintf(command, "lsblk -l -o name,mountpoint,UUID %s\n", devices_first_paths);
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
		// "sdb        "
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
					char mount_point_trimmed[PATH_CAP] = {0}; 
					trim_left(
						mount_point_trimmed,
						mount_point
					);
					if(strlen(mount_point_trimmed) > 1) {
						char uuid_ham[PATH_CAP] = {0};
						char uuid_trimmed[PATH_CAP] = {0};
						split_line_in_two(mount_point_trimmed,
							current_state->storages[ij.i].mount_devs[ij.j].mount_point, uuid_ham);
						trim_left(uuid_trimmed, uuid_ham);
						strncpy(current_state->storages[ij.i].uuid, uuid_trimmed, strlen(uuid_trimmed));
						current_state->storages[ij.i].mount_devs[ij.j].is_mounted = true;
						current_state->storages[ij.i].does_have_mounted_part = true;	
					}					
				}
				break;
			}
		}
		
		/*printf("%ld, %ld, devname: %s mountpoint: baş'%s'son mountpoint: %s\n", 
			ij.i, ij.j, name,
			current_state->storages[ij.i].mount_devs[ij.j].mount_point,
			mount_point);*/
		line_index++;
	}
	pclose(fl);
}

bool search_password(const kilit_state current_state, password_device non_hashed) {
	for(size_t i = 0; i < current_state.stored_count; i++) {
		printf("--- stored uuid: '%s' uuid got: '%s'\n", current_state.stored[i].uuid, non_hashed.uuid);
		if(strcmp(current_state.stored[i].uuid, non_hashed.uuid) == 0) {
			printf("non_hashed: '%s'\nhashed: '%s'\n", non_hashed.hashed_password, current_state.stored[i].hashed_password);
			int ret = bcrypt_checkpw(non_hashed.hashed_password, current_state.stored[i].hashed_password);
			if(ret < 0) {
				printf("ERROR: bcrypt_checkpw returned %d\n", ret);
				exit(-1);
			}
			return ret == 0;
		}
	}
	printf("false\n");
	return false;
}

bool search_for(const kilit_state current_state, const char mountpoint[PATH_CAP], const char uuid[PATH_CAP]) {
	//printf(" --- search_for: mp: %s, uuid: %s\n",mountpoint,uuid);
	char buff[COMMAND_OUTPUT_CAP] = {0};
	char command[PATH_CAP] = {0};
	sprintf(command, "find %s -name "PASS_FILE_NAME, mountpoint);
	FILE *fl = popen(command, "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return false;
	}
	bool ret = false;
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		char clean_path[COMMAND_OUTPUT_CAP] = {0};
		strncpy(clean_path, buff, strlen(buff)-1); // trim the last '\n'
		int non_hashed = open(clean_path, O_RDONLY);
		char non_hashed_content[NON_HASHED_CAP] = {0};
		read(non_hashed, non_hashed_content, NON_HASHED_CAP);
		password_device passdev = {0};
		strcpy(passdev.uuid, uuid);
		strncpy(passdev.hashed_password, non_hashed_content, strlen(non_hashed_content)-1);
		ret = search_password(current_state, passdev);
		break;
		//printf("-- %s\n", non_hashed_content);
	}
	pclose(fl);
	return ret;
}

bool search_non_hashed(kilit_state *current_state) {
	for(size_t i = 0; i < current_state->usb_index; i++) {
		if(!current_state->storages[i].does_have_mounted_part)
			continue;
		for(size_t j = 0; j < current_state->storages[i].devp_index; j++) {
			if(!current_state->storages[i].mount_devs[j].is_mounted)
				continue;
			bool ret = search_for(*current_state,
				current_state->storages[i].mount_devs[j].mount_point,
				current_state->storages[i].uuid);
			if(ret) return true;
		} 
	}
	return false;
}


void parse_file(kilit_state *current_state, char passwords[HASH_FILE_CAP], ssize_t count) {
	int line_index = 0;
	int last_index = 0;
	for(ssize_t i = 0; i < count; i++) {
		char one_line[HASHED_CAP] = {0};
		if(passwords[i] == '\n') {
			strncpy(one_line, passwords+last_index, i-last_index);
			printf("one_line: %s\n", one_line);
			if(strlen(one_line) < 3) {
				continue;
			}
			split_line_in_two(
				one_line,
				current_state->stored[line_index].uuid,
				current_state->stored[line_index].hashed_password
				);
			printf("uuid: '%s'\n	hash: '%s'\n",
			current_state->stored[line_index].uuid,
			current_state->stored[line_index].hashed_password);
			current_state->stored_count++;
			line_index++;
			last_index = i+1;
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
	const char* usb_directory = "/dev/disk/by-id/";
	char buff[COMMAND_OUTPUT_CAP] = {0};
	
	DIR *d;
	struct dirent *dir;
	
	d = opendir(usb_directory);
	if(d == NULL) {
		printf("Opening /dev/disk/by-id/ is not possible...\n");
		return;
	}
	while ((dir = readdir(d)) != NULL)
        {
		if(strncmp(dir->d_name, "usb-", 4) == 0) {
			strcpy(buff, usb_directory);
			strncpy(buff+strlen(usb_directory), dir->d_name, strlen(dir->d_name));
			char link_buffer[PATH_CAP] = {0};
			ssize_t length = 0;
			if((length = readlink(buff, link_buffer, strlen(buff)-1)) == -1) {
				printf("Error at readlink: %s\n", strerror(errno));
				break;
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
			
			add_to_storage(current_state, link_buffer);
			
			memset(buff, 0, strlen(buff));
		}
        }
        printf("opendir ended\n");
        closedir(d);
        
	if(current_state->usb_index != 0)
		get_mountpoints(current_state);
}
