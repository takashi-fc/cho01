package main

import (
	"fmt"

	"github.com/ieee0824/lex-rank/util"
)

func main() {
	ta := []string{
		"今日",
		"私",
		"は",
		"眠い",
	}
	tb := []string{
		"今日",
		"私",
		"は",
		"アニメ",
		"を",
		"見る",
		"のだ",
	}
	tc := []string{
		"すごーい",
		"君",
		"は",
		"考える",
		"の",
		"が",
		"得意",
		"な",
		"フレンズ",
		"なんだ",
		"ね",
	}

	fmt.Println(util.CalcCosSimil(ta, tb))

	fmt.Println(util.CalcCosSimil(ta, tc))

	fmt.Println(util.CalcCosSimil(tb, tc))
}
