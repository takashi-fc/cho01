# PHPのORマッパーを調べてみた 2017 

## ぴーえっちぴーちほーで元気に暮らす老害おじちゃん！

こんにちは。 @boscoworks です。普段は目黒あたりでPHPエンジニアをやっています。  
最近はRuby on RailsでのWeb開発が主流になってきましたね。仕事柄、社外のエンジニアの方ともよくお会いするのですが、Railsをやっているエンジニアはとても多い印象です。  
私の周りでもたまに「*PHPはオワコン*」という言葉が飛び交っていて「そろそろエンジニアから足を洗うときが近づいているかな」などと冗談めかして言ったりしています。~~来年あたり本当にやめているかもしれない。~~  
さて、とはいえ私自身も私の周辺もまだまだPHPは活況、PHP+MySQLは未だWeb開発の王道を行っていると断言して差し支えないでしょう。  
ただPHPでのWeb開発をやっていて（個人的に）いつも悩むのがORマッパーの選定です。ぐるぐる悩んだ結果「ええいもうPDOで良くないかコレ！」と叫んだこともしばしば。  
結局フレームワークに標準で入ってるORマッパーを使うのが定石なんですけどね、なんかこうモヤモヤしませんか？しますよね？  
ということで、最近のPHPのORマッパーをあれこれ調べることにしてみましたよ。  

## 調査対象
独断と偏見で以下をピックアップしました。  

### 1. Propel
 - 一時代前のSymfonyが標準ORMとして導入していたやつ。最近はあんまり聞かない。
     - http://propelorm.org/
     - https://github.com/propelorm/Propel2
     - https://packagist.org/packages/propel/propel

### 2. Doctrine
 - 最近のSymfonyが標準ORMとして導入しているやつ。ザ・PHPのORM感ある。
     - http://www.doctrine-project.org/
     - https://github.com/doctrine/doctrine2
     - https://packagist.org/packages/doctrine/orm

### 3. Eloquent
 - LaravelのORM。こいつが有名というよりは、Lavavelが急成長した結果、相対的に使われるようになった的な。
     - https://laravel.com/docs/5.4/eloquent
     - https://github.com/illuminate/database
     - https://github.com/laravel/framework
     - https://packagist.org/packages/illuminate/database
     - https://packagist.org/packages/laravel/framework

### 4. Idiorm & Paris
 - 「PHP ORマッパー」でググると上の方に出てくるやつ。
     - http://j4mie.github.io/idiormandparis/
     - https://github.com/j4mie/idiormandparis

## 調査方法
3つの視点から「どのORマッパーが人気なのか」を探ります。

+ 「Googleトレンド ( https://trends.google.co.jp/trends/ )」による人気度の動向調査
+ GitHubのStar数、Fork数
+ Packagist経由でのcomposer install数

### Googleトレンドによる人気度の動向調査
  
![ORM動向](images/boscoworks_trend1.png)

Doctrineはさすが王者の風格、1位を譲らないですね。  
注目はEloquent、2013年末あたりでPropelを追い越してからも順調に右肩上がり。いまやDoctrineに並ぶ勢いですね。  
Idiorm...( ＾ω＾)  

これ結局SymfonyとLaravelのシェア争いとイコールになるんじゃないかと思い調べてみると。  

![フレームワーク動向](images/boscoworks_trend2.png)
  
Symfonyはシェアを落とすことなく、ただLaravelが怒涛の右肩上がり。Laravel人気すごいですね。  

### GitHubのStar数、Fork数
Star数、Fork数が多いということは、すなわち開発が活発に行われているということ。  
不具合があれば直りやすいし、周辺技術への追従も比較的頻繁に行われるかもしれません。  

|ORM|Star数|Fork数|
|---|---:|---:|
|Propel|940|282|
|Doctrine|3,505|1,686|
|Eloquent(illuminate/database)|1,008|281|
|Eloquent(laravel/framework)|6,941|4,134|
|Idiorm|8|0|

(※3/28調べ)

Eloquent は illuminate/database というパッケージが大本になっていたのが、 laravel/framework に replace されたのですね。
なのでStar数Fork数は分けて表示。なんか比較できない感じになってしまった。  
DoctrineとEloquentはいいとして、PropelのStar数、Fork数がそこそこあるのが意外です。  
いまやレガシーなORMと思いきや、しっかりメンテナーもついているんですね。  

### Packagist経由でのcomposer install数
|ORM|Install数|
|---|---:|
|Propel|295,535|
|Doctrine|21,048,164|
|Eloquent(illuminate/database)|2,262,532|
|Eloquent(laravel/framework)|20,991,608|
|Idiorm|194,306|

(※3/28調べ)

こうやってみると Doctrine 健闘しているんですね。 Laravel 本体といい勝負。  
Idiorm は composer install はされてるのに GitHub では下火なのが印象的。興味本位でインストールして、そのままバイバイなケースが多いのかな。  
  

## 閑話休題

### FuelPHP のORマッパーについて一言言わせてほしいんだ

最近、完全に私の好みで FuelPHP でプロダクトコードを書いているのですが、ORマッパーが微妙だなーと思うのは私だけでしょうか・・・。  
チェーンメソッドでクエリを構築できるのは直感的に書けて良いのですが、

- SELECTクエリの結果をデフォルトでメモリにキャッシュしちゃう
- モデルクラスが oil コマンドで強制上書きなので、DDLに弱い (ただし工夫すれば回避できる)
- アンダーバー区切りでディレクトリを切る仕組みなので、N対Nなテーブルの中間テーブルはいびつなモデル名になる
- Model_Crud とクエリビルダが共存することができる。どっちかに寄せないとすぐキメラになる

みたいな感じで、ちょっと癖のある子になっています。  

### FuelPHP のモデルキャッシュを削除する方法

Model_Crud のキャッシュをクリアするには、\Orm\Model の $_cached_objects を初期化してあげる必要があります。  

```php
class Model_Article extends \Orm\Model
{
    public static cc()
    {
        static::$_cached_objects[get_called_class()] = [];
    }
}
```

クエリビルダのほうは `delete()` なるメソッドがあるのでそれを使います。  

```php
$query = DB::query("SELECT * FROM articles")
            ->cached($cacheTtl, "articles.all", false)->execute();
Cache::delete("articles.all");
```

「DDL発行が頻繁に起きるDBに対してのバッチ用途」で FuelPHP のモデル作成は結構運用コストがかかっています。  
裏を返せば、「そんなにDDL発行が発生しないWebアプリケーションもしくはREST API用途」であれば、FuelPHP のORマッパーは結構優秀なのかな。  

## まとめ
最後は FuelPHP のORマッパーについての(超個人的な)愚痴でしたが、PHPのORマッパーについて時代の潮流を垣間見ることができたように思います。数字としてはっきり出た感じはしますね。時代は Laravel 。  
Ruby 界隈で Rails が覇道を行くように、いまは PHP on Laravel 。  
Laravel を使うならORマッパーは Eloquent を自然に使うべきだし、 Laravel を選べないなら Doctrine 。  
10年くらい前の PHP 界隈では Propel VS Doctrine だった気もするけど。たぶん10年後くらいには「昔 Laravel っていうフレームワークがあってだな」って言っているんでしょうね。  
さて、冒頭でも少し言いましたが、最近周りが Railsエンジニアばかりなので、「いやいや時代はPHPでしょ」みたいな気骨のあるPHPエンジニアな友達が欲しいです。そろそろコミュニティに参加したり、何某かのコミッターにでもならないといけないかなぁ。


