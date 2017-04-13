# ご挨拶
こんにちは。 @boscoworks です。普段は目黒あたりでPHPエンジニアをやっています。  
最近はRuby on RailsでのWeb開発が主流になってきましたね。仕事柄、社外のエンジニアの方ともよくお会いするのですが、Railsをやっているエンジニアはとても多い印象です。  
私の周りでもたまに「*PHPはオワコン*」という言葉が飛び交っていて「そろそろエンジニアから足を洗うときが近づいているかな」などと冗談めかして言ったりしています。~~来年あたり本当にやめているかもしれない。~~  
さて、とはいえ私自身も私の周辺もまだまだPHPは活況、PHP+MySQLは未だWeb開発の王道を行っていると断言して差し支えないでしょう。  
ただPHPでのWeb開発をやっていて（個人的に）いつも悩むのがORマッパーの選定です。ぐるぐる悩んだ結果「ええいもうPDOで良くないかコレ！」と叫んだこともしばしば。  
結局フレームワークに標準で入ってるORマッパーを使うのが定石なんですけどね、なんかこうモヤモヤしませんか？しますよね？  
ということで、最近のPHPのORマッパーをあれこれ調べることにしてみましたよ。  

# 調査対象
独断と偏見で以下をピックアップしました。  

## 1. Propel
 - 一時代前のSymfonyが標準ORMとして導入していたやつ。最近はあんまり聞かない。
     - http://propelorm.org/
     - https://github.com/propelorm/Propel2
     - https://packagist.org/packages/propel/propel
## 2. Doctrine
 - 最近のSymfonyが標準ORMとして導入しているやつ。ザ・PHPのORM感ある。
     - http://www.doctrine-project.org/
     - https://github.com/doctrine/doctrine2
     - https://packagist.org/packages/doctrine/orm
## 3. Eloquent
 - LaravelのORM。こいつが有名というよりは、Lavavelが急成長した結果、相対的に使われるようになった的な。
     - https://laravel.com/docs/5.4/eloquent
     - https://github.com/illuminate/database
     - https://github.com/laravel/framework
     - https://packagist.org/packages/illuminate/database
     - https://packagist.org/packages/laravel/framework
## 4. Idiorm & Paris
 - 「PHP ORマッパー」でググると上の方に出てくるやつ。
     - http://j4mie.github.io/idiormandparis/
     - https://github.com/j4mie/idiormandparis

# 調査方法
3つの視点から「どのORマッパーが人気なのか」を探ります。

+ 「Googleトレンド ( https://trends.google.co.jp/trends/ )」による人気度の動向調査
+ GithubのStar数、Fork数
+ Packagist経由でのcomposer install数

## Googleトレンドによる人気度の動向調査
 - https://trends.google.co.jp/trends/explore?q=propel%20php,doctrine%20php,eloquent%20php,idiorm%20php
（ここに画像を貼りたい）

Doctrineはさすが王者の風格、1位を譲らないですね。  
注目はEloquent、2013年末あたりでPropelを追い越してからも順調に右肩上がり。いまやDoctrineに並ぶ勢いですね。  
Idiorm...( ＾ω＾)  

これ結局SymfonyとLaravelのシェア争いとイコールになるんじゃないかと思い調べてみると。  
（ここに画像を貼りたい）  
Symfonyはシェアを落とすことなく、ただLaravelが怒涛の右肩上がり。Laravel人気すごいですね。  

## GithubのStar数、Fork数
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

## Packagist経由でのcomposer install数
|ORM|Install数|
|---|---:|
|Propel|295,535|
|Doctrine|21,048,164|
|Eloquent(illuminate/database)|2,262,532|
|Eloquent(laravel/framework)|20,991,608|
|Idiorm|194,306|

(※3/28調べ)

こうやってみるとDoctrine健闘しているんですね。Laravel本体といい勝負。

