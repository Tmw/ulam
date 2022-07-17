#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <emscripten.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STEPS 500
#define STEP_SIZE 25
#define NODE_RADIUS 8
#define NODE_COLOR {0xaa, 0xaa, 0xaa, 0xff}
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define WINDOW_MID_X WINDOW_WIDTH/2
#define WINDOW_MID_Y WINDOW_HEIGHT/2
#define BLACK_COLOR {0x00, 0x00, 0x00, 0xff}

SDL_Window* window;
SDL_Renderer* renderer;

typedef enum MoveDirection {
  DIR_NORTH,
  DIR_EAST,
  DIR_SOUTH,
  DIR_WEST,
} MoveDirection;

void clear_screen() {
  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
  SDL_RenderClear(renderer);
}

void aaFilledCircle(
    SDL_Renderer* renderer, 
    int posX, 
    int posY, 
    int radius,
    SDL_Color color
) {
    filledCircleRGBA(renderer, posX, posY, radius, color.r, color.g, color.b, color.a);
    aaellipseRGBA(renderer, posX, posY, radius + 1, radius, color.r, color.g, color.b, color.a);
    aacircleRGBA(renderer, posX, posY, radius, color.r, color.g, color.b, color.a);
}

SDL_Point calculate_next_point(SDL_Point current_point, MoveDirection dir) {
  SDL_Point new_point = current_point;

  switch (dir) {
    case DIR_NORTH:
      new_point.y -= STEP_SIZE;
      break;

    case DIR_EAST:
      new_point.x += STEP_SIZE;
      break;

    case DIR_SOUTH:
      new_point.y += STEP_SIZE;
      break;

    case DIR_WEST:
      new_point.x -= STEP_SIZE;
      break;
  }

  return new_point;
}

MoveDirection counter_clockwise_next_direction(MoveDirection current_direction) {
  switch (current_direction) {
    case DIR_NORTH:
      return DIR_WEST;

    case DIR_WEST:
      return DIR_SOUTH;

    case DIR_SOUTH:
      return DIR_EAST;

    case DIR_EAST:
      return DIR_NORTH;
  }
}

bool is_prime(int maybe_prime) {
  if(maybe_prime <= 1) {
    return false;
  }

  for(int i = 2; i < maybe_prime/2 + 1; i++) {
    if(maybe_prime % i == 0){
      return false;
    }
  }

  return true;
}

SDL_Point interpolate(SDL_Point point, MoveDirection direction, int progress) {
  switch (direction) {
    case DIR_EAST:
      point.x -= STEP_SIZE - progress;
      return point;

    case DIR_WEST:
      point.x += STEP_SIZE - progress;
      return point;

    case DIR_NORTH:
      point.y += STEP_SIZE - progress;
      return point;

    case DIR_SOUTH:
      point.y -= STEP_SIZE - progress;
      return point;
  }
}

int segment_progress = 0;
int draw_step = 2;
void draw_ulam_spiral() {
  clear_screen();

  SDL_Point last_point = {WINDOW_MID_X, WINDOW_MID_Y};
  MoveDirection direction = DIR_EAST;
  int steps_per_turn = 1;

  SDL_Color circle_color = NODE_COLOR;

  for (int i = 1; i <= draw_step; i++) {
    SDL_Point new_point = calculate_next_point(last_point, direction);

    if (i == draw_step && draw_step < MAX_STEPS) {
      segment_progress+=5;
      new_point = interpolate(new_point, direction, segment_progress);
    }

    if (segment_progress >= STEP_SIZE && draw_step < MAX_STEPS) {
      draw_step++;
      segment_progress = 0;
    }

    thickLineRGBA(renderer, last_point.x, last_point.y, new_point.x, new_point.y, 2, 0x00, 0x00, 0x00, 0xff);

    if (is_prime(i)) {
      aaFilledCircle(renderer, last_point.x, last_point.y, NODE_RADIUS, circle_color);
    }

    if (i % steps_per_turn == 0) {
      direction = counter_clockwise_next_direction(direction);
      if (direction == DIR_WEST || direction == DIR_EAST) {
        steps_per_turn++;
      }
    }

    last_point = new_point;
  }

  SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  emscripten_set_main_loop(draw_ulam_spiral, 0, 1);
}
