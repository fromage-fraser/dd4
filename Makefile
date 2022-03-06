.PHONY: build up up-fg down logs build-dev dev

build:
	docker-compose build --no-cache server

up:
	docker-compose up -d server

up-fg:
	docker-compose up server

down:
	docker-compose down

logs:
	docker-compose logs -f server

build-dev:
	docker-compose build --no-cache dev

dev:
	docker-compose run --rm --service-ports dev
