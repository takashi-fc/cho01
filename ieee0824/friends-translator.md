# フレンズ語翻訳機を実装する

## フレンズ語とは
フレンズが喋ってる独特の言い回しの言葉である。
辛い気持ちのときでも優しく包んでくれて癒やされるのである。
例を挙げると以下のようになる。おもしろーい。

### フレンズ語の例
* わーい
* たーしー
* まんぞく
* えっへん
* やったぁ
* すごいよー、これ、しんたいけん
* あなたは◯◯が得意なフレンズなんだね
* すっごーい
* はやく、はやくー
* わたしのせいじゃないよぉ
* そんな恐ろしいことしないよ
* でもまぁ騒ぐほどのことでもないか

## 手法について考える
手始めにやりやすそうなところから考える。
フレンズ語の文法は先を度挙げた例を見ればわかるように文法が比較的かんたんである。
例えば「あなたは◯◯が得意なフレンズなんだね」などある程度テンプレート化すれば作れそうなものも存在する。
というわけでまずは簡単なテンプレートを作ってフレンズ語を生成してみようではないか。

### フレンズのテンプレートの生成手法
ここで使用するものは今話題の深層学習などではない。
まずは昔ながらの技術を使うことにしよう。
日本語の文法を解析しようと思えばやはりmecabとcabochaであろう。

### cabochaとmecabを使用してフレンズのテンプレートを生成する
例えば次のような文章を翻訳するとしよう。

```
「私は歌うのが好きだ」
```

テンプレートは以下のようなものを使うとする

```
「すごーい、君は◯◯フレンズなんだね」
```

つまり翻訳後は

```
「すごーい、君は歌うのが好きなフレンズなんだね」
```
となるべきである。

ぱっと見た感じ主語をぶった切って「◯◯」のところに入れるとそれっぽくなる感じがする。
とりあえずcabochaにかけてみようではないか。

```
$ echo "私は歌うのが好きだ" | cabocha
    私は---D
  歌うのが-D
      好きだ
```

一番一のチャンクを無視すれば主語をなくせそうである。

## 実際に実装する
Go言語で書くとこのようになる。
ちなみに自然言語処理をGoで書くのは珍しいかもしれないが私はGopherなのでGo言語を使う。
皆さんは好きな言語を使って実装すれば良いと思う。

```
import (
	cabocha "github.com/ledyba/go-cabocha"
)

func TrimSubject(s string) (string, error) {
    var ret string
    c := cabocha.MakeCabocha()
    sentence, err := c.Parse(s)
    if err != nil {
        return "", err
    }
    for i, chunk := range sentence.Chunks {
		// 最初のチャンクをスキップする。また主語は名詞であることが多いので名詞で始まるときも条件に加える
        if i == 0 && chunk.Tokens[0].Features[0] == "名詞" {
            continue
        }

        }
        for _, token := range chunk.Tokens {
            ret += token.Body
        }
    }

    return ret, nil
}
```

「私は歌うのが好きだ」において「歌うのが好きだ」が抽出できる。
しかし、このままでは「君は歌うのが好きだフレンズなんだね」となり文章的に違和感のあるものになってしまう。
mecabで文書構造を見てみよう

```
$ echo 私は歌うのが好きだ | mecab
私	名詞,代名詞,一般,*,*,*,私,ワタシ,ワタシ
は	助詞,係助詞,*,*,*,*,は,ハ,ワ
歌う	動詞,自立,*,*,五段・ワ行促音便,基本形,歌う,ウタウ,ウタウ
の	名詞,非自立,一般,*,*,*,の,ノ,ノ
が	助詞,格助詞,一般,*,*,*,が,ガ,ガ
好き	名詞,形容動詞語幹,*,*,*,*,好き,スキ,スキ
だ	助動詞,*,*,*,特殊・ダ,基本形,だ,ダ,ダ
```

まず邪魔なセルリアン的な文字である「だ」は助動詞であるようだ。
一番最後の助動詞をぶった切ろう。
すると「歌うのが好き」という言葉が抽出できるようになる。
そして「すごーい、君は歌うのが好きフレンズなんだね」という言葉が生成出来るようになるが。
日本語が苦手なフレンズのような言葉になってしまうので一番最後に 「な」 を入れてあげる。
すると晴れて「すごーい、君は歌うのが好きなフレンズなんだね」という文書を生成できる。

ちなみに今回は「好き」が形容詞語幹なので「な」を挿入したが一般のときは「の」を入れてやるとそれっぽくなる。
コード化すると以下のようになる。

```
//第一引数活用させたもの
//第二引数未活用のもの
func ExtractCharacteristicWords(s string) (string, string, error) {
    nodes, err := mecab.Parse(s)
    if err != nil {
        return "", "", err
    }
    var ret = []string{}
    if err != nil {
        return "", "", err
    }
    for i := len(nodes) - 1; i >= 0; i-- {
        if nodes[i].Pos != "名詞" && nodes[i].Pos != "形容詞" {
            nodes = nodes[:i]
        } else {
            for _, node := range nodes {
                ret = append(ret, node.Surface)
            }

            if nodes[i].Pos1 == "形容動詞語幹" {
                ret = append(ret, "な")
            } else if nodes[i].Pos1 == "一般" {
                ret = append(ret, "の")
            } else if nodes[i].Pos == "形容詞" {
                ret[len(ret)-1] = nodes[i].Base
            }
            break
        }
    }

    if len(ret) == 0 {
        return "", "", errors.New("can not convert")
    }

    return strings.Join(ret, ""), strings.Join(ret[:len(ret)-1], ""), nil
}
```

以下は簡単な文書で実行した例である

```
$ go run main.go -i 私は歌うのが好きだ
すごーい。君は歌うのが好きなフレンズなんだね

$ go run main.go -i 君は泳げるんだね
すごーい。君は泳げるんフレンズなんだね。

$ go run main.go -i 涼宮ハルヒの憂鬱
すごーい。君は憂鬱のフレンズなんだね。

$ go run main.go -i 僕は友達が少ない
すごーい。君は友達が少ないフレンズなんだね。
```

なんかそれっぽい文書が生成された。
しかし、まだ不十分である。入力された文書がポジティブな発言の場合は問題にならないが、
ネガティブな言葉を入力されると煽られている感じになる。
サーバルちゃんは煽ってこないのだ。

## 煽られないようにする
端的にいうとポジティブな発言のときに「すごーい」と言わせてそれ意外のときは「だいじょーぶ、フレンズには得意不得意があるから。」
と言ってもらえば良い。

### 手法について
`東北大学の乾・岡崎研究室が公開している日本語評価極性辞書` と言うものを利用してネガポジ判定を行う。
この辞書は用語の感情極性をネガティブ、ポジティブの2値で、名詞の感情極性をネガティブ、ポジティブ、ニュートラルで表している。
この辞書を利用してネガポジ判定を行う。

### 実装する
形態素解析解析によって分割された単語から名詞の基本形と用言を取得する。

```
$ echo "私は歌うのが好き" | mecab
私	名詞,代名詞,一般,*,*,*,私,ワタシ,ワタシ
は	助詞,係助詞,*,*,*,*,は,ハ,ワ
歌う	動詞,自立,*,*,五段・ワ行促音便,基本形,歌う,ウタウ,ウタウ
の	名詞,非自立,一般,*,*,*,の,ノ,ノ
が	助詞,格助詞,一般,*,*,*,が,ガ,ガ
好き	名詞,形容動詞語幹,*,*,*,*,好き,スキ,スキ
```

すると「私」、「歌う」、「の」、「好き」が取り出せる。
辞書には「好き」がポジティブなのでポジティブ側のパラメータを+1する。
これによって文書がポジティブであることを判定できる。

### 実装

```
type NPElem struct {
	p int
	s string
}
var index = map[string][]NPElem{}

func readNPIndex(path string) error {
	var err error
	f, err := os.Open(path)
	if err != nil {
		return err
	}
	reader := bufio.NewReader(f)
	for line := ""; err == nil; line, err = reader.ReadString('\n') {
		line = strings.Replace(line, "\n", "", -1)
		i := strings.Split(line, "\t")
		if len(i) < 2 {
			continue
		}
		np := parseNP(i[0])
		words := strings.Split(i[1], " ")

		e := NPElem{np, i[1]}
		key := words[0]
		index[key] = append(index[key], e)
	}
	return nil
}

func CalcNP(s string) (int, error) {
	var result int
	nodes, err := mecab.Parse(s)
	if err != nil {
		return 0, err
	}
	for _, node := range nodes {
		elems, ok := index[node.Base]
		if !ok {
			continue
		}

		result += elems[0].p
	}
	return result, nil
}
```

### 実行結果

```
$ go run main.go -i 私は歌が好き
すごーい。君は歌が好きなフレンズなんだね

go run main.go -i 僕は運動が苦手だ
だいじょーぶ。フレンズには得意不得意があるからー。

$ go run main.go -i スホーイは空を飛ぶ
すごーい。君は空のフレンズなんだね。
```

### この手法の問題
辞書に載っていない単語がやってきた時対応しづらい。  
また、ネガティブな単語とポジティブな単語が同じバランスで入っている時きれいに判定ができない。


## さらなる精度を求めるための改良
ベイズ分類器を利用してネガポジを振り分ける。
ベイズ分類器について詳細に書くとページ数の関係で辛いことになりそうなのでこのあたりの記事を見ると良さそうです。

[ナイーブベイズ分類器を頑張って丁寧に解説してみる - Qiita](http://qiita.com/aflc/items/13fe52243c35d3b678b0)

なにに使われている技術かと言うと迷惑メールのフィルタとかに使われている技術です。
迷惑メールの場合は迷惑メールである、迷惑メールでないを判定しますがここではポジティブな文書である、ネガティブな文書である、どちらでもないを判定します。
Go言語で実現するためには `github.com/jbrukh/bayesian` というパッケージを利用するとよいです。

### 実装
まずClassと分類器を定義します。

```
// classの定義
const (
    P bayesian.Class = "Posi"
    N bayesian.Class = "Nega"
    E bayesian.Class = "Neither"
)

// 分類器を定義した構造体
type classifier struct {
    PN *bayesian.Classifier
    NE *bayesian.Classifier
    EP *bayesian.Classifier
}

// 分類器を初期化する
func newClassifier() *classifier {
    return &classifier{
        bayesian.NewClassifier(P, N),
        bayesian.NewClassifier(N, E),
        bayesian.NewClassifier(E, P),
    }   
}
```

判定関数を実装する
ベイズ分類器は分かち書きを作って食わせて上げる必要があるのでmecabを使って分かち書きをつくる。
PN, NE, EP判定機に分かち書きにした文書を入力するとscoreが取得できるのでP, N, Eをそれぞれ集計する。
一番0に近いものを結果として出力する。


```
func (c *classifier) DeliberationNP(s string) bayesian.Class {
    var doc = []string{}
    var (
        PScore = float64(1.0)
        NScore = float64(1.0)
        EScore = float64(1.0)
    )
    nodes, err := mecab.Parse(s)
    if err != nil {
        return ""
    }

    for _, node := range nodes {
        doc = append(doc, node.Base)
    }

    pnScores, _, pnb := c.PN.LogScores(doc)
    if pnb {
        PScore = PScore * (-1 * pnScores[0])
        NScore = NScore * (-1 * pnScores[1])
    }
    neScores, _, neb := c.NE.LogScores(doc)
    if nib {
        NScore = NScore * (-1 * niScores[0])
        EScore = EScore * (-1 * niScores[1])
    }
    epScores, _, epb := c.EP.LogScores(doc)
    if epb {
        EScore = EScore * (-1 * epScores[0])
        PScore = PScore * (-1 * epScores[1])
    }

    if PScore < NScore {
        if PScore < EScore {
            return P
        }
    } else {
        if NScore < EScore {
            return N
        }
    }
    return E
}
```

学習関数は以下のようにする


```
func learn(s string, which bayesian.Class, c *classifier) *classifier {
    var doc = []string{}
    nodes, err := mecab.Parse(s)
    if err != nil {
        return c
    }

    for _, node := range nodes {
        doc = append(doc, node.Base)
    }
    c.Learn(doc, which)
    return c
}

func (c *classifier) Learn(document []string, which bayesian.Class) {
    if which == P {
        c.PN.Learn(document, which)
        c.EP.Learn(document, which)
    } else if which == N {
        c.NE.Learn(document, which)
        c.PN.Learn(document, which)
    } else if which == E {
        c.EP.Learn(document, which)
        c.NE.Learn(document, which)
    }
}
```

分類器を３つに分割することで全ての学習データが揃わなくても分類することができる。

### 実行する

```
$ go run nb/main.go -i 今日は凍え死にそうだ
Nega

$ go run nb/main.go -i あなたは何もできないフレンズなのね
Nega

$ go run nb/main.go -i 残業辛い
Nega

$ go run nb/main.go -i かばんちゃんはすっごいんだよ！
Posi

$ go run nb/main.go -i 当たり前じゃない！かばんちゃんを助けて、また色んなところ行くんだから！
Posi

$ go run nb/main.go -i スホーイは空をとぶのが好きなフレンズなんだね。
Posi

$ go run nb/main.go -i 食べないよ！
Nega

$ go run nb/main.go -i いきるのがつらい
Nega
```

## さいごに
今回はテンプレートに単語を埋め込んでフレンズ語を生成しました。
文書のタグ付け技術とか深層学習をつかってフレンズ語を生成できると面白いなと思っています。
もし第二弾がかけるようであれば作ってみたいなと思っています。

## 参考文献
* [ナイーブベイズ分類器を頑張って丁寧に解説してみる - Qiita](http://qiita.com/aflc/items/13fe52243c35d3b678b0)
* [ネガポジ判定を行うGem作ってみた - Qiita](http://qiita.com/moroku0519/items/e6352d31311418f38227)
* [日本語評価極性辞書 - 乾・岡崎研究室 - Tohoku University](http://www.cl.ecei.tohoku.ac.jp/index.php?Open%20Resources%2FJapanese%20Sentiment%20Polarity%20Dictionary)

## 使用ライブラリ
* [github.com/ledyba/go-cabocha](https://github.com/ledyba/go-cabocha)
* [github.com/yukihir0/mecab-go](https://github.com/yukihir0/mecab-go)
* [github.com/jbrukh/bayesian](https://github.com/jbrukh/bayesian)

## つくったもの
* [https://github.com/ieee0824/friends-translator](https://github.com/ieee0824/friends-translator)
