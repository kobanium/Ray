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

```
# git clone https://github.com/kobanium/Ray.git
# cd Ray
# make
```

使い方
------


コマンドラインオプション
------------------------


| オプション | 概要 | 使い方 |
|------------|------|--------|
| --size  | 碁盤のサイズの指定 | --size [2以上19以下の整数] |
| --playout  | 1手あたりのプレイアウト回数の指定 | --playout [1以上の整数] |
| --const-time  | 1手あたりの思考時間(秒数)の指定 | --const-time [1以上の整数] |
| --time  | 持ち時間の指定 | --time [1以上の整数] |
| --thread  | 探索するスレッド数の指定 | --time [1以上の整数] |
| --komi  | コミの指定 | --komi [任意の小数] |
| --handicap  | 置き石の個数の指定 | --handicap [2以上の整数] |
| --reuse-subtree  | 探索木の再利用を有効化 | --reuse-subtree |
| --pondering | 予測読みの有効化 | --pondering |
| --tree-size | 探索木の最大サイズの指定 | --tree-size [2以上の2のべき乗の整数] |
| --no-debug  | コンソールメッセージ出力の抑止 | --no-debug |
| --superko  | 超劫の有効化 | --superko |


開発者向け
----------
モンテカルロ・シミュレーション用のパラメータの学習の方法は[こちら](LearningForSimulation.md).
