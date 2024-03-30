.PHONY: all
all: clean scribe-manual.pdf

scribe-manual.pdf: scribe-manual.adoc
	docker pull asciidoctor/docker-asciidoctor
	docker run -it -v .:/documents/ asciidoctor/docker-asciidoctor asciidoctor-pdf scribe-manual.adoc

clean:
	-rm scribe-manual.pdf
