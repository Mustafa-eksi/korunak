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
#include "kilitler.c"

kilit_state durum = {0};

int main(int argc, char *argv[])
{
	init_kilit(&durum);
	bcrypt_gensalt(12, durum.salt);
	
	char hash[BCRYPT_HASHSIZE] = {0};
	char password[BCRYPT_HASHSIZE] = {0};
	char password_hash[BCRYPT_HASHSIZE] = {0};
	printf("Salt: %s\n", durum.salt);
	time_t now = time(NULL);
	sprintf(password, "%ld", now);
	bcrypt_hashpw(password, durum.salt, password_hash);
	printf("argv: %s\n",password_hash);
	if(bcrypt_hashpw(password_hash, durum.salt, hash) != 0) {
		printf("ERROR: bcrypt_hashpw failed\n");
		return -1;
	}
	printf("Hashed password: %s\n", hash);
	return 0;
}

