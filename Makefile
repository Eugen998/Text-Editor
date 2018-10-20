build: editor
	gcc -Wall editor.c -o editor
run: editor
	./editor
clean: editor
	rm -rf editor