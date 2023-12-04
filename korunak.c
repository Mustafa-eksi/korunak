#include <stdio.h>
#include <stdlib.h>
#include "raylib/include/raylib.h"
#include <stdbool.h>
#include <time.h>
#include "kilitler.c"

typedef struct KilitState {
	bool kilitli;
	time_t kilitlenme_vakti;
} KilitState;

KilitState state = {
	.kilitli = true,
	.kilitlenme_vakti = 0
};

int main(void) {
	detect_new_usbs();
	return 0;
	InitWindow(1920, 1080, "Korunak kilit");
	int w = GetScreenWidth(), h = GetScreenHeight();
	ToggleFullscreen();
	Font font = LoadFont("/usr/share/fonts/truetype/noto/NotoSansDisplay-Regular.ttf");
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawTextEx(font, state.kilitli ? "Kilitli" : "Açık", (Vector2){w/2, h/2}, 32, 0, BLACK);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
