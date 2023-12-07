#include <stdio.h>
#include <stdlib.h>
#include "raylib/include/raylib.h"
#include <stdbool.h>
#include <time.h>
#include "kilitler.c"

kilit_state durum = {0};

int main(void) {
	init_kilit(&durum);
	dump_durum(durum);
	printf("%s\n", search_non_hashed(&durum) ? "Geçer" : "Geçmez");
	return 0;
	InitWindow(1920, 1080, "Korunak kilit");
	int w = GetScreenWidth(), h = GetScreenHeight();
	ToggleFullscreen();
	Font font = LoadFont("/usr/share/fonts/truetype/noto/NotoSansDisplay-Regular.ttf");
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTextEx(font, "WIP", (Vector2){w/2, h/2}, 32, 0, BLACK);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
