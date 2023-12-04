#include "kilitler.h"

void detect_new_usbs(void) {
	char buff[LS_BUFFER_CAP] = {0};
	
	FILE *fl = popen("ls /dev/disk/by-id/usb-* ", "r");
	if(fl == NULL) {
		printf("Popen failed\n");
		return;
	}
	while(fgets(buff, sizeof(buff), fl) != NULL) {
		buff[strlen(buff)-1] = 0;
		printf("%s\n", buff);
		char link_buffer[LS_BUFFER_CAP] = {0};
		ssize_t length = 0;
		if((length = readlink(buff, link_buffer, strlen(buff)-1)) == -1) {
			printf("Error at readlink: %s\n", strerror(errno));
			return;
		}
		link_buffer[length] = '\0';
		printf("linked: %s\n", link_buffer);
		
	}
	pclose(fl);
}
