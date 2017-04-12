# 超技術書典

## じゅんび

* OSX or macOSちほー
* rbenvが使えるフレンズ

```
git clone git@github.com:takashi-fc/cho01.git && cd cho01
rbenv install 2.2.3
rbenv rehash
gem install review
gem install md2review # markdownで書いている場合
```

#### PDFを作るのが得意なフレンズになる方法（上の作業中に並行してやるのがオススメ）

[C89-FirstStepReVIEW-v2/blob/master/articles/setup.re](https://github.com/TechBooster/C89-FirstStepReVIEW-v2/blob/master/articles/setup.re) より引用

> Re:VIEW文書をPDFに変換するにはLaTeX（platexまたはlualatexなど）を使います。  
出力時の処理はreview形式→reviewツール実行→latex形式→platex実行→PDF という流れです。

とゆーことなので、論文などを書いた環境じゃなければ下記のインストールが必要だよ

1. MacTeX http://tug.org/cgi-bin/mactex-download/MacTeX.pkg をダウンロードしてインストール  
(※2.7GBぐらいある重たいフレンズなので、時間がかかるよ)
2. ターミナル再起動
3. これであなたも本が作れるフレンズになったよ

## とりあえずRe:Viewを試してみたいフレンズ

```
cp -rf ~/.rbenv/versions/2.2.3/lib/ruby/gems/2.2.0/gems/review-2.2.0/test/sample-book ./
cd sample-book/src
review-pdfmaker config.yml
open book.pdf
```

## 自分の書いた文章をpdf化したいフレンズ

```
# markdonで書いていた場合、Re:View形式に変換
cd cho01
md2review roana0229/gas.md > articles/gas.re

cd articles
vim catalog.yml
### CHAPS:
###  - ファイル名を追記

# .reをpdf化
review-pdfmaker config.yml
open cho01.pdf
```
