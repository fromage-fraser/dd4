.PHONY: all
all: clean scribe-manual.pdf

scribe-manual.pdf: docker scribe-manual.adoc
	docker run -it -v .:/documents/ asciidoctor/docker-asciidoctor asciidoctor-pdf scribe-manual.adoc

clean:
	-rm scribe-manual.pdf

.PHONY: docker
docker:
	docker pull asciidoctor/docker-asciidoctor
