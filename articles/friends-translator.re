
= フレンズ語翻訳機を実装する

== フレンズ語とはどんなものか


フレンズが喋ってる独特の言い回しの言葉である。
辛い気持ちのときでも優しく包んでくれて癒やされるのである。
例を挙げると以下のようになる。おもしろーい。


=== フレンズ語の例
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


== 手法について考える


手始めにやりやすそうなところから考える。
フレンズ語の文法は先を度挙げた例を見ればわかるように文法が比較的かんたんである。
例えば「あなたは◯◯が得意なフレンズなんだね」などある程度テンプレート化すれば作れそうなものも存在する。
というわけでまずは簡単なテンプレートを作ってフレンズ語を生成してみようではないか。


== フレンズのテンプレートの生成手法


ここで使用するものは今話題の深層学習などではない。
まずは昔ながらの技術を使うことにしよう。
日本語の文法を解析しようと思えばやはりmecabとcabochaであろう。


=== cabochaとmecabを使用してフレンズのテンプレートを生成する


例えば次のような文章を翻訳するとしよう。


//emlist{
「私は歌うのが好きだ」
//}


テンプレートは以下のようなものを使うとする


//emlist{
「すごーい、君は◯◯フレンズなんだね」
//}


つまり翻訳後は


//emlist{
「すごーい、君は歌うのが好きなフレンズなんだね」
//}


となるべきである。



ぱっと見た感じ主語をぶった切って「◯◯」のところに入れるとそれっぽくなる感じがする。
とりあえずcabochaにかけてみようではないか。


//emlist{
$ echo "私は歌うのが好きだ" | cabocha
    私は---D
  歌うのが-D
      好きだ
//}


一番一のチャンクを無視すれば主語をなくせそうである。
Go言語で書くとこのようになる。
ちなみに自然言語処理をGoで書くのは珍しいかもしれないが私はGopherなのでGo言語を使う。
皆さんは好きな言語を使って実装すれば良いと思う。


//emlist{
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
//}


「私は歌うのが好きだ」において「歌うのが好きだ」が抽出できる。
しかし、このままでは「君は歌うのが好きだフレンズなんだね」となり文章的に違和感のあるものになってしまう。
mecabで文書構造を見てみよう


//emlist{
$ echo 私は歌うのが好きだ | mecab
私 名詞,代名詞,一般,*,*,*,私,ワタシ,ワタシ
は 助詞,係助詞,*,*,*,*,は,ハ,ワ
歌う  動詞,自立,*,*,五段・ワ行促音便,基本形,歌う,ウタウ,ウタウ
の 名詞,非自立,一般,*,*,*,の,ノ,ノ
が 助詞,格助詞,一般,*,*,*,が,ガ,ガ
好き  名詞,形容動詞語幹,*,*,*,*,好き,スキ,スキ
だ 助動詞,*,*,*,特殊・ダ,基本形,だ,ダ,ダ
EOS
zuikaku:friends-transl
//}


まず邪魔なセルリアン的な文字である「だ」は助動詞であるようだ。
一番最後の助動詞をぶった切ろう。
すると「歌うのが好き」という言葉が抽出できるようになる。
そして「すごーい、君は歌うのが好きフレンズなんだね」という言葉が生成出来るようになるが。
日本語が苦手なフレンズのような言葉になってしまうので一番最後に 「な」 を入れてあげる。
すると晴れて「すごーい、君は歌うのが好きなフレンズなんだね」という文書を生成できる。



ちなみに今回は「好き」が形容詞語幹なので「な」を挿入したが一般のときは「の」を入れてやるとそれっぽくなる。
コード化すると以下のようになる。


//emlist{
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
//}


以下は簡単な文書で実行した例である


//emlist{
$ go run main.go -i 私は歌うのが好きだ
すごーい。君は歌うのが好きなフレンズなんだね

$ go run main.go -i 君は泳げるんだね
すごーい。君は泳げるんフレンズなんだね。

$ go run main.go -i 涼宮ハルヒの憂鬱
すごーい。君は憂鬱のフレンズなんだね。

$ go run main.go -i 僕は友達が少ない
すごーい。君は友達が少ないフレンズなんだね。
//}


なんかそれっぽい文書が生成された。
しかし、まだ不十分である。入力された文書がポジティブな発言の場合は問題にならないが、
ネガティブな言葉を入力されると煽られている感じになる。
サーバルちゃんは煽ってこないのだ。


== 煽られないようにする


端的にいうとポジティブな発言のときに「すごーい」と言わせてそれ意外のときは「だいじょーぶ、フレンズには得意不得意があるから。」
と言ってもらえば良い。


=== 手法について


@<tt>{東北大学の乾・岡崎研究室が公開している日本語評価極性辞書} と言うものを利用してネガポジ判定を行う。
この辞書は用語の感情極性をネガティブ、ポジティブの2値で、名詞の感情極性をネガティブ、ポジティブ、ニュートラルで表している。
この辞書を利用してネガポジ判定を行う。


=== 実装する


形態素解析解析によって分割された単語から名詞の基本形と用言を取得する。


//emlist{
$ echo "私は歌うのが好き" | mecab
私 名詞,代名詞,一般,*,*,*,私,ワタシ,ワタシ
は 助詞,係助詞,*,*,*,*,は,ハ,ワ
歌う  動詞,自立,*,*,五段・ワ行促音便,基本形,歌う,ウタウ,ウタウ
の 名詞,非自立,一般,*,*,*,の,ノ,ノ
が 助詞,格助詞,一般,*,*,*,が,ガ,ガ
好き  名詞,形容動詞語幹,*,*,*,*,好き,スキ,スキ
//}


すると「私」、「歌う」、「の」、「好き」が取り出せる。
辞書には「好き」がポジティブなのでポジティブ側のパラメータを+1する。
これによって文書がポジティブであることを判定できる。


=== 実際の実装

//emlist{
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
//}

=== 実行結果

//emlist{
$ go run main.go -i 私は歌が好き
すごーい。君は歌が好きなフレンズなんだね

go run main.go -i 僕は運動が苦手だ
だいじょーぶ。フレンズには得意不得意があるからー。
//}
