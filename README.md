# nucleo_utility
ARM用のgccを使ってコンパイルできるNucleo用のプログラムやライブラリ群

# Settings
## ARM-GCC Install
[このWebページのLinux欄参照](http://yuqlid.sakura.ne.jp/dokuwiki/doku.php?id=stm32_devenv)

## Makefile
* メインファイルの名前を変更した場合は、Makefileの"OBJECTS += "を変更する
* プロジェクトの名前を変更した場合は、Makefileの"PROJECTS += "を変更する
* このリポジトリをCloneしたパスにMakefileの"LIBRARY_DIR += "を変更する

# Write for Nucleo
1. makeに成功するとBUILD/$PROJECTS.binというバイナリファイルが生成される
2. PCとNucleoを接続するとNucleoがマウントされる
3. 2.でマウントされたところに1.のバイナリファイルをコピーするとプログラムが書き込まれる.
4. PCとNucleoを一度切断してから再接続すると正常に動作する

# Source
* template_hoge: Nucleo hogeのテンプレートプログラム
* mdd: Nucleo L432KCを用いたMDDのプログラム

# Feature
* ST-Linkによるデバッグ
* constexprの有効化
