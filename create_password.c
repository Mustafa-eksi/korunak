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
#include "kilitler.c"

kilit_state durum = {0};

int main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("Error: No password spesified.\n");
		return -1;
	}
	//sync_with_file(&durum);
	bcrypt_gensalt(12, durum.salt);
	printf("argv: %s\n",argv[1]);
	char hash[BCRYPT_HASHSIZE] = {0};
	printf("Salt: %s\n", durum.salt);
	if(bcrypt_hashpw(argv[1], durum.salt, hash) != 0) {
		printf("ERROR: bcrypt_hashpw failed\n");
		return -1;
	}
	printf("Hashed password: %s\n", hash);
	return 0;
}

