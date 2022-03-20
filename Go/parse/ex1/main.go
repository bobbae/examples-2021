package main

import (
	"fmt"
	"regexp"
)

type Metric struct {
	Desc string
	Vals []int
}

func main() {
	words := make([]string, 10)
	words[0] = "this"
	words[1] = "metric"
	words[2] = "is"
	words[3] = "44"
	words[4] = "and"
	words[5] = "32"
	words[6] = "plus" 
	words[7] = "furthermore"
	words[8] = "100"
	words[9] = ""
	
	content := make([]Metric,0)
	var metric Metric
	metricStr := ""
	var numStr string
	var num int
	nums := make([]int,1)
	r := regexp.MustCompile("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$")
	for _,w := range words {
		numStr =  r.FindString(w)
		if numStr != "" {
			metric.Desc = metricStr
			fmt.Sscanf(numStr,"%d",&num)
			nums[0] = num
			metric.Vals = nums
			metricStr = ""
			fmt.Println(metric)
			content = append(content, metric)
		} else {
			metricStr += " " + w
		}
	}
	fmt.Println(content)
}
