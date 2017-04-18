
= 近くのフレンズを探すアプリを作ろう！


こんにちは、masao です。普段は Rails で社内システムを作ったり、iOSとAndroidのアプリ開発をしています。今回は出会い系アプリのような近くの人の写真のサムネイルが表示される、フレンズを探せるアプリを作っていきたいと思います。


== APIの準備

=== テーブルの用意


まずはアプリから叩く API にフレンズの位置情報を保存する DB を作成しておきます
今回はフレンズの座標(緯度経度)を Mysql の geometry型で保存します
　　


//emlist[][mysql]{
CREATE TABLE `members` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `geometry` geometry NOT NULL,
  PRIMARY KEY (`id`),
  SPATIAL KEY `geometry` (`geometry`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

//}


geometry型を取得する際にはバイナリが取得されるのでASTEXT() を使って文字列として取得します


//emlist[][mysql]{
mysql> SELECT id, ASTEXT(geometry) FROM members;
+----+---------------------------------------------+
| id | ASTEXT(geometry)                              |
+----+---------------------------------------------+
|  1 | POINT(132.132521 32.703595)  |
|  2 | POINT(113.151213 32.103456)  |
+----+---------------------------------------------+
//}

=== クエリの準備


続いては、自分の座標から自分以外の 1km 以内のフレンズを探すクエリを作ります使う関数は以下です@<br>{}
X = 経度, Y = 緯度@<br>{}
LINESTRING = POINTとPOINTを繋いだ線のことです@<br>{}
GEOMFROMTEXT = geometry型に変換できます@<br>{}
GLENGTH = LINESTRINGの長さを取得できます@<br>{}
以上を使ってこんな感じのクエリを作成しましょう　


//emlist[][mysql]{
Glength(
  GeomFromText(
    'LineString(132.132521 32.703595, 113.151213 32.103456)'
  )
)
//}


ただし、緯度経度はデータから取ってくるので CONCAT を使います


//emlist[][mysql]{
Glength(
  GeomFromText(
    CONCAT('LINESTRING(',
      X( self.geometry ),' ', Y( self.geometry ), ",",
      X( other.geometry ), ' ', Y( other.geometry ), ')'
    )
  )
//}


これで自分とフレンズの距離が取れました！次はこの距離を使って 1km 以内の距離に絞り込みましょう。ここで問題なのが、Glengthは度の値なので、1km を度に変えなければいけません。まず、１度が何kmなのかを求めます。地球の外周が およそ 40,075km なので、これを 360度で割ります。@<br>{}
40075 / 360 ≒ 111.319444@<br>{}
つまり、1度は 111.319444 km です。111.319444km のうち 1km は何度かを求めます。@<br>{}
1 / 111.319444 = 0.00898316@<br>{}
1km は Glength にすると約 0.009 なので、これをもとに 1km 以内のフレンズを探しましょう！


//emlist[][mysql]{
SELECT
  friends.id,
  GLENGTH(GEOMFROMTEXT(
    CONCAT('LINESTRING(',
      X( self.geometry ),' ',Y( self.geometry ), ",",
      X( friends.geometry ),' ',Y( friends.geometry ), ')'
    )
  )) AS distance
FROM
  members AS self
    INNER JOIN
  members AS friends
WHERE
  self.id = 1 AND self.id != friends.id
ORDER BY
  distance ASC
HAVING
  distance <= 0.009
//}


これで 1km 以内のフレンズが探せるようになったので API で叩いて取れるようにしておきましょう！(ry


== アプリの準備


今回は Swift でアプリを作っていきます。UICollectionView を使って周りにいる人たちの画像を表示しましょう！


=== StoryBoard


ViewController に CollectionView を設置していきます。CollectionViewCell に identifier を設定していきます。今回は memberCell にしました。



//image[masao1][storyboard][scale=0.7]{
//}



=== ViewController


ViewController で先ほど作った API を叩いてフレンズを取得します。
取得したフレンズ を CollectionView に設置していきます。


//emlist[][swift]{
import UIKit

class ViewController: UIViewController, UICollectionViewDataSource {

    private var nearlyFriends = Array()

    override func viewDidLoad() {
        super.viewDidLoad()

        // ここにAPIからフレンズを取得する処理(今回は省略)
        getNearlyFriends()
    }

    //フレンズの個数を返すメソッド
    func collectionView(_ collectionView: UICollectionView,
                        numberOfItemsInSection section: Int) -> Int
    {
        // APIから取得してきたフレンズの個数を返す
        return nearlyFriends.count
    }


    //フレンズを返すメソッド
    func collectionView(_ collectionView: UICollectionView,
                        cellForItemAt indexPath: IndexPath) -> UICollectionViewCell
    {
        //CollectionViewからセルを取得する。
        let cell = collectionView.dequeueReusableCell(
                                    withReuseIdentifier: "memberCell",
                                    for: indexPath) as UICollectionViewCell

        // APIから取得したフレンズから IndexPath 番目のデータを取得
        let friend = nearlyFriends[indexPath]

        // cell に取得したフレンズをもとに画像などを設定
        cell.backgroundView = friend.getImage()
    }
}
//}


collectionView で画像を設定したり、距離を表示したりしてカスタマイズしていきましょう。
