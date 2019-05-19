#!/bin/bash

set -e

DATE=`date '+%Y%m%d%H%M'`
ROOT_DIR=/home/dd4/server
ARCHIVE=${ROOT_DIR}/backup/players/player_$DATE.tar.bz2

tar cjvf ${ARCHIVE} -C ${ROOT_DIR} player | egrep '^player/[a-z]/[a-zA-Z]'

echo "Player directory archived as ${ARCHIVE}"

