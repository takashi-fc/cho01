# GoogleAppsScriptライブラリ開発

みなさんGoogleAppsScript（以下、GAS）は使ったことがありますか？
GASは簡単に言うと、Googleのサーバ上でJavaScriptを実行できるサービスです。
外部にAPIとして公開しPOST,GETのリクエストを受けたり、GoogleFusionTables（以下、FusionTables）やGoogleSpreadSheets（以下、SpreadSheets）でデータを保持することもできます。
個人のアプリでもバックエンドにGASを利用していて、DBにSpreadSheetsを利用し、ライブラリ化した際に得られた知見と成果の内容です。

## きっかけとGoogleAppsScriptから利用するDBの選択

結論から言うと、FusionTablesよりSpreadSheetsを利用することをオススメします。
理由としては、FusionTablesがUPDATE, DELETEでWHERE句が利用できないからです。正確に言うと、ROW_IDというオートインクリメントされるカラムしか指定できません。
削除したい対象をSELECTで抽出してから、ROW_IDを指定してDELETEすることは可能です。
しかし、書き込みの上限が30件/分かつ5000件/日と厳しく、リクエストを送るクライアントが多いと現実的ではありません。

そこでSpreadSheetsの出番です。SpreadSheetsをDBとして扱うメリットとしては「非エンジニアでも触れる」だと思います。例えば、

* ボタンやプッシュ通知のA/Bパターン文言
* メールやチャットで利用するテンプレートメッセージ

などを改善していく際に、エンジニアを介さず行えることが大きな利点です。
そんな気軽に変更されてはたまらん！というデータはシートを分けておき、編集可能者を制限しておくと良いでしょう。それぞれに実行権限を付与できるのも利点です。

良いところしかないのかと言うとそうではなく、SpreadSheetsのAPIで提供されているメソッドでしか情報をやり取りすることができません。そして、そこにSQLの選択肢はなく、SQLを利用してデータを抽出することができません。
そこで SpreadSheetsSQL というSpreadSheetsからSQLライクにデータを抽出できるGASのライブラリを公開しています。このライブラリなしでもSpreadSheetsをDBとして利用することはです。

## ライブラリの開発

> SpreadSheetsSQL: https://github.com/roana0229/spreadsheets-sql

GASのライブラリはGASで作ることができます。SpreadSheetsSQLのソースコードを例に説明します。
まず初めにこのライブラリを使う時は以下のように使います。

```サンプルコード
var result = SpreadSheetsSQL.open(SHEET_ID, SHEET_NAME).select(['name', 'cv']).result();

// resultの内容
[{
  name: 'サーバルちゃん',
  cv: '尾崎由香'
},{
  name: 'かばんちゃん',
  cv: '内田彩'
}]
```

SpreadSheetsSQLを利用する際には、`SpreadSheetsSQL.open()`を呼ぶ必要があります。
これは`ライブラリのプロジェクト名.メソッド`という形になっていて

```SpreadSheetsSQL.gs
/**
 * This method use to create SpreadSheetsSQL instance.
 * @param {String} id SpreadSheet id
 * @param {String} name SpreadSheet sheet name
 * @return {SpreadSheetsSQL} SpreadSheetsSQL instance
 */
function open(id, name) {
  return new SpreadSheetsSQL_(id, name);
}
```

コードではこのようになっています。コメントはJSDocで記述することができます。
`new SpreadSheetsSQL_(id, name)`で`SpreadSheetsSQL_`というクラスをインスタンス化していることがわかると思います。
また、末尾にアンダースコアがついていることにお気付きかと思います。これはスコープ制御をするためであり、ライブラリ内でのみ参照できるプライベートな状態になります。
`SpreadSheetsSQL.open()`した際の戻り値は`SpreadSheetsSQL_`型ですが、JSDocにはアンダースコアなしで記述されています。
実際にはプライベートクラスのインスタンスが返っているが、JSDocを見るとプロジェクト名の型のようなものが返るということです。

次に取得するカラムの指定に`SpreadSheetsSQL.open().select()`を呼ぶ必要があります。

```SpreadSheetsSQL.gs
/**
 * This method use to get columns.
 * <pre><code>Example: SpreadSheetsSQL.open(id, name).select(['name', 'age', 'married', 'company']).result();
 * </pre></code>
 * @param {String[]} selects column array
 * @return {SpreadSheetsSQL} SpreadSheetsSQL instance
 */
function select(selects) {
  throw new Error("it's a mock method for content assist");
}
```

コードではこのようになっています。`result()`でも同様のコードが記述されています。
呼び出されたら例外を投げるメソッドに見えます。しかし、ここで宣言されているメソッドはあくまでライブラリ外から参照するためのメソッドでしかありません。実態は

```SpreadSheetsSQL.gs
class SpreadSheetsSQL_ {
  ...
  select(selects) {...}
  ...
}
```

のように`SpreadSheetsSQL_`クラス内に記述されています。通常ではプライベートなクラスのメソッドは外からは見えず呼び出すことができません。メソッドが見えないということは呼び出すことができないということになっていまします。
そのため、ライブラリのライブラリのルート階層にインターフェースとして記述し、プライベートなメソッドでもGASのオンラインエディタでの補完を有効にしています。

まとめるとこのようになります。

* プロジェクト名.メソッドという形で呼び出すことができる。
* 末尾にアンダースコアでライブラリ外からは見えないようにする。
* 補完のためのインターフェースをルート階層に列挙し、JSDocの@returnにはライブラリ名を指定する。

## ライブラリの公開

開発もそこまで気をつけることはありませんでしたが、公開はもっと簡単です。

1. GASのライブラリ化したいプロジェクトを開く。
2. 「ファイル」->「版を管理」に移動して、新しいバージョンを保存する。
3. 「ファイル」->「プロジェクトのプロパティ」で表示される、スクリプトIDを共有する。

たったこれだけで、他人がでライブラリとしてGASプロジェクトを扱うことができます。

## おまけ: ES6 + ローカルで開発する

GASは個人でサーバを用意せず、気軽に開発できて便利です。
ただ、オンラインエディタ上でしか書けないため、普段使いしている慣れた環境で開発することができません。
そこでbabel + gulp + node-google-apps-script を使うことで、ES6かつローカルで開発しています。

ローカルのファイルをアップロードするための選択肢としていくつか方法がありますが、今回はGoogleDevelopersJapanで過去に紹介された node-google-apps-script (https://github.com/danthareja/node-google-apps-script) を利用します。セットアップはURL先に譲ります。

また、GASはES6に対応していないため、ES6で記述したJavaScriptをそのまま実行することはできません。
そのため https://babeljs.io を利用して、実行可能なJavaScriptのコードに変換します。
単純にbabelを利用して変換しても良いですが、コードを変更したら自動で変換+同期できるように、gulpを利用します。

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

```es6のコード.js
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

```babelによって変換されたコード.js
"use strict";

var _createClass = function () {...}();

function _classCallCheck(instance, Constructor) {...}

var Bot = function () {
  function Bot(message) {
    _classCallCheck(this, Bot);

    this.message = message;
  }

  _createClass(Bot, [{
    key: "say",
    value: function say() {
      Logger.log(this.message);
    }
  }]);

  return Bot;
}();

function main() {
  var bot = new Bot("ようこそ技術書典へ");
  bot.say();
}
```

これでローカルかつES6で開発しながら、GASに実行可能な状態で同期することができました。

## さいごに

どうでしたか？GoogleAppsScriptが大きく目立つことがない中、更にそのライブラリを作るというニッチなところまとめてみました。GoogleAppsScriptはAPI+DBとしてだけではなく、cronのように定期的に処理を実行することも可能です。また、個人ではなかなかサーバ側に手が出せないという人や、ちょっとした便利ツールを作る時にとても便利だと思っています。
