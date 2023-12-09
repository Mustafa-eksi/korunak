#include <stdio.h>
#include <stdlib.h>
#include "raylib/include/raylib.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "kilitler.c"

#define DEBUG_UI_OFF

kilit_state durum = {0};

bool check_usbs(time_t *prev) {
	time_t now = time(NULL);
	if(now - (*prev) > 5) {
		memset(&durum.storages, 0, sizeof(usb_storage)*MAX_USB);
		durum.usb_index = 0;
		init_kilit(&durum);
		dump_durum(durum);
		if(search_non_hashed(&durum))
			return true;
		*prev = now;
	}
	return false;
}

void ui(void) {
	InitWindow(1920, 1080, "Korunak kilit");
	int w = GetScreenWidth(), h = GetScreenHeight();
	if(!IsWindowFullscreen())
		ToggleFullscreen();
	Font font = LoadFont("/usr/share/fonts/truetype/noto/NotoSansDisplay-Regular.ttf");
	time_t prev_now = time(NULL);
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTextEx(font, "WIP", (Vector2){w/2, h/2}, 32, 0, BLACK);
		EndDrawing();
		if(check_usbs(&prev_now))
			break;
	}

	CloseWindow();
}

int main(void) {
	//dump_durum(durum);
	//printf("%s\n", search_non_hashed(&durum) ? "Geçer" : "Geçmez");
	bool quit = false;
	time_t prev_now = time(NULL);
	sync_with_file(&durum);
	while(!quit) {
		memset(&durum.storages, 0, sizeof(usb_storage)*MAX_USB);
		durum.usb_index = 0;
		init_kilit(&durum);
		dump_durum(durum);
		if(!search_non_hashed(&durum))
			ui();
		sleep(4);
	}
	return 0;
}
