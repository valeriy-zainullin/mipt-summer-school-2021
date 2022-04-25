#pragma once

#include <stddef.h>
#include <stdint.h>

static const size_t SCREEN_COLS = 800;
static const size_t SCREEN_ROWS = 600;

struct rgba {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

struct screen_state {
	struct {
		float x;
		float y;
	} center;
	
	// Difference in coordinates if moving to an adjancent cell.
	float scale;
};

static const struct screen_state INITIAL_SCREEN_STATE = {{0, 0}, 0.015625}; // 2^(-6) (TODO: проверить!)

static const size_t MAX_NUM_ITERATIONS = 256;
static const float MAX_ALLOWED_MODULO_SQ = 100.0f;

// We are using powers of 2 here as it's bringing no precision losses
// while stepping and dividing and multiplying by 2 also brings no
// losses in precision, which helps us to save the center of the
// picture (it's coordinates x and y, not gather precision losses).
// Center of the picture is at col = SCREEN_COLS / 2 and
// row = SCREEN_ROWS / 2.
static const int SCALE_TIMES_STEP = 2;
static const int PIXEL_STEP = 32;

#if defined(__cplusplus)
extern "C" {
#endif

void compute_nosse(struct rgba * buffer, struct screen_state* screen_state);
void compute_sse(struct rgba * buffer, struct screen_state* screen_state);

#if defined(__cplusplus)
}
#endif