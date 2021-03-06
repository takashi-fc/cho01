# MySQLでいろいろなグラフを書こう！
## はじめに
MySQLの出力結果をグラフにして表示するには手間がかかります。  
GUIツールを使うこともあれば、結果をCSVに出力してエクセルなどで操作することもあるでしょう。

しかし、我々には標準出力という強い味方がいます。

SSHで接続した時、エクセルを開くのが面倒なとき、  
様々な場合であっても標準出力であれば気軽に結果を確認することができます。

今回は、MySQLの標準出力のみでリッチなグラフを描画する方法について考えていきたいと思います。  
MySQLでグラフをかければ、面倒なエクセル操作とおさらばできます！やったー！

### 環境
あまり難しいことを考えず、HomebrewでインストールしたMySQLを利用します。  
versionについては以下の通りです。

```
$ brew info mysql
mysql: stable 5.7.17 (bottled)
```

## 連番を生成する
レコードの内容に対して、連番を生成するところから始めます。  
グラフとか関係なく、日付順にデータを並べたりする際などに便利です。

MySQLに制御構文はありませんが、SELECT句によるレコード一件を1回のループとしてみれば、  
変数をインクリメントさせることができます。

説明用に以下のようなテーブルを用意しました。

```
mysql> SELECT * FROM random_nums;
+-------+
| num   |
+-------+
|   122 |
| 13123 |
|   189 |
|  1213 |
|     3 |
| 79879 |
|  3169 |
|   791 |
|  9810 |
|   114 |
+-------+
10 rows in set (0.00 sec)
```

連番を生成しつつ、ランダムな整数を小さい順に並べたいと思います。

```
SET @i = 0;

SELECT
  @i := @i + 1 AS increment,
  num
FROM
  random_nums
ORDER BY
  num
;
```

結果は以下のようになります。

```
+-----------+-------+
| increment | num   |
+-----------+-------+
|         1 |     3 |
|         2 |   114 |
|         3 |   122 |
|         4 |   189 |
|         5 |   791 |
|         6 |  1213 |
|         7 |  3169 |
|         8 |  9810 |
|         9 | 13123 |
|        10 | 79879 |
+-----------+-------+
10 rows in set (0.00 sec)
```

連番を生成することができました。  
ここで注意しなくてはならないのは、インクリメントの回数の上限はテーブルのレコード数に依存するということです。  
より大きな数でインクリメントを生成した場合は、それ相応の大きさをのテーブルを用意する必要があります。


## 座標平面
グラフを描画するにはまず画面がなくてはいけません。  
先ほど作った連番を利用して、座標平面を作ってみます。  
21のインクリメントが必要なので、適当に21のレコードを持つテーブルを生成します。

```
CREATE TABLE increments (id INT, num INT);

SET @i = 0;

INSERT INTO increments (id, num)
SELECT
  @i := @i + 1 AS id,
  FLOOR(RAND() * 100) AS num
FROM
  hoge
LIMIT
  21
;
```

※ hogeは21行ある適当なテーブルです。
ではこのincrementsテーブルを利用して座標平面を描いていきます。

```
SELECT
  (i.id - 11) / 10 AS x_axis,
  (j.id - 11) / 10 AS y_axis
FROM
  increments AS i,
  increments AS j
ORDER BY
  x_axis,
  y_axis
;
```

結果は以下のようになります。

```
+---------+---------+
| x_axis  | y_axis  |
+---------+---------+
| -1.0000 | -1.0000 |
| -1.0000 | -0.9000 |
| -1.0000 | -0.8000 |
| -1.0000 | -0.7000 |
.....................
|  1.0000 |  0.9000 |
|  1.0000 |  1.0000 |
+---------+---------+
441 rows in set (0.00 sec)
```
このように、(-1.0, -1.0)から(1, 1)を0.1刻みで分割した(x, y)の組み合わせを生成することができました。
あとはGROUP_CONCAT関数を使って結合してあげれば完成です。
実際にやってみます。

```
SELECT
  GROUP_CONCAT(
    value
    ORDER BY x_axis, y_axis DESC
    SEPARATOR ''
  ) AS 'Coordinate plane'
FROM (
  SELECT
    grid.x_axis,
    grid.y_axis,
    CASE
      WHEN grid.x_axis = 0    AND y_axis = 0
        THEN '-+-'
      WHEN grid.x_axis = -0.1 AND y_axis = -0.1
        THEN ' O '
      WHEN grid.x_axis = 0.1  AND y_axis = -1
        THEN ' y '
      WHEN grid.x_axis = 1    AND y_axis = -0.1
        THEN ' x '
      WHEN grid.x_axis = 0
        THEN ' | '
      WHEN grid.y_axis = 0
        THEN '---'
      ELSE '   '
    END AS value
  FROM (
    SELECT
      (i.id - 11) / 10 AS x_axis,
      (j.id - 11) / 10 AS y_axis
    FROM
      increments AS i,
      increments AS j
    ORDER BY
      x_axis,
      y_axis
  ) AS grid
  ORDER BY
    grid.x_axis,
    grid.y_axis
) AS vlues,
(
  SELECT @cel_size := 10
) AS cel_size
GROUP BY
  y_axis
;
```

クエリの実行結果は以下のようになります。
綺麗に座標平面を描くことができました。
  
  
  
  
  
  
  

```
+-----------------------------------------------------------------+
| Coordinate plane                                                |
+-----------------------------------------------------------------+
|                                |  y                             |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                             O  |                             x  |
| -------------------------------+------------------------------- |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
|                                |                                |
+-----------------------------------------------------------------+
21 rows in set (0.00 sec)
```

## 棒グラフを作ろう
では、先ほど作成した座標平面を利用して、棒グラフを作ってみます。  
基本的な作成方法としては、先ほどの座標平面の各(x, y)において、棒グラフが描画されるかどうかを判断すればOKです。

例として、都道府県の人口トップ4をピックアップしました  
(ex. wikipedia)

```
mysql> SELECT
    ->   *
    -> FROM
    ->   prefecture_population
    -> LIMIT
    ->   4
    -> ;
+----+--------------+------------+
| id | name         | population |
+----+--------------+------------+
|  1 | 東京都       |   13515271 |
|  2 | 神奈川県     |    9126214 |
|  3 | 大阪府       |    8839469 |
|  4 | 愛知県       |    7483128 |
+----+--------------+------------+
4 rows in set (0.00 sec)
```

先ほどの座標平面の各(x, y)について、描画するvalueを割り当てていきます。  
クエリは以下のようになりました。

```
SELECT
  GROUP_CONCAT(
    value
    ORDER BY x_axis
    SEPARATOR ''
  ) AS '主要4都府県の人口'
FROM (
  SELECT
    x_axis,
    y_axis * -1 AS yy_axis,
    MAX(x_range),
    name,
    rate,
    CASE
      WHEN x_range = x_axis
        THEN '   '
      WHEN rate > y_axis
        THEN '◽◽'
      ELSE '   '
    END AS value
  FROM (
    SELECT
      x_axis,
      y_axis,
      (id - 2) / 2 AS x_range,
      name,
      population,
      @max := (SELECT MAX(population) FROM prefecture_population) AS max,
      ROUND(population / @max, 1) AS rate
    FROM (
      SELECT
        (i.id - 11) / 10 AS x_axis,
        (j.id - 11) / 10 AS y_axis
      FROM
        increments AS i,
        increments AS j
      ORDER BY
        x_axis,
        y_axis
    ) AS grid, (
      SELECT
        id,
        name,
        population
      FROM
        prefecture_population
      ORDER BY
        population DESC
      LIMIT 4
    ) AS populations
  ) AS tmp_values
  WHERE
    x_axis <= x_range
  GROUP BY
    x_axis,
    yy_axis
) AS graph_value,
(
  SELECT @cel_size := 10
) AS cel_size
GROUP BY yy_axis
;
```

実行した結果は以下の通りです。
文字コードの影響と紙面の都合で、画像ですが一般的なシェルであれば問題なく表示できるはずです。

![棒グラフ](images/graph.png, "棒グラフ")

## おわりに
SQLと標準出力で、グラフィカルな棒グラフを作成することができました。  
紙面の都合上カットしましたが、MySQLに用意されている三角関数系のメソッドを使うことで、  
円グラフを作成することも可能です。

また、MySQLは8系でWITH句がサポートされたことで、クエリの可読性が担保されただけでなく、  
再帰的にテーブルを作成することも可能になりました。  
非常にニッチかつ役に立たないジャンルではありますが、今後もより精度の高い作図を目指していきたいと思います。
ここまで読んでくださり、ありがとうございました！
