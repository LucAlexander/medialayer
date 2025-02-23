#!/bin/bash

out=ah

function clean_env {
	rm -f $out.*
	rm -f $out
	rm -f *.c
	rm -f *.h
}

function debug_env {
	rm -rf bin/*
	clean_env
	cp demo/*.c .
	cp demo/*.h .
	cp src/* .
	gcc -o $out *.c -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -g -Wall
	mv $out bin/.
	cp -r demo/fnt bin/.
	cp -r demo/snd bin/.
	cp -r demo/spr bin/.
	clean_env
}

function compile_env {
	rm -rf bin/*
	clean_env
	cp demo/*.c .
	cp demo/*.h .
	cp src/* .
	gcc -o $out *.c -lSDL2main -lSDL2 -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lm -O3
	mv $out bin/.
	cp -r demo/fnt bin/.
	cp -r demo/snd bin/.
	cp -r demo/spr bin/.
	clean_env
}

function wasm_env {
	rm -rf bin/*
	clean_env
	cp demo/*.c .
	cp demo/*.h .
	cp src/* .
	emcc *.c --bind -s WASM=1 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["png"]' -s USE+SDL_TTF=2 -s USE_SDL_MIXER=2 -lm -O3 -o $out.html --preload-file fnt/ --preload-file spr/ --preload-file snd/ --use-preload-plugins --shell-file wasm_template.html
	echo "hostname -I;python3 -m http.server 7000" > testserver.sh
	chmod +x testserver.sh
	mv $out.* bin/
	cp -r demo/fnt bin/.
	cp -r demo/snd bin/.
	cp -r demo/spr bin/.
	clean_env
}

if [ $1 == "debug" ]; then
	debug_env
elif [ $1 == "compile" ]; then
	compile_env
elif [ $1 == "wasm" ]; then
	wasm_env
elif [ $1 == "clean" ]; then
	clean_env
fi
