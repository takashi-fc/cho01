package main

import (
	"flag"
	"fmt"

	cabocha "github.com/ledyba/go-cabocha"
	mecab "github.com/yukihir0/mecab-go"
)

var (
	input = flag.String("i", "", "")
)

func init() {
	flag.Parse()
}

func PosiCon(s string) string {
	return fmt.Sprintf("すごーい。君は%sフレンズなんだね。\n", s)
}

func TrimSubject(s string) (string, error) {
	var ret string
	c := cabocha.MakeCabocha()
	sentence, err := c.Parse(s)
	if err != nil {
		return "", err
	}
	for i, chunk := range sentence.Chunks {
		if i == 0 && chunk.Tokens[0].Features[0] == "名詞" {
			continue
		}
		for _, token := range chunk.Tokens {
			ret += token.Body
		}
	}
	return ret, nil
}

func Trim(s string) (string, error) {
	nodes, err := mecab.Parse(s)
	if err != nil {
		return "", err
	}
	var ret string
	if err != nil {
		return "", err
	}
	for i := len(nodes) - 1; i >= 0; i-- {
		if nodes[i].Pos != "名詞" && nodes[i].Pos != "形容詞" {
			nodes = nodes[:i]
		} else {
			for _, node := range nodes {
				ret += node.Surface
			}

			if nodes[i].Pos1 == "形容動詞語幹" {
				ret += "な"
			} else if nodes[i].Pos1 == "一般" {
				ret += "の"
			}
			break
		}
	}

	return ret, nil
}

func main() {
	s, _ := TrimSubject(*input)
	result, _ := Trim(s)
	fmt.Println(PosiCon(result))
}
