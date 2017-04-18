
= TableViewを好きになってもScrollViewは嫌いにならないでください

== はじめに


みなさんこんにちは。iOSアプリでスクロールする画面を作るとき、皆さんはどうやって作成しているでしょうか？TableViewControllerやPageViewControllerを使ってやられる方結構いると思います。TableViewControllerであれば複雑なレイアウトもセルごとに分割できるのでわかりやすくできますよね。
私はこれまでScrollViewを使った実装もしてきました。ただ、ScrollViewの特性を良く理解しないまま使っていたので結構詰まることもあり、結局TabaleViewを使うなどしてきました。
しかし、スクロールさせたいならScrollViewでもいいじゃないですか！私がScrollViewについて詳しくないから使いこなせていないだけだ、そうにちがいない、もっとScrollViewを好きになろうという決意でこの記事を書いています。笑
ScrollViewが苦手な人も基本をおさらいしてみましょう。


== ScrollViewの基本


ScrollViewは二つのsizeを持っています。
たとえながら説明すると、一つは窓枠の役割を担うframe、もう一つは景色の役割を担うcontentSizeです。
frameは他のオブジェクトも持つプロパティで、オブジェクトの見た目の大きさです。contentSizeは表示コンテンツの大きさを示すもので、ScrollViewの上に乗っているViewの大きさになります。frameという窓から見える景色の大きさがcontentSizeと覚えるとわかりやすいですね。大きは横・縦どちらの方向にもcontentSizeとして設定できるので横・縦どちらのスクロールも設定次第でできます。


== 使ってみてわかるもどかしさ


以下のコードでは二つの画像に挟まれたtextViewを用意しています。
//image[sampl1][ねこ][scale=0.5]{
//}



//emlist[][:サンプル1]{
@IBOutlet weak var text1: UILabel!

override func viewDidLoad() {
    super.viewDidLoad()
    text1.translatesAutoresizingMaskIntoConstraints = true
    text1.text = "ねこ"
    text1.sizeToFit()
}
//}


この状態でtext1.textをこのようにしてみます


//emlist[][swift:テキスト変更]{
text1.text = "ねこねこねこねこねこねこねこねこねこねこね..."
//}


こうするとどうなるかというと



//image[sample2][ねこ２][scale=0.5]{
//}




こうなるんですが、このままでは @<strong>{スクロールができません。}
なぜかというとScrollViewに乗っているContentViewの高さが変動していないからです。ContentViewの高さを変動させContentSizeを変更させるために、以下の処理を追加します


//emlist{
contentViewConstHeight.constant += 調整したい高さ;
//}


これでContentViewの大きさは調整されるのでスクロールすることができます。
ちなみに、contentSizeを調整するにはScrollViewがもつcontentSizeプロパティからもできます。


//emlist{
ScrollView.contentSize += 調整したい高さ;
//}


こんな感じで簡単な仕組みですが、複雑なデザインを調整するとAutoLayoutでやる場合は使いにくかったりします。


== 忘れがちなエラー


私自身やってしまう失敗があります。


//emlist[][:よく出るエラー]{
Scroll View
Has ambiguous scrollable content width

Scroll View
Has ambiguous scrollable content height

Ambiguous Layout
Scrollable content size is ambiguous for "Scroll View"
//}


だいたいこれらはAutoLayoutでScrollViewを配置して上にviewを乗せたときに発生します。要は、width・heightが不明、つまりContentViewの大きさがわからないというエラーですね。
手順項目としては以下のとおり

 * ScrollViewを配置
 * ScrollViewのwidth・heightを設定
 * ContentViewを配置



ScrollViewの制約は以下のようになります。
//image[scroll_design][scrollView_AutLayoutの設定]{
//}




ContentViewの制約は以下のようになります。
//image[content_design][contentView_AutLayoutの設定]{
//}




ScronnViewのwidth・heightが決まらないとContentViewをおいてもエラーが出てしまうので気を付けましょう。


== ページ単位のスクロール


ちなみにScrollViewに画面ごとのViewを配置して複数画面をスクロールできるように設定することもできますが、ページ単位のスクロールであればUIPageViewControllerを使うと良いかもしれません。ページ単位のViewをまとめて管理してくれます。


//emlist[][:UIPageViewController]{
https://developer.apple.com/reference/uikit/uipageviewcontroller
//}


また無限スクロールできるライブラリもあるようです。使ってみてはいかがでしょう。


//emlist[][:無限スクロールできるUIPageViewControllerを使ったライブラリ]{
EndouMari/TabPageViewController
//}

== おわりに


いかがでしたでしょうか、ScrollView好きになりましたか？私は...やっぱりTableViewの方が使いやすいかなと思ってしまいました。笑
複雑なデザインを構成するとき、追加で表示させるパーツや高さ調整がややこしくなったりしてしまいますね。こういうときはTableViewを使ってセル単位でデザインを使用した方が扱いやすいですね。
そういった点からScrollViewはシンプルな画面のスクロールで使うぶんには良いかなと思います。常にViewをシンプルなものにできれば良いですが...。
ただ、今回はScrollViewwをおさらいしたのでもう基本は大丈夫ですね。みなさんScrollViewを嫌わずに使ってみましょう！
