/*
 * create_password.c
 * 
 * Copyright 2023 Mustafa Ekşi <mustafa@excalibur-debian>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "kilitler.c"

kilit_state durum = {0};

void salter(char sifre[BCRYPT_HASHSIZE], char hash[BCRYPT_HASHSIZE]) {
	char salt[BCRYPT_HASHSIZE] = {0};
	bcrypt_gensalt(12, salt);
	
	char password[BCRYPT_HASHSIZE] = {0};
	time_t now = time(NULL);
	sprintf(password, "%ld", now);
	bcrypt_hashpw(password, salt, sifre);
	if(bcrypt_hashpw(sifre, salt, hash) != 0) {
		printf("ERROR: bcrypt_hashpw failed\n");
	}
}

void add(char sifre[BCRYPT_HASHSIZE], char hash[BCRYPT_HASHSIZE], size_t usb_index) {
	int dosya = open(HASHED_PATH, O_WRONLY | O_APPEND);
	if(dosya == -1) {
		printf("Couldn't open "HASHED_PATH"\n");
		exit(-1);
	}
	
	char line[2048] = {0};
	for(size_t j = 0; j < durum.storages[usb_index].devp_index; j++) {
		if(!durum.storages[usb_index].mount_devs[j].is_mounted)
			continue;
		char path[1050] = {0};
		sprintf(path, "%s/"PASS_FILE_NAME,
			durum.storages[usb_index].mount_devs[j].mount_point);
		int sifre_dosya = open(path, O_CREAT | O_WRONLY);
		if(sifre_dosya == -1) {
			printf("Couldn't open "PASS_FILE_NAME"\n");
			break;
		}
		char sifre_buff[PATH_CAP] = {0};
		sprintf(sifre_buff,"%s\n", sifre);
		write(sifre_dosya, sifre_buff, strlen(sifre_buff));
		close(sifre_dosya);
		
		sprintf(line, "%s %s\n", durum.storages[usb_index].uuid, hash);
		printf("line: %s\n", line);
		write(dosya, line, strlen(line));
		
		break;
	}
	
	close(dosya);
}

int main(int argc, char **argv) {
	if(argc < 2) {
		printf("Error: no arguments given\n");
		return -1;
	}
	init_kilit(&durum);
	if(strcmp(argv[1], "list") == 0) {
		dump_durum(durum);
	}else if(strcmp(argv[1], "add") == 0) {
		if(argc < 3) {
			printf("Error: no arguments given 1-n\n");
			return -1;
		}
		int index = atoi(argv[2]);
		if(index == 0) {
			printf("0 is not accepted, use 1 based indexing.\n");
			return -1;
		}
		index--;
		char password[BCRYPT_HASHSIZE] = {0};
		char hashed_password[BCRYPT_HASHSIZE] = {0};
		salter(password, hashed_password);
		add(password, hashed_password, index);
		printf("Added successfully\n");
	}else {
		printf("COMMAND UNKNOWN: '%s'\n", argv[1]);
	}
	return 0;
}
