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
#include <gtk/gtk.h>
#include "kilitler.c"

kilit_state durum = {0};

char* salter() {
	char salt[BCRYPT_HASHSIZE] = {0};
	init_kilit(&durum);
	bcrypt_gensalt(12, salt);
	
	char hash[BCRYPT_HASHSIZE] = {0};
	char password[BCRYPT_HASHSIZE] = {0};
	char password_hash[BCRYPT_HASHSIZE] = {0};
	printf("Salt: %s\n", salt);
	time_t now = time(NULL);
	sprintf(password, "%ld", now);
	bcrypt_hashpw(password, salt, password_hash);
	printf("argv: %s\n",password_hash);
	if(bcrypt_hashpw(password_hash, salt, hash) != 0) {
		printf("ERROR: bcrypt_hashpw failed\n");
		return -1;
	}
	printf("Hashed password: %s\n", hash);
}

static void
quit_activated(GSimpleAction *action, GVariant *parameter, GApplication *application) {
  g_application_quit (application);
}

static void
app_activate (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);
  GtkWidget *win = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (win), "Şifre oluşturucu");
  gtk_window_set_default_size (GTK_WINDOW (win), 600, 800);

  gtk_window_present (GTK_WINDOW (win));
}

static void
app_startup (GApplication *application) {
  GtkApplication *app = GTK_APPLICATION (application);

  GSimpleAction *act_quit = g_simple_action_new ("quit", NULL);
  g_action_map_add_action (G_ACTION_MAP (app), G_ACTION (act_quit));
  g_signal_connect (act_quit, "activate", G_CALLBACK (quit_activated), application);

  
}

#define APPLICATION_ID "me.okul.korunak.mustafa"


int main(int argc, char *argv[])
{
	
	GtkApplication *app;
	int stat;

	app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
	g_signal_connect (app, "startup", G_CALLBACK (app_startup), NULL);
	g_signal_connect (app, "activate", G_CALLBACK (app_activate), NULL);

	stat =g_application_run (G_APPLICATION (app), argc, argv);
	g_object_unref (app);
	return stat;
}

