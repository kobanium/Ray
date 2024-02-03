# About Ray
Ray is a Go (Weiqi, Baduk) game engine based on Monte-Carlo tree search without Deep Learning.

日本語は[こちら](doc/ja/README.md).

# Installation
1. 'cd' to the directory which includes 'Makefile'
2. Type 'make' to compile
3. Place parameter files to ```sim_params``` and ```uct_params``` directories (You can download parameter files from [here](https://github.com/kobanium/Ray/releases))

# How to run
When using a GUI that supports GTP (Go Text Protocol), the following command can be used Ray as a engine.
```
./ray
```

Ray's command line options are as follows,

## Game settings
| Option | Description | Value | Example of value | Default value | Note |
| --- | --- | --- | --- | --- | --- |
| `--size` | Size of go board| Integer more then 1 and less than or equal to PURE_BOARD_SIZE | 9 | PURE_BOARD_SIZE ( = 19 ) | PURE_BOARD_SIZE is defined in include/board/Constant.hpp |
| `--komi` | Komi value | Real number | 6.5 | KOMI ( = 6.5 ) | KOMI is defined in include/board/Constant.hpp |
| `--superko` | Positional super ko | - | - | - | Supporting positional super ko only |
| `--handicap` | The number of handicap stones | Integer more than 1 | 2 | 0 | This option is for debugging. |
| `--cgos` | CGOS player mode | - | - | - | Activating capturing all dead stones mode |

### annotation
The `--playout` option, the `--const-time option`, and the `--time` option are effective when specified last, respectively.

## Time management and search settings
| Option | Description | Value | Example of value | Default value | Note |
| --- | --- | --- | --- | --- | --- |
| `--playout` | The number of playouts per move | Integer more than 0 | 1000 | - | |
| `--const-time` | Time to thinking per move (seconds) | Real number | 7.5 | CONST_TIME ( = 10.0) | CONST_TIME is defined in include/mcts/SearchManager.hpp |
| `--time` | Total remaining time for a game (minutes) | Real number | 1800.0 | - | |
| `--thread` | The number of search thread | Integer more than 0 and less than or equal to THREAD_MAX ( = 64 ) | 16 | 1 | THREAD_MAX is defined in include/mcts/UctSearch.hpp |
| `--reuse-subtree` | Reusing MCTS sub-tree | - | - | - | |
| `--pondering` | Pondering on opponent's thinking time | - | - | - | |
| `--tree-size` | Maximum number of MCTS nodes | Integer power of 2 | 16834 | UCT_HASH_SIZE ( = 16834 ) | UCT_HASH_SIZE is defined in include/board/ZobristHash.hpp |
| `--resign` | Resign threshold | Rean number more than or equal to 0.0 and less than or equal to 1.0 | 0.1 | RESIGN_THRESHOLD ( = 0.20 ) | RESIGN_THRESHOLD is defined in include/mcts/MoveSelection.hpp |

### annotation
When the `--pondering` option is enabled, the `--reuse-pondering` option is automatically enabled automatically.  

## Misc

| Option | Description | Value | Example of value | Default value | Note |
| --- | --- | --- | --- | --- | --- |
| `--no-debug` | No debug message mode | - | - | - | |


## Example settings

By default settings, Ray will consume 10 seconds each move on a single CPU 
and require 800MB of memory. 

    ./ray

Playing with 4 sec/move with 8 threads

    ./ray --const-time 4 --thread 8

Playing with 1000 playouts/move with 1 threads

    ./ray --playout 1000

Playing with 16 threads and 65536 uct nodes. Time setting is 30 minutes.
Ray thinks during the opponent's turn.

    ./ray --time 1800 --thread 16 --tree-size 65536 --pondering


## License
Ray is distributed under the BSD License.
Please see the "COPYING" file.

## Contact
rayauthor19x19@gmail.com (Yuki Kobayashi)
