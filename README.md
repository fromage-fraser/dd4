# Dragons Domain IV MUD

The _Dragons Domain IV_ (or _DD4_) MUD is based on the Envy MUD codebase.
It has been significantly customised, with many original game mechanics and areas.

_DD4_ was hosted in New Zealand. The MUD's heyday was around the year 2000; I don't believe it is currently active anywhere.

The codebase is ancient, but can be built and run fairly easily with Docker.

I had a lot of fun playing and extending this MUD.

_Gezhp 2019_


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

1.  Install docker.
1.  Install docker-compose.
1.  From the root directory, build the server image:

    ```
    // With make
    make build

    // With docker-compose directly
    docker-compose build --no-cache
    ```

    This will build a `dd4_server` docker image.
    Repeating this command will rebuild the server.


1.  From the root directory, start the server:

    ```
    // With make
    make up

    // With docker-compose directly
    docker-compose up -d
    ```

    The MUD server will start up on port 8888.

1.  To view the log of the running server:

    ```
    // With make
    make logs

    // With docker-compose directly
    docker-compose logs -f
    ```

1.  To stop the server:

    ```
    // With make
    make down

    // With docker-compose directly
    docker-compose down
    ```

The server container is configured to always restart: edit [the docker-compose configuration](docker-compose.yml)
if you don't want this behaviour.


## Files

| Directory         | Contents                          | Notes
| ---               | ---                               | ---
| `server/area/`    | Area files                        | *
| `server/player/`  | Player files ("pfiles")           | *
| `server/src/`     | Source code                       |
| `faq/`            | Frequently Asked Questions        | See below
| `maps/`           | Maps generated from area files    | See below

`*` These directories are both mounted when using the docker-compose file to run the MUD.
    Areas can be modified and characters saved.


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

| File                                      | Notes
| ---                                       | ---
| [Index](maps/area/index.html)             | Index of all maps
| [Midgaard](maps/area/midgaard.html)       | Starting city
| [Haon Dor](maps/area/haon.html)           | Wilderness west of Midgaard (many low-level areas nearby)
| [Miden'nir](maps/area/midennir.html)      | Wilderness east and south of Midgaard (many low-level areas nearby)
| [Town Of Solace](maps/area/solace.html)   | Low-level city with many areas nearby
| [The City of Anon](maps/area/tcwn.html)   | Key mid-level city


#### World maps

World maps can be found in `maps/world` directory.

| File                                          | Notes
| ---                                           | ---
| [world-map.svg](maps/world/world-map.svg)     | All areas linked to the world. Note the layout is not "geographic": locations are not arranged accurately with respect to the cardinal directions. Portals between areas are shown in this map.

(This map was a first attempt at trying to figure out where everything was again after all these years, but the lack
of any sense of direction made it difficult to use.)


## Immortals

Three original immortals are provided. Log in as one of the following; passwords are blank.

    Gezhp
    Owl
    Shade

If you want to create your own immortal:

1.  Create a new character.
1.  Reach level 2 (complete the training area).
1.  Save your character: `save`.
1.  Quit the game: `quit`.
1.  Edit the new character's pfile. It will be the file `server/player/<initial letter>/<name>`.
    Make the following changes:

    Set the characters level to 106 by altering the value of the existing _Lvl_ key:
    ```
    Lvl         106
    ```
    Add the _sset_ (_skill set_) skill under the existing set of skills:
    ```
    Skll        100 'sset'
    ```
1.  Log back into the game. The character is now level 106, the highest rank of immortal in the game.
1.  Set all skills to 100%. This grants access to every mortal skill and all immortal ("wiz") commands.
    ```
    sset <name> all 100
    ```
1.  Save your character.
1.  The `wizhelp` command should work if these steps were completed successfully.


## Notes

-   _Shouldn't it be "Dragon's Domain"?_

    Probably.

-   I used [tinyfugue](http://tinyfugue.sourceforge.net/) as a game client.

-   The server seems to crash hard if directories it expects to exist do not. E.g. subdirectories of `/server/player`.

-   It would be better to have the server write to persistent log files automatically.
