.PHONY: build up up-fg down logs

build:
	docker-compose build --no-cache

up:
	docker-compose up -d

up-fg:
	docker-compose up

down:
	docker-compose down

logs:
	docker-compose logs -f
