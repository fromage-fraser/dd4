# Dragons Domain IV MUD

The _Dragons Domain IV_ (or _DD4_) MUD is based on the Envy MUD codebase.
It has been significantly customised, with many original game mechanics and areas.

🐉 &ensp; _DD4_ is currently hosted in New Zealand and can be accessed at **dragons-domain.org** on port **8888**.

🐲 &ensp; Visit [www.dragons-domain.org](https://www.dragons-domain.org/) for more information!

The codebase is ancient, but can be built and run fairly easily with Docker (see below).

_Owl, Shade, Gezhp_


    O---=<  Dragons Domain IV   >=---------------- __----^^^^^^^^^^^------__ ---O
    |          .              .         _//^    ^//====......          __--^^^  |
    |    )              -_       _..--+^o`\     ||_   ^^^^^^::::... /^    .     |
    | .              ___-==_     `--=;_`_  \   ||  -_             _/^^-         |
    |   ]=I==II==I=[^.==^|-_=_     ^-^ _/^ |-   _||   -_        _/]I-I-II-I-I[  |
    |   _((__||__))=^    |  -_-_       /  /-   / ||     -_     /   \_`   '__/   |
    |  =  |     +|       |    -_-_    /  /-   /   ||      -_  /     |     -|    |
    | /  _|__   #|       |      - ^-_/  /|- _/   .||        -/      |   '  |    |
    | |^^ |     +|^^^^--_|_     ^==-/   | \--===^^        ./        |-_ [],|  . |
    |     |- . '*|       ^|       /|    |-^\^       __--^^   .     _|______|_   |
    |     |     +|   +    |^^^^-_/ |  ]--I-[-]-I--[   .         /\<=-=-==-=-=>  |
    |  ___| _ '_*|___         __/ _-_ (-(-|---|-)-)              \ \__   _'_/   |
    |  I_I|_I__I_|I_I _   _  ((->/_   _|        ||            .   ^^|`    ||    |
    |  \-\--|--|--/-/' `-' `-' `-' `-' | []     #|------_-_-_-_-_-_-|_'   #|    |
    |   |     '     |=-=-=-=-=-=-=-=-=-|      []*|-----________- -`-- ]   *|    |
    |  .|  ` ` []   |    | | | | |     |-       #|      ,    ' ------['  _#|    |
    |.: | '  `  '   | [] | | | | |  []`|  []    *|   `    []    . `  |'  I*|.: .|
    |   /_'_v_.___  |.__,|_|_|_|_|     |   `  , *|          _.___,__/v_.._.|  ' |
    |   .        __----    _---^^^^^--/_._v_Y_,_v))-----^^^^       .        '   |
    |     :   ---          --        `                       `          `       |
    O---<   Welcome Traveller, to the   D r a g o n s   D o m a i n   IV    >---O
    | DikuMud created by H.Starfeldt, K.Nyboe, T.Madsen, M.Seifert, & S.Hammer. |
    | Based on the original Merc 2.2 source written by Kahn, Hatchet and Furey. |
    | DD III code by Shade, Gezhp, Owl, Brutus, Jyhad, Tavolir, and Danath.     |
    | DD IV code by Shade, Owl, Gezhp and the rest of the team.                 |
    O-----< MobCode from Merc 2.2 >-----< Quest Code (c) 1996 Ryan Addams >-----O


## Quick start

1. Install `docker`.
2. Install `docker-compose`.
3. Recommended: install `make`.

4. From the root directory, initialise the player directory:

    ```
    cp -r server/player_skeleton server/player
    ```

    This only has to be done once.

5. From the root directory, build the server image:

    ```
    // With make
    make build

    // With docker-compose directly
    docker-compose build --no-cache server
    ```

   This will build a `dd4-server` docker image.
   Repeating this command will rebuild the server.

6. From the root directory, start the server:

    ```
    // With make
    make up

    // With docker-compose directly
    docker-compose up -d server
    ```

    The MUD server will start up on port 8888.

7. To view the log of the running server:

    ```
    // With make
    make logs

    // With docker-compose directly
    docker-compose logs -f server
    ```

8. To stop the server:

    ```
    // With make
    make down

    // With docker-compose directly
    docker-compose down
    ```

The server container is not configured to always restart: edit [the docker-compose configuration](docker-compose.yml)
if you don't want this behaviour.


## Files

| Directory                 | Contents                           | Notes                                     |
|---------------------------|------------------------------------|-------------------------------------------|
| `server/area/`            | Area files                         | *                                         |
| `server/player/`          | Player files ("pfiles") and tables | The directory actively used by the server |
| `server/player_skeleton/` | Initial player and table data      | Initial player data: see below            |
| `server/src/`             | Source code                        |                                           |
| `faq/`                    | Frequently Asked Questions         | See below                                 |
| `maps/`                   | Maps generated from area files     | See below                                 |
| `builder/`                | Docs and tools for area building   |                                           |

`*` These directories are both mounted when using the docker-compose file to run the MUD.
    Areas can be modified and characters saved.


### The _server/player/_ and _server/player_skeleton/_ directories

The MUD server expects a _server/player/_ directory with the correct structure to exist when it starts up.
There are subdirectories for each letter of the alphabet, plus a _TABLES/_ directory that stores the current values
of various tables shown in the game.

As files in this location often change, we do not keep them under version control. The _server/player_skeleton/_ is
provided as a starting point for new installations or checkouts.

Please create a copy of the _server/player_skeleton/_ directory as _server/player/_ before running the server for the
first time. This only has to happen once.

    cp -r server/player_skeleton server/player

> **Note** The server will start if the player directory is missing, however it will crash when later attempting to
> save player and table information.


## FAQ

An HTML FAQ for new players was hosted on the MUD's original website.
The rendered FAQ pages can be found in the `faq` directory: [view the FAQ index](faq/dd-faq.html).

There was a script and source files to generate the FAQ pages, but I no longer have them.

The FAQ is a good place to start if you have no idea about how to play, or you aren't familiar with the DD4 MUD.


## Maps

Maps of the DD4 world can be found in the `maps` directory.


#### Area maps

Individual area maps can be found in `maps/areas` directory.

Please note that the code used to generate these maps is far from perfect.
There are currently some very strange layouts!

(Generating these maps is a fairly interesting problem, as the world seems like a grid, but it is not really.)

*Spoiler alert*: area maps contain information about healers, teachers, shops, no-recall and no-mobile rooms, and show
all exits (apart from portals). However, the following are not shown: whether doors are locked or bash/pick/pass-proof;
the location of any objects; the location of any mobiles (beyond healers, teachers and shopkeepers).

| File                                    | Notes                                                               |
|-----------------------------------------|---------------------------------------------------------------------|
| [Index](maps/area/index.html)           | Index of all maps                                                   |
| [Midgaard](maps/area/midgaard.html)     | Starting city                                                       |
| [Haon Dor](maps/area/haon.html)         | Wilderness west of Midgaard (many low-level areas nearby)           |
| [Miden'nir](maps/area/midennir.html)    | Wilderness east and south of Midgaard (many low-level areas nearby) |
| [Town Of Solace](maps/area/solace.html) | Low-level city with many areas nearby                               |
| [The City of Anon](maps/area/tcwn.html) | Key mid-level city                                                  |

## Immortals

Three original immortals are provided. Log in as one of the following; passwords are blank.

    Gezhp
    Owl
    Shade

If you want to create your own immortal:

1. Create a new character.
2. Reach level 2 (complete the training area).
3. Save your character: `save`.
4. Quit the game: `quit`.
5. Edit the new character's pfile. It will be the file `server/player/<initial letter>/<name>`.
    Make the following changes:

    Set the characters level to 106 by altering the value of the existing _Lvl_ key:
    ```
    Lvl         106
    ```
    Add the _sset_ (_skill set_) skill under the existing set of skills:
    ```
    Skll        100 'sset'
    ```
6. Log back into the game. The character is now level 106, the highest rank of immortal in the game.
7. Set all skills to 100%. This grants access to every mortal skill and all immortal ("wiz") commands.
    ```
    sset <name> all 100
    ```
8. Save your character.
9. The `wizhelp` command should work if these steps were completed successfully.


## Development

A development shell is provided to more quickly build, run and debug the server in environments that might have
trouble compiling the code.
The shell environment includes versions of gcc and gdb that will support the codebase.
Using the shell can speed up development, as files can be quickly changed, compiled and debugged without constantly
rebuilding the main server image.

To run the shell:

    // With make
    make dev

    // With docker-compose directly
    docker-compose run --rm --service-ports dev

This command will mount the MUD server files under `/dd4-dev` in a `dd4-dev` container.

Port 8888 within the container will be mapped to host port 18888.

Note that the working directory for the running MUD server is the `area` directory, so execution must occur from there.


### Builder tools

The dev image has the contents of the `builder` directory under `/dd4-builder`.
The area building tool _Scribe_ is available as the command `scribe.pl`.

    root@container:/dd4-dev# scribe.pl
    DD4 Area File Scribe version 0.5
    Usage: scribe.pl <source file> <destination file>


### Building and running the server in the dev shell

Start the container:

    user@host$ make dev

Compile the server:

    root@container:/dd4-dev# cd src

    // Or `make -j` for parallel processing
    root@container:/dd4-dev/src# make

Run the server:

    root@container:/dd4-dev/src# cd ../area

    root@container:/dd4-dev/area# ../src/dd4

Debug the server:

    root@container:/dd4-dev/area# gdb ../src/dd4

    (gdb) run

Clean rebuild of the server:

    root@container:/dd4-dev/src# make clean

    root@container:/dd4-dev/src# make

Rebuilding the dev container (if you want to update installed programs or config):

    user@host$ make build-dev

