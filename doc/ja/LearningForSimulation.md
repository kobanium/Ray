はじめに
--------
モンテカルロ・シミュレーションに用いる着手の確率分布を学習させる方法を記述します.
使用している手法はBradley-Terryモデルを用いたMinorization Maximization法です.


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

| 項目 | 概要 | デフォルト値 |
|------|------|--------------|
| ANALYZE_KIFU_PATH | パターンを収集するSGFファイルが格納されているディレクトリのパス. 後述するTRAIN_KIFU_PATHと一致させてください. | *** |
| ANALYZE_RESULT_PATH | 収集した学習対象パターンを記録したファイルの出力ディレクトリ. この項目は変更不要です. | "learning_result/analyze" |
| ANALYZE_KIFU_START | 棋譜の最初のファイルインデックス. | 1 |
| ANALYZE_KIFU_END | 棋譜の最後のファイルインデックス. | 30000 |
| HASH_TABLE_MAX | マンハッタン距離3以上のパターンを記録するハッシュテーブルのサイズ. この項目は変更不要です. | 16777216 |
| HASH_TABLE_LIMIT | マンハッタン距離3以上のパターンの登録するレコードスウの上限. この項目は変更不要です. | 16000000 |
| HASH_TABLE_HALF | この項目は変更不要です. | HASH_TABLE_MAX / 2 |
| APPEARANCE_MIN | 学習対象とする配石パターンの出現回数の閾値. この値以上の回数出現した配石パターンを学習対象とします. | 5 |


ANALYZE_KIFU_PATH, ANALYZE_KIFU_START, ANALYZE_KIFU_ENDを環境に合わせて定義してください.
またより多くのパターンを学習したい場合はAPPEARANCE_MINの値を変更してください.  

## プログラムの実行
include/learn/PatternAnalyzer.hppを変更し終わったら, src/RayMain.cppにあるAnalyzePattern関数の呼び出しのコメントアウトを解除し, GTP_main関数の呼び出しをコメントアウトしてから, プログラムを再ビルドしてください. ビルドが完了したら, オプションを設定せずに下記コマンドを実行してください.

```
# ./ray
```

プログラムの実行が完了するとANALYZE_RESULT_PATHに指定したディレクトリ配下に下記4つのファイルが生成されます.
 - MD2Target.txt : 学習対象とするマンハッタン距離2の配石パターン(対称系, 色反転をまとめたパターン)
 - MD2TargetForSimulation.txt : 学習対象とするマンハッタン距離2の配石パターン(対称系のみまとめたパターン)
 - MD3Target.txt : 学習対象とするマンハッタン距離3の配石パターン
 - MD4Target.txt : 学習対象とするマンハッタン距離4の配石パターン
 - MD5Target.txt : 学習対象とするマンハッタン距離5の配石パターン

ファイル名が紛らわしいですが, 使用するのはMD2Target.txtになります. MD2TargetForSimulation.txtは試験的に実装したもので特に使用しません. 

# 学習の実行
学習対象の配石パターンを収集し終わったら, Minorization-Maximization法を用いて学習を実行します.
学習の処理はsrc/learn/MinorizationMaximization.cppに実装しています.
何らかの特徴を追加, 削除して学習したい場合は本ファイルの変更が必要です.

## 設定するパラメータ
学習に関するパラメータは下記の通りです.
各パラメータはinclude/learn/LearningSetttings.hppに定義しています.

| 項目 | 概要 | デフォルト値 |
|------|------|--------------|
| TRAIN_THREAD_NUM | 学習に使用するスレッド数. 使用するCPUに合わせて変更してください. スレッド数が増えるほど速くできます. | 6 |
| TARGET_PATTERN_MAX | 学習対象パターンの上限値. MD2Target.txtの行数以上の数値にしてください. | 500000 |
| TRAIN_KIFU_PATH | 学習に使用するSGFファイルが格納されているディレクトリのパス. ANALYZE_KIFU_PATHと一致させてください. | *** |
| TRAIN_KIFU_START_INDEX | 学習する棋譜の最初のファイルインデックス. | 1 |
| TRAIN_KIFU_LAST_INDEX | 学習する棋譜の最後のインデックス. | 30000 |
| TEST_KIFU_PATH | 着手予測精度評価用のSGFファイルが格納されているディレクトリのパス. |
| TEST_KIFU_START_INDEX | 着手予測精度評価用の棋譜の最初のファイルのインデックス | 40001 |
| TEST_KIFU_LAST_INDEX | 着手予測精度評価用の棋譜の最後のファイルのインデックス | 40001 |
| ACCURACY_LOG_FILE_NAME | 着手予測正解率を記録するファイル名. この項目は変更不要です | "accuracy.log" |
| UPDATE_INTERVAL | 全ての特徴を更新するのに必要なステップ数 | 8 |
| UPDATE_STEPS | 学習ループ実行回数 | 30 |


TRAIN_THREAD_NUM, TRAIN_KIFU_PATH, TRAIN_KIFU_START_INDEX, TRAIN_KIFU_LAST_INDEX, TEST_KIFU_PATH, TEST_KIFU_START_INDEX, TEST_KIFU_LAST_INDEXを環境に合わせて変更してください.

## プログラムの実行
include/learn/LearningSettings.hppを変更し終わったら, src/RayMain.cppTrainBTModelByMinorizationMaximization関数のコメントアプトを解除してプログラムを再ビルドしてください. ビルドが完了したら, オプションを設定せずに書きコマンドを実行してください.

```
# ./ray
```
学習ループがUPDATE_INTERVAL回ごとにlearning_result/md2/result**ディレクトリにパラメータを出力し, accuracy.logファイルにテストファイルでの正解率の計測結果を追記していきます. 各特徴の名前をつけられたテキストファイル(ex. SIM_ATARI.txt)は学習経過ファイルなので, 特に必要ありません.
学習したパラメータを利用する場合はlerning_result/md2以下にあるresult**ディレクトリの内容をsim_paramsに配置してください.




FAQ
---

### Q1. パラメータが発散してしまう.
パラメータが発散してしまう場合は, 学習に使用する棋譜の数を減らしてください.
使用する棋譜の質にもよりますが, 40000局以上を学習させようとするとパラメータが発散する可能性があることは確認しています.
強さの観点で言えば, 20000局あればほぼ十分だと思われます.

### Q2. APPEARANCE_MINの最適値は.
不明です. ただし, 1にすると顕著に過学習する傾向があるようです.
棋譜の数, 性質に合わせて, 調整してください.