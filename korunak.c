#include <stdio.h>
#include <stdlib.h>
#include "raylib/include/raylib.h"
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include "kilitler.c"
#include <math.h>

#define SAAT_FONT "/usr/share/fonts/truetype/liberation2/LiberationSans-Bold.ttf"
#define REL_FONT "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf"
#define SAAT_FONT_SIZE 72
#define REL_FONT_SIZE 48
//#define DEBUG_UI_OFF
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

const char  *okul_kapali = "Okul kapalı",
	    *tenefuse_kaldi = "Tenefüsün bitmesine %d dakika kaldı",
	    *ogle_arasinin_bitmesine = "Öğle arasının bitmesine %d dakika kaldı",
	    *derse_kaldi = "Dersin bitmesine %d dakika kaldı",
	    *dersin_kaldi = "Dersin başlamasına %d dakika kaldı",
	    *okulun_bitmesine = "Okulun bitmesine %d dakika kaldı";
	    
const char* tum_karakterler = "1234567890qwertyuıopğüasdfghjklşizxcvbnmöçQWERTYUIOPĞÜASDFGHJKLŞİZXCVBNMÖÇ";


const int ders_suresi = 40, tenefus_suresi = 10,
	  tenefus_suresi_cuma = 5,
	  okulun_baslmasi = 60*8 + 15,
	  okulun_bitmesi = 60*15 + 20,
	  okulun_bitmesi_cuma = 60*15 + 15,
	  ogle_arasinin_baslamasi = (ders_suresi+tenefus_suresi)*6,
	  ogle_arasinin_baslamasi_cuma = 50+(45*5),
	  ogle_arasi = 45,
	  ogle_arasi_cuma = 60;

void get_relative_time(struct tm tms, char* relative) {
	if(tms.tm_wday == 6 || tms.tm_wday == 0) { // Haftasonu
		strcpy(relative, okul_kapali);
		return;
	}
	//char buffer[64] = {0};
	int geceden_beri = tms.tm_hour * 60 + tms.tm_min;
	int okuldan_beri = geceden_beri - okulun_baslmasi;
	if(okuldan_beri < 0) {
		sprintf(relative, dersin_kaldi, -okuldan_beri);
		return;
	}
	if(tms.tm_wday == 5) { // Cuma
		if(geceden_beri - okulun_bitmesi_cuma > 0) {
			strcpy(relative, okul_kapali);
			return;
		}
		if(okuldan_beri < ogle_arasinin_baslamasi_cuma) { // Öğle arasından önce
			if(okuldan_beri < 50) {
				int kalan = okuldan_beri % 50;
				if(kalan < 40) { // dersin bitmesine
					sprintf(relative, derse_kaldi, ders_suresi-kalan);
					return;
				}else { // Tenefüsün bitmesine
					sprintf(relative, tenefuse_kaldi, tenefus_suresi-(kalan-ders_suresi));
					return;
				}
			}else {
				int kalan = (okuldan_beri-50) % (ders_suresi+tenefus_suresi_cuma);
				if(kalan < 40) { // dersin bitmesine
					sprintf(relative, derse_kaldi, ders_suresi-kalan);
					return;
				}else { // Tenefüsün bitmesine
					sprintf(relative, tenefuse_kaldi, tenefus_suresi_cuma-(kalan-ders_suresi));
					return;
				}
			}
		}
		int ogleden_beri = okuldan_beri - ogle_arasinin_baslamasi_cuma;
		if(ogleden_beri < ogle_arasi_cuma) { // Hala öğle arası
			sprintf(relative, ogle_arasinin_bitmesine, ogle_arasi_cuma - ogleden_beri);
			return;
		}
		int ogleden_sonra = ogleden_beri - ogle_arasi_cuma;
		if(ogleden_sonra < 40) { // 7. ders içinde
			sprintf(relative, derse_kaldi, ders_suresi - ogleden_sonra);
			return;
		}else if(ogleden_sonra < ders_suresi+tenefus_suresi) { // Son tenefüs içinde
			sprintf(relative, tenefuse_kaldi, tenefus_suresi-(ogleden_sonra-ders_suresi));
			return;
		}else { // Okul bitişi
			sprintf(relative, okulun_bitmesine, ders_suresi-(ogleden_sonra-(ders_suresi+tenefus_suresi)));
			return;
		}
		return;
	}else { // Diğer günler
		if(geceden_beri - okulun_bitmesi > 0) {
			strcpy(relative, okul_kapali);
			return;
		}
		//int ders = ceil(okuldan_beri / 50.0);
		if(okuldan_beri < ogle_arasinin_baslamasi) { // Öğle arasından önce
			int kalan = okuldan_beri % 50;
			if(kalan < 40) { // dersin bitmesine
				sprintf(relative, derse_kaldi, ders_suresi-kalan);
				return;
			}else { // Tenefüsün bitmesine
				sprintf(relative, tenefuse_kaldi, tenefus_suresi-(kalan-ders_suresi));
				return;
			}
		}
		int ogleden_beri = okuldan_beri - ogle_arasinin_baslamasi;
		if(ogleden_beri < ogle_arasi) { // Hala öğle arası
			sprintf(relative, ogle_arasinin_bitmesine, ogle_arasi - ogleden_beri);
			return;
		}
		int ogleden_sonra = ogleden_beri - ogle_arasi;
		if(ogleden_sonra < 40) { // 7. ders içinde
			sprintf(relative, derse_kaldi, ders_suresi - ogleden_sonra);
			return;
		}else if(ogleden_sonra < 50) { // Son tenefüs içinde
			sprintf(relative, tenefuse_kaldi, tenefus_suresi-(ogleden_sonra-ders_suresi));
			return;
		}else { // Okul bitişi
			sprintf(relative, okulun_bitmesine, ders_suresi-(ogleden_sonra-(ders_suresi+tenefus_suresi)));
			return;
		}
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
	int *codepoints = LoadCodepoints(tum_karakterler, &codepointCount);

	Font saat_font = LoadFontEx(SAAT_FONT, SAAT_FONT_SIZE, 0, 0);
	Font rela_font = LoadFontEx(REL_FONT, REL_FONT_SIZE, codepoints, codepointCount);
	
	free(codepoints);

	time_t prev_now = time(NULL);
	char time_buffer[64] = {0};
	struct tm *tm_struct = localtime(&prev_now);
	sprintf(time_buffer, "%02d/%02d/%02d %02d:%02d",
	tm_struct->tm_mday, tm_struct->tm_mon + 1, tm_struct->tm_year + 1900,
	tm_struct->tm_hour, tm_struct->tm_min);
	Vector2 time_vec = MeasureTextEx(saat_font, time_buffer, SAAT_FONT_SIZE, 0);
	
	char relative_time_buffer[64] = {0};
	get_relative_time(*tm_struct, relative_time_buffer);
	Vector2 rel_vec = MeasureTextEx(rela_font, relative_time_buffer, REL_FONT_SIZE, 0);
	
	while (!WindowShouldClose()) {
		time_t now = time(NULL);
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTextEx(saat_font, time_buffer,
			(Vector2){(w-time_vec.x)/2, (h-time_vec.y-MARGIN_Y)/2},
			SAAT_FONT_SIZE, 0, BLACK);
		
		DrawTextEx(rela_font, relative_time_buffer,
			(Vector2){(w-rel_vec.x)/2, (h-rel_vec.y+(MARGIN_Y))/2},
			REL_FONT_SIZE, 0, BLACK);
		
		EndDrawing();
		if(now - prev_now > 5) {
			struct tm *tm_struct = localtime(&now);
			sprintf(time_buffer, "%02d/%02d/%02d %02d:%02d",
			tm_struct->tm_mday, tm_struct->tm_mon + 1, tm_struct->tm_year + 1900,
			tm_struct->tm_hour, tm_struct->tm_min);
			time_vec = MeasureTextEx(saat_font, time_buffer, SAAT_FONT_SIZE, 0);
			
			get_relative_time(*tm_struct, relative_time_buffer);
			rel_vec = MeasureTextEx(rela_font, relative_time_buffer, REL_FONT_SIZE, 0);
			
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
