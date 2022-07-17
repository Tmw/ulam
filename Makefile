app:
	emcc main.c -o index.js -s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_SDL_GFX=2 --preload-file assets

serve:
	emrun --port 3001 ./index.html

