Rayについて
-----------
Rayはモンテカルロ木探索を用いた囲碁の思考エンジンです.
AlphaGo出現前の技術を用いているため, 深層学習は使用していません.  
RayはGTP(Go Text Protocol)に対応した思考エンジンのため, GUIを持ちません.
対局相手として利用する際には, GoGUIやLizzieのようにGTPに対応したGUI経由で利用してください.  

インストール
------------
Windowsでも動くと思われますが, Linux (Ubuntu 16.04, 18,04, 20.04) 環境で開発しているため,
Windowsでの動作確認はしていません. Linuxで利用する際には下記手順でソースコードをビルドしてください.  
前提ツールはg++とmakeのみで, その他パッケージのインストールは不要です.
ただし, C++11の機能を用いているのでC++11をサポートしているバージョンのコンパイラをご利用ください.

```
# git clone https://github.com/kobanium/Ray.git
# cd Ray
# make
```

使い方
------



## Game settings
| オプション | 概要 | 設定する値 | 値の例 | デフォルト値 | 備考 |
| --- | --- | --- | --- | --- | --- |
| `--size` | 碁盤のサイズの指定 | 1以上PURE_BOARD_SIZE以下の整数 | 9 | PURE_BOARD_SIZE ( = 19 ) | PURE_BOARD_SIZE は include/board/Constant.hpp に定義しています。 |
| `--komi` | コミの値の設定 | 実数 | 6.5 | KOMI ( = 6.5 ) | KOMI は include/board/Constant.hpp に定義しています。 |
| `--superko` | 超劫の有効化 | - | - | - | Positional Super Koのみサポートしています。 |
| `--handicap` | 置き石の数の指定 | 1以上の整数 | 2 | 0 | デバッグ用のオプションです。 |

### 注意
--playoutオプション, --const-timeオプション, --timeオプションはそれぞれ最後に指定されたものが有効になります.  



## Time management and search settings
| Option | Description | Value | Example of value | Default value | Note |
| --- | --- | --- | --- | --- | --- |
| `--playout` | 1手あたりのプレイアウト回数の指定 | 1以上の整数 | 0 | 1000 | - | |
| `--const-time` | 1手あたりの思考時間の指定 | 実数 | 7.5 | CONST_TIME ( = 10.0) | CONST_TIME は include/mcts/SearchManager.hpp に定義しています。 |
| `--time` | 1局の持ち時間の指定 (分) | 実数 | 1800.0 | - | |
| `--thread` | 探索スレッド数の指定 | 1以上THREAD_MAX ( = 64 ) 以下の整数 | 16 | 1 | THREAD_MAX は include/mcts/UctSearch.hpp に定義しています。 |
| `--reuse-subtree` | サブツリー再利用の有効化 | - | - | - | |
| `--pondering` | 予測読みの有効化 | - | - | - | |
| `--tree-size` | MCTSノード数の指定 | 2のべき乗の整数 | 16834 | UCT_HASH_SIZE ( = 16834 ) | UCT_HASH_SIZE は include/board/ZobristHash.hpp に定義しています。 |

### 注意
--ponderingオプションを有効にすると, 自動的に--reuse-ponderingオプションが自動で有効になります.  


## Misc

| Option | Description | Value | Example of value | Default value | Note |
| --- | --- | --- | --- | --- | --- |
| `--no-debug` | デバッグメッセージ出力の抑制 | - | - | - | |


## 開発者向け
モンテカルロ・シミュレーション用のパラメータの学習の方法は[こちら](LearningForSimulation.md).  
木探索用のパラメータの学習方法は[こちら](LearningForTree.md).