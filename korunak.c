#include <stdio.h>
#include <stdlib.h>
#include "raylib/include/raylib.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "kilitler.c"

#define SAAT_FONT "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf"
#define REL_FONT "/usr/share/fonts/truetype/liberation2/LiberationSans.ttf"
#define FONT_SIZE 72
#define DEBUG_UI_OFF
#define MARGIN_Y 60

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

const char *okul_kapali = "Okul kapalı";
const char *tenefuse_kaldi = "Tenefüsün bitmesine %d dakika kaldı";
const char *derse_kaldi = "Dersin başlamasına %d dakika kaldı";
const int okulun_baslmasi = 60*8 + 15;
const int okulun_bitmesi = 60*15 + 20;
const int okulun_bitmesi_cuma = 60*15 + 15;

void get_relative_time(struct tm tms, char* relative) {
	if(tms.tm_wday == 6 || tms.tm_wday == 0) { // Haftasonu
		strcpy(relative, okul_kapali);
		return;
	}
	int geceden_beri = tms.tm_hour * 60 + tms.tm_min;
	int okuldan_beri = geceden_beri - dersin_baslmasi;
	if(okuldan_beri < 0) {
		
	}
}

void ui(void) {
	InitWindow(1920, 1080, "Korunak kilit");
	int w = GetScreenWidth(), h = GetScreenHeight();
	SetTargetFPS(30);
	if(!IsWindowFullscreen())
		ToggleFullscreen();

		
	// UTF-8 yazı hazırlığı
	int codepointCount = 0;
	int *codepoints = LoadCodepoints(okul_kapali, &codepointCount);

	Font saat_font = LoadFontEx(SAAT_FONT, FONT_SIZE, 0, 0);
	Font rela_font = LoadFontEx(SAAT_FONT, FONT_SIZE, codepoints, codepointCount);
	
	free(codepoints);

	time_t prev_now = time(NULL);
	char time_buffer[64] = {0};
	struct tm *tm_struct = localtime(&prev_now);
	sprintf(time_buffer, "%02d/%02d/%02d %02d:%02d",
	tm_struct->tm_mday, tm_struct->tm_mon + 1, tm_struct->tm_year + 1900,
	tm_struct->tm_hour, tm_struct->tm_min);
	Vector2 time_vec = MeasureTextEx(saat_font, time_buffer, FONT_SIZE, 0);
	
	char relative_time_buffer[64] = {0};
	get_relative_time(*tm_struct, relative_time_buffer);
	Vector2 rel_vec = MeasureTextEx(rela_font, relative_time_buffer, FONT_SIZE, 0);

	
	
	while (!WindowShouldClose()) {
		time_t now = time(NULL);
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTextEx(saat_font, time_buffer,
			(Vector2){(w-time_vec.x)/2, (h-time_vec.y-MARGIN_Y)/2},
			FONT_SIZE, 0, BLACK);
			
			
		
		DrawTextEx(rela_font, relative_time_buffer,
			(Vector2){(w-rel_vec.x)/2, (h-rel_vec.y+(MARGIN_Y))/2},
			FONT_SIZE, 0, BLACK);
		
		EndDrawing();
		if(now - prev_now > 5) {
			struct tm *tm_struct = localtime(&now);
			sprintf(time_buffer, "%d:%d", tm_struct->tm_hour, tm_struct->tm_min);
			time_vec = MeasureTextEx(saat_font, time_buffer, FONT_SIZE, 0);
			
			get_relative_time(*tm_struct, relative_time_buffer);
			Vector2 rel_vec = MeasureTextEx(rela_font, relative_time_buffer, FONT_SIZE, 0);
			
			memset(&durum.storages, 0, sizeof(usb_storage)*MAX_USB);
			durum.usb_index = 0;
			init_kilit(&durum);
			dump_durum(durum);
			if(search_non_hashed(&durum))
				break;
			prev_now = now;
		}
#ifdef DEBUG_UI_OFF
		if(IsKeyReleased(KEY_Q))
			exit(0);
#endif
	}

	CloseWindow();
}

int main(void) {
	//dump_durum(durum);
	//printf("%s\n", search_non_hashed(&durum) ? "Geçer" : "Geçmez");
	bool quit = false;
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
