app:
	mkdir -p dist
	emcc main.c \
		-o dist/index.js \
		-s USE_SDL=2 \
		-s USE_SDL_GFX=2 \
		-s EXPORTED_FUNCTIONS="['_reconfigure', '_main']"

	cp index.html dist/index.html

serve:
	emrun --port 3001 dist/index.html

