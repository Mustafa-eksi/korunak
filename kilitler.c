#include "kilitler.h"

kilit_state durum = {0};

void mount_usb(usb_storage storage) {

}

void dump_durum(void) {
	for(size_t i = 0; i < durum.usb_index; i++) {
		printf("device_name: %s\n", durum.storages[i].disk_name);
		for(size_t j = 0; j < durum.storages[i].devp_index-1; j++){
			printf("	dev path %ld: %s\n", j, durum.storages[i].dev_paths[j]);
		}
	}
}

void init_kilit(void) {
	char buff[LS_BUFFER_CAP] = {0};
	FILE *fl = popen("ls /dev/disk/by-id/usb-* ", "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return;
	}
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		buff[strlen(buff)-1] = 0;
		//printf("%s\n", buff);
		char link_buffer[LS_BUFFER_CAP] = {0};
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
		char device_name[LS_BUFFER_CAP] = {0};
		for(size_t i = 20; i < strlen(buff); i++) {
			if(buff[i] == '-') {
				strncpy(device_name, buff+20, i);
				break;
			}
		}
		if(durum.usb_index != 0) {
			bool eklendi = false;
			for(size_t i = 0; i < durum.usb_index; i++) {
				if(strcmp(durum.storages[i].disk_name, device_name) == 0) {
					
					strncpy(
					durum.storages[i].dev_paths[durum.storages[i].devp_index],
					link_buffer, strlen(link_buffer));
					durum.storages[i].devp_index += 1;
					eklendi = true;
				}
			}
			if(!eklendi) {
				printf("eklenmedi: %s\n", device_name);
				durum.storages[durum.usb_index] = (usb_storage){
					.disk_name = device_name,
					.devp_index = 1,
					.checked = false
				};
				strncpy(durum.storages[durum.usb_index].dev_paths[0], link_buffer, strlen(link_buffer));
				durum.usb_index += 1;
				durum.usb_index++;
			}
		} else {
			durum.storages[durum.usb_index] = (usb_storage){
				.disk_name = device_name,
				.devp_index = 1,
				.checked = false
			};
			strncpy(durum.storages[durum.usb_index].dev_paths[0], link_buffer, strlen(link_buffer));
			durum.usb_index += 1;
		}
		//printf("linked: %s\n", link_buffer);
	}
	pclose(fl);
}
