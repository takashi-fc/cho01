# GoogleAppsScriptの快適な開発環境（ES6 + ローカル開発）

みなさんGoogleAppsScriptは使ったことがありますか？
GoogleAppsScriptは簡単に言うと、Googleのサーバ上でJavaScriptを実行できるサービスです。
Googleのサービスとの連携が簡単で、様々なことができます。例えば、

* GoogleFormsで作ったアンケートが回答されたらチャットに流す。
* 毎日クロールした結果をGoogleSpreadSheetsに溜めていく。
* 毎朝GooglePlay,AppStoreのレビューをチャットに流す。

のようなことができます。ただ、オンラインエディタ上でしか書けないため、普段使いしている慣れた環境で開発することができません。
この章では、オンラインエディタではなくES6+ローカルで開発できるようにするという内容です。

## ローカルで開発する

ローカルのファイルをアップロードするための選択肢として

* https://github.com/danthareja/node-google-apps-script
* https://github.com/soundTricker/gas-manager

があります。今回はGoogleDevelopersJapanで過去に紹介されたnode-google-apps-scriptを利用します。

1. GoogleDrive上でGoogleAppsScriptのプロジェクトを作成
2. そのプロジェクトを開いて、[リソース] > [Googleの拡張サービス] を開いてDriveApiを有効にする
3. そのまま[Google API コンソール]のリンクをクリック
4. [Google Apps API] > [Drive API] > [有効にする]をクリック
5. そのまま左メニューの[認証情報] > [認証情報を作成] > [OAuthクライアントID]をクリック
6. アプリケーションの種類は[その他]、名前に[GoogleAppsScript-local-development]など適当な名前を設定し、作成
7. OAuthクライアントという画面が表示されるがそのまま閉じる
8. 作成した[GoogleAppsScript-local-development]の右側にあるダウンロードボタンをクリックし、jsonを保存
9. `npm install -g node-google-apps-script`
10. `gapps auth ダウンロードしたjsonのパス`
11. 表示されるURLをブラウザで開いて、認証
12. `gapps init 1で作成したプロジェクトのID` (IDは[https://script.google.com/d/xxx/edit]の[xxx]部分)
13. [gapps.config.json], [src/コード.js]が作成されている
14. `gapps upload`することでコード.jsが反映されます

これでローカルで編集したファイルを反映させることができ、Gitで管理できるようになりました。
srcディレクトリでファイルを作成・削除・変更したものが同期されます。その際、.jsから.gsへとGoogleAppsScript用の形式に変換されます。.jsが不完全な場合に.gsへの変換が失敗し同期が失敗します。

## ES6で開発する

GoogleAppsScriptはES6に対応していないため、ES6で記述したJavaScriptをそのまま実行することはできません。
そのため https://babeljs.io を利用して、実行可能なJavaScriptのコードに変換します。
単純にBABELを利用して変換しても良いですが、コードを変更したら自動で変換できるように、gulpも利用します。

1. `npm install -g gulp && npm install --save-dev gulp gulp-babel babel-preset-es2015`
2. es6というディレクトリを作って、そこに`コード.js`と同じ名前のファイルを作成
3. リスト1.1のgulpfile.jsというファイルを作成
4. `gulp babel`と実行すると、src/コード.jsに変換されたJavaScriptが出力される
5. `gapps upload`することでコード.jsが反映されます
6. `gulp`と実行すると、監視状態になりes6のコードを変更したら、babelで変換し、アップロードまでを自動的に行います

```gulpfile.js
var gulp = require('gulp');
var babel = require('gulp-babel');
var spawn = require('child_process').spawn;

gulp.task('default', ['watch', 'upload']);

gulp.task('watch', () => {
  gulp.watch('es6/*.js', ['babel']);
});

gulp.task('babel', () => {
  return gulp.src('es6/*.js')
    .pipe(babel({
			presets: ['es2015']
		}))
    .pipe(gulp.dest('src'));
});

gulp.task('upload', () => {
  gulp.watch('src/*.js', function() {
    spawn('gapps', ['upload']);
    console.log('finished `gapps upload`');
  });
});
```

```es6/コード.js
class Bot {
  constructor(message) {
    this.message = message;
  }

  say() {
    Logger.log(this.message);
  }
}

function main() {
  var bot = new Bot("ようこそ技術書典へ");
  bot.say();
}
```

これでローカルでes6のコードを変換し、GoogleAppsScript上で実行することができました。

## コマンドラインからGoogleAppsScript上のコードを実行する

さて、ローカルで快適にJavaScriptを書くことができるようになりましたが、今のままでは結局オンラインエディタ上で実行しないといけません。

1. 先程作ったGoogleAppsScriptのプロジェクトを開く
2. そのプロジェクトを開いて、[リソース] > [Googleの拡張サービス] を開いてDriveApiを有効にする
3. そのまま[Google API コンソール]のリンクをクリック
4. [Google Apps API] > [Google Apps Script Execution API] > [有効にする]をクリック

```実行コマンド
curl -X POST https://script.googleapis.com/v1/scripts/1BIxem_DZ-9n1lScDDl_WMe6hnh2iAt9gkIavbhI3FQj2ChwLimPBfU79:run \
-d '{
  "function": "main",
  "parameters": [
    "hoge"
  ],
  "devMode": true,
}'
```

これでコードの書き始め〜完成まで、全てローカルで試せるようになりました。

## さいごに

どうでしたか？冒頭で書いた以外にもGoogleAppsScriptは様々な用途に使用できます。
cronのように定期的に実行することも可能であれば、APIとしてget,postなどを実装することも可能です。
つまり、無料で日々のタスクの自動化からアプリのAPIとして利用することも可能だということです。
より快適なGoogleAppsScript開発のための内容でした。
