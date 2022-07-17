#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <emscripten.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_STEPS 180
#define STEP_SIZE 50

#define FONT_SIZE 30
#define NODE_SIZE 50
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800

#define WINDOW_MID_X WINDOW_WIDTH/2
#define WINDOW_MID_Y WINDOW_HEIGHT/2

#define FONT_COLOR {0xff, 0xff, 0xff}
#define BLACK_COLOR {0x00, 0x00, 0x00, 0xff}

SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;

typedef enum MoveDirection {
  DIR_NORTH,
  DIR_EAST,
  DIR_SOUTH,
  DIR_WEST,
} MoveDirection;

// TODO:
// - Let's try to link with SDL_TTF in emscripten and get some text rendering done,
//    then we can start on working on our Ulam spiral and perhaps get this project finished one day :D 
//
//
// Make it have options:
// - Hide non-primes vs. 
// - Only render primes
// - Render primes differently
// - no difference
//
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


/*
 * FONT RENDERING
    SDL_Color color = FONT_COLOR;
    SDL_Surface* surface = TTF_RenderText_Blended(font, "0", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    // get the width / height of the texture
    int iw, ih;
    SDL_QueryTexture(texture, NULL, NULL, &iw, &ih);

    SDL_Rect font_rect = {n->x + 18, n->y + 3, iw, ih};
    SDL_SetRenderDrawColor(renderer, 0xff, 0xcc, 0x00, 0xff);
    SDL_RenderCopy(renderer, texture, NULL, &font_rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    */

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

void draw_ulam_spiral() {
  clear_screen();

  SDL_Point last_point = {WINDOW_MID_X, WINDOW_MID_Y};
  MoveDirection direction = DIR_EAST;
  int steps_per_turn = 1;

  SDL_Color circle_color = {0x00, 0x0, 0x0, 0xff};

  for (int i = 1; i < MAX_STEPS; i++) {
    aaFilledCircle(renderer, last_point.x, last_point.y, 10, circle_color);
    SDL_Point new_point = calculate_next_point(last_point, direction);
    thickLineRGBA(renderer, last_point.x, last_point.y, new_point.x, new_point.y, 2, 0x00, 0x00, 0x00, 0xff);

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

  if(TTF_Init() != 0) {
    printf("[ERROR] Unable to initialize SDL_TTF\n");
    return 1;
  }

  // load the font
  font = TTF_OpenFont("assets/font.ttf", FONT_SIZE);
  if (font == NULL) {
    printf("[ERROR] unable to load font?\n");
  }

  SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

  //emscripten_set_main_loop(draw_ulam_spiral, 0, 1);
  draw_ulam_spiral();

  TTF_CloseFont(font);
}
