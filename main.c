#include <SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STEPS 800
#define STEP_SIZE 25
#define NODE_SIZE 16
#define DRAW_SPEED 5
#define NODE_COLOR {0xaa, 0xaa, 0xaa, 0xff}

SDL_Window* window;
SDL_Renderer* renderer;
int screen_width, screen_height, screen_mid_x, screen_mid_y;

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

void draw_node(
    SDL_Renderer* renderer, 
    int posX, 
    int posY, 
    int size
) {

  SDL_Color color = NODE_COLOR;

  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_Rect rect = {posX - size/2, posY - size/2, size, size};
  SDL_RenderFillRect(renderer, &rect);
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

void setup_sizing() {
  double width, height;
  emscripten_get_element_css_size("canvas", &width, &height);

  screen_width = width;
  screen_height = height;
  screen_mid_x = screen_width / 2;
  screen_mid_y = screen_height / 2;
}

int segment_progress = 0;
int draw_step = 2;
void draw_ulam_spiral() {

  SDL_Point last_point = {screen_mid_x, screen_mid_y};
  MoveDirection direction = DIR_EAST;
  int steps_per_turn = 1;


  for (int i = 1; i <= draw_step; i++) {
    SDL_Point new_point = calculate_next_point(last_point, direction);

    if (i == draw_step && draw_step < MAX_STEPS) {
      segment_progress+= DRAW_SPEED;
      new_point = interpolate(new_point, direction, segment_progress);
    }

    if (segment_progress >= STEP_SIZE && draw_step < MAX_STEPS) {
      draw_step++;
      segment_progress = 0;
    }

    thickLineRGBA(
      renderer, 
      last_point.x, 
      last_point.y, 
      new_point.x, 
      new_point.y, 
      2, 
      0x00, 
      0x00, 
      0x00, 
      0xff
    );

    if (is_prime(i)) {
      draw_node(
        renderer, 
        last_point.x, 
        last_point.y, 
        NODE_SIZE
      );
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

void main_loop() {
  clear_screen();

  // handle events (only resizing for now)
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_WINDOWEVENT:
        if(event.window.event == SDL_WINDOWEVENT_RESIZED) {
          setup_sizing();
        }
        break;

      default:
        break;
    }
  }

  draw_ulam_spiral();
}

int main(int argc, char* argv[]) {
  setup_sizing();

  SDL_Init(SDL_INIT_VIDEO);
  SDL_CreateWindowAndRenderer(screen_width, screen_height, SDL_WINDOW_RESIZABLE, &window, &renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  emscripten_set_main_loop(main_loop, 0, 1);
}
