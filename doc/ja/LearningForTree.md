はじめに
--------
木探索時の着手評価に用いる着手の確率分布を学習させる方法を記述します.
Factorization MachinesとBradley-Terryモデルを組み合わせたモデルを用いて確率的勾配法で学習します.


事前準備
--------
学習するSGFファイルを用意してください.
なお, Rayに実装しているSGFパーサは分岐を扱えないため, 分岐のないSGFファイルである必要があります.
用意したSGFファイルは適当なディレクトリに配置し, 1.sgf, 2.sgf, 3.sgf, ...,と"<数値>.sgf"というファイル名に統一してください.

手順
----

# 学習するパターンの収集
はじめに学習する配石パターンのターゲットを収集します.
学習対象とする配石パターンの収集処理はsrc/learn/PatternAnalyzer.cppに実装しています.
特に実装について変更する必要はありません.

## 設定するパラメータ
パターン収集に関するパラメータは下記の通りです.
各パラメータはinclude/learn/PatternAnalyzer.hppに定義しています.  

| 項目 | 概要 | デフォルト値 | 変更の要否 |
|------|------|--------------|----------|
| ANALYZE_KIFU_PATH | パターンを収集するSGFファイルが格納されているディレクトリのパス. 後述するTRAIN_KIFU_PATHと一致させてください. | *** | 環境に合わせて変更 |
| ANALYZE_KIFU_START | 棋譜の最初のファイルインデックス. | 1 | 実行環境に合わせて変更 |
| ANALYZE_KIFU_END | 棋譜の最後のファイルインデックス. | 30000 | 実行環境に合わせて変更 |
| HASH_TABLE_MAX | マンハッタン距離3以上のパターンを記録するハッシュテーブルのサイズ. | 16777216 | 変更不要 |
| HASH_TABLE_LIMIT | マンハッタン距離3以上のパターンの登録するレコード数の上限. | 16000000 | 変更不要 |
| HASH_TABLE_HALF | マンハッタン距離3以上のパターンを記録するハッシュテーブルの半分のサイズ | HASH_TABLE_MAX / 2 | 変更不要 |
| APPEARANCE_MIN | 学習するパターンの出現回数の閾値 (1以上の整数) | 10 | 適当な値を設定 |

APPEARANCE_MINの値を小さくすると学習対象パターン数が多くなりますが, 過学習しやすくなります.  
一方でAPPEARANCE_MINの数を大きくすると配石パターンの過学習を抑制できますが, 学習対象パターン数が少なくなります.  
最適な値は不明なので, この値については調整の余地があると思われます.


## プログラムの実行
include/learn/PatternAnalyzer.hppを変更し終わったら, src/RayMain.cppにあるAnalyzePattern関数の呼び出しのコメントアウトを解除し, GTP_main関数の呼び出しをコメントアウトしてから, プログラムを再ビルドしてください. ビルドが完了したら, オプションを設定せずに下記コマンドを実行してください.

```
# ./ray
```

プログラムの実行が完了するとlearning_result/pattern-targetディレクトリに下記4つのファイルが生成されます.
 - MD2Target.txt : 学習対象とするマンハッタン距離2の配石パターン
 - MD3Target.txt : 学習対象とするマンハッタン距離3の配石パターン
 - MD4Target.txt : 学習対象とするマンハッタン距離4の配石パターン
 - MD5Target.txt : 学習対象とするマンハッタン距離5の配石パターン

本学習では上記4つのファイルを利用します.

# 学習の実行
学習対象の配石パターンを収集し終わったら, 確率的勾配法で学習を実行します.
学習の処理はsrc/learn/FactorizationMachines.cppに実装しています.
何らかの特徴を追加, 削除して学習したい場合は本ファイルの変更が必要です.

## 設定するパラメータ
本学習で使用するパラメータは下記の通りです.
各パラメータはinclude/learn/LearningSetttings.hppに定義しています.

| 項目 | 概要 | デフォルト値 | 変更の要否 |
|------|------|--------------|------------|
| TRAIN_THREAD_NUM | 学習に使用するスレッド数. 使用するCPUに合わせて変更してください. スレッド数が増えるほど速くできます. | 8 | 実行環境に合わせて変更 |
| TRAIN_KIFU_PATH | 学習に使用するSGFファイルが格納されているディレクトリのパス. ANALYZE_KIFU_PATHと一致させてください. | *** | 実行環境に合わせて変更 |
| TRAIN_KIFU_START_INDEX | 学習する棋譜の最初のファイルインデックス | 1 | 実行環境に合わせて変更 |
| TRAIN_KIFU_LAST_INDEX | 学習する棋譜の最後のインデックス | 30000 | 実行環境に合わせて変更 |
| BTFM_UPDATE_MAX | 学習ループの総実行回数 | 100 | 適切な値を設定 |
| LEARNING_RESULT_DIR_NAME | 学習結果を格納するディレクトリ名 | "learning_result" | 変更不要 |
| PATTERN_TARGET_DIR_NAME | 学習対象の配石パターンファイルを格納したディレクトリ名 | "pattern-target" | 変更不要 |
| TREE_RESULT_DIR_NAME | 本学習結果を格納するディレクトリ名 | "uct" | 変更不要 |
| MD2_TARGET_FILE_NAME | 学習対象のマンハッタン距離2の配石パターンを記録したファイル名 | "MD2Target.txt" | 変更不要 |
| MD3_TARGET_FILE_NAME | 学習対象のマンハッタン距離3の配石パターンを記録したファイル名 | "MD3Target.txt" | 変更不要 |
| MD4_TARGET_FILE_NAME | 学習対象のマンハッタン距離4の配石パターンを記録したファイル名 | "MD4Target.txt" | 変更不要 |
| MD5_TARGET_FILE_NAME | 学習対象のマンハッタン距離5の配石パターンを記録したファイル名 | "MD5Target.txt" | 変更不要 |

## 学習率の減衰スケジュールの設定
学習率のスケジューリングの設定はsrc/learn/FactorizationMachines.cppのInitializeLearning関数内の下記記述で設定しています.
```
  lr_schedule[ 0] = 0.01;
  lr_schedule[ 5] = 0.005;
  lr_schedule[15] = 0.0025;
  lr_schedule[25] = 0.00125;
  lr_schedule[35] = 0.000625;
  lr_schedule[40] = -1.0;
```

lr_scheduleのキーに学習率を変更する学習エポック数, バリューに学習率を設定します. -1.0を代入しているエポック数で学習を終了します.

## プログラムの実行
include/learn/LearningSettings.hppを変更し終わったら, src/RayMain.cppのTrainBTModelWithFactorizationMachines関数のコメントアウトを解除してプログラムを再ビルドしてください. ビルドが完了したら, オプションを設定せずに書きコマンドを実行してください.

```
# ./ray
```

棋譜を1周学習するたびにlearning_result/uct/resultディレクトリにパラメータを出力します. 各特徴の名前をつけられたテキストファイルは学習ファイルなので, 特に必要はありません.
学習したパラメータを利用する場合はlerning_result/uct以下にあるresultディレクトリの内容をuct_paramsに配置してください.


特徴の追加
----------
特徴を追加する場合は, 特徴の定義をinclude/feature/UctFeature.hppに, 特徴の判定処理をinclude/feature/UctFeature.cppに定義してください. src/learn/FactorizationMachines.cppに追加した特徴の判定と更新処理, 学習用の変数(btfm_t型)を追加することで, 新たに追加した特徴の学習を実行できます.  

FAQ
---

### Q1. 使用できる棋譜の数の限界は.
不明です. デフォルトで添付しているパラメータファイルは30万局で学習させています.

### Q2. 確率的勾配法の更新手法は.
たぶんAdamWです. AdamからL2正則化項の問題を解決したものです.

