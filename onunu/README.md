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

## 1. 連番を生成する
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
mysql> SET @i = 0;
Query OK, 0 rows affected (0.00 sec)

mysql> SELECT
    ->   @i := @i + 1 AS increment,
    ->   num
    -> FROM
    ->   random_nums
    -> ORDER BY
    ->   num
    -> ;
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


## 2. 座標平面
グラフを描画するにはまず画面がなくてはいけません。
先ほど作った連番を利用して、座標平面を作ってみます。
21のインクリメントが必要なので、適当に21のレコードを持つテーブルを生成します。

```
mysql> CREATE TABLE increments (id INT, num INT);
Query OK, 0 rows affected (0.03 sec)

mysql> SET @i = 0;
Query OK, 0 rows affected (0.00 sec)

mysql> INSERT INTO increments (id, num) SELECT @i := @i + 1 AS id, FLOOR(RAND() * 100) AS num FROM hoge LIMIT 21;
Query OK, 21 rows affected (0.00 sec)
Records: 21  Duplicates: 0  Warnings: 0
```
※ hogeは21行ある適当なテーブルです。
ではこのincrementsテーブルを利用して座標平面を描いていきます。

```
mysql> SELECT
    ->   (i.id - 11) / 10 AS x_axis,
    ->   (j.id - 11) / 10 AS y_axis
    -> FROM
    ->   increments AS i,
    ->   increments AS j
    -> ORDER BY
    ->   x_axis,
    ->   y_axis
    -> ;

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
こんな感じで、(-1.0, -1.0)から(1, 1)を0.1刻みで分割した(x, y)の組み合わせを生成することができました。
あとはGROUP_CONCAT関数を使って結合してあげれば完成です。
実際にやってみます。

```
mysql> SELECT
    ->   GROUP_CONCAT(
    ->     value
    ->     ORDER BY x_axis, y_axis DESC
    ->     SEPARATOR ''
    ->   ) AS 'Coordinate plane'
    -> FROM (
    ->   SELECT
    ->     grid.x_axis,
    ->     grid.y_axis,
    ->     CASE
    ->       WHEN grid.x_axis = 0    AND y_axis = 0
    ->         THEN '-+-'
    ->       WHEN grid.x_axis = -0.1 AND y_axis = -0.1
    ->         THEN ' O '
    ->       WHEN grid.x_axis = 0.1  AND y_axis = -1
    ->         THEN ' y '
    ->       WHEN grid.x_axis = 1    AND y_axis = -0.1
    ->         THEN ' x '
    ->       WHEN grid.x_axis = 0
    ->         THEN ' | '
    ->       WHEN grid.y_axis = 0
    ->         THEN '---'
    ->       ELSE '   '
    ->     END AS value
    ->   FROM (
    ->     SELECT
    ->       (i.id - 11) / 10 AS x_axis,
    ->       (j.id - 11) / 10 AS y_axis
    ->     FROM
    ->       increments AS i,
    ->       increments AS j
    ->     ORDER BY
    ->       x_axis,
    ->       y_axis
    ->   ) AS grid
    ->   ORDER BY
    ->     grid.x_axis,
    ->     grid.y_axis
    -> ) AS vlues,
    -> (
    ->   SELECT @cel_size := 10
    -> ) AS cel_size
    -> GROUP BY
    ->   y_axis
    -> ;
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
