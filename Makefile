.PHONY: build up up-fg down logs dev

build:
	docker-compose -f server-docker-compose.yml build --no-cache

up:
	docker-compose -f server-docker-compose.yml up -d

up-fg:
	docker-compose -f server-docker-compose.yml up

down:
	docker-compose -f server-docker-compose.yml down

logs:
	docker-compose -f server-docker-compose.yml logs -f

dev:
	docker-compose -f dev-docker-compose.yml run --rm --service-ports dev
