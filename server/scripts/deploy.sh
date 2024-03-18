#!/bin/bash
# Written by Owl 6/3/22
# Run as root with sudo or sudo -i

# Below for maximum portability. Used so you can work from a local repo pull and
# move only essential files to a server directory (avoid copying over pfiles and
# tables when you update your local, etc).

CP_PATH=/bin/cp
# Change -v below to nothing (blank) to reduce output spam
CP_VERBOSE=-v
CHOWN_PATH=/bin/chown
CHMOD_PATH=/bin/chmod
ECHO_PATH=/bin/echo
SERVER_USER=ddhost
LOCAL_AREA_FILE_DIR=/home/ddsrc/dd4/server/area/
SERVER_AREA_FILE_DIR=/home/ddhost/dd4/area/
LOCAL_MOBPROG_FILE_DIR=/home/ddsrc/dd4/server/area/MOBProgs/
SERVER_MOBPROG_FILE_DIR=/home/ddhost/dd4/area/MOBProgs/
LOCAL_DOC_DIR=/home/ddsrc/dd4/server/doc/
SERVER_DOC_DIR=/home/ddhost/dd4/doc/
LOCAL_STARTUP_SCRIPT=/home/ddsrc/dd4/server/scripts/start_dd4
SERVER_STARTUP_SCRIPT=/home/ddhost/dd4/scripts/start_dd4
LOCAL_EXECUTABLE=/home/ddsrc/dd4/server/src/dd4
SERVER_EXECUTABLE=/home/ddhost/dd4/src/dd4.new
LOCAL_WEBSITE_BUILDER_DIR=/var/www/smihilist.com/dd4/web/main/building/
SERVER_BUILDER_DIR=/home/ddsrc/dd4/builder/
LOCAL_MERCH_FILE=/home/ddsrc/dd4/server/src/merc.h


if [[ $EUID -ne 0 ]]; then
   $ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mThis script must be run as root, exiting without doing anything...\e[0m\r\n"
   exit 1
fi

# Copy all areas to server directory
$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mCopying all area files to ${SERVER_AREA_FILE_DIR}...\e[0m\r\n"
$CP_PATH $CP_VERBOSE -R ${LOCAL_AREA_FILE_DIR}* $SERVER_AREA_FILE_DIR
$CHOWN_PATH -R ${SERVER_USER}:${SERVER_USER} $SERVER_AREA_FILE_DIR

# Copy all docs to server directory

$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mCopying docs to $SERVER_DOC_DIR...\e[0m\r\n"
$CP_PATH $CP_VERBOSE -R ${LOCAL_DOC_DIR}* ${SERVER_DOC_DIR}
$CHOWN_PATH ${SERVER_USER}:${SERVER_USER} ${SERVER_DOC_DIR}*

# Copy all builder information to the website's "building" directory

$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mCopying builder stuff to $LOCAL_WEBSITE_BUILDER_DIR...\e[0m\r\n"
$CP_PATH $CP_VERBOSE -R ${SERVER_BUILDER_DIR}* ${LOCAL_WEBSITE_BUILDER_DIR}
$CP_PATH $CP_VERBOSE ${LOCAL_MERCH_FILE} ${LOCAL_WEBSITE_BUILDER_DIR}
$CHOWN_PATH ${SERVER_USER}:${SERVER_USER} ${LOCAL_WEBSITE_BUILDER_DIR}*

# Copy startup script to server directory

$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mCopying startup script to $SERVER_STARTUP_SCRIPT...\e[0m\r\n"
$CP_PATH $CP_VERBOSE $LOCAL_STARTUP_SCRIPT $SERVER_STARTUP_SCRIPT
$CHOWN_PATH ${SERVER_USER}:${SERVER_USER} $SERVER_STARTUP_SCRIPT

# Copy envy executable to server directory

$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mCopying executable to $SERVER_EXECUTABLE...\e[0m\r\n"
$CP_PATH $CP_VERBOSE $LOCAL_EXECUTABLE $SERVER_EXECUTABLE
$CHOWN_PATH ${SERVER_USER}:${SERVER_USER} $SERVER_EXECUTABLE
$ECHO_PATH -e "\r\n\e[38;5;0;48;5;196mMaking $SERVER_EXECUTABLE executable...\e[0m\r\n"
$CHMOD_PATH +x $SERVER_EXECUTABLE

$ECHO_PATH -e "\r\n\e[38;5;0;48;5;154mDone!  Please login to $SERVER_USER account and restart MUD with $SERVER_STARTUP_SCRIPT & \r\n(after killing old process) to see changes.\e[0m\r\n"


