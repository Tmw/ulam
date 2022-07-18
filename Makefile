app:
	mkdir dist
	emcc main.c -o dist/index.js -s USE_SDL=2 -s USE_SDL_GFX=2
	cp index.html dist/index.html

serve:
	emrun --port 3001 dist/index.html

