compile:
	gcc src/*.c -o build/qlang -std=c17

run:
	build/qlang test/input.q test/out --debugNodes