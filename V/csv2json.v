module main

import os

fn main() {
	if os.args.len < 2 {
		prog_name := os.args[0]
		eprintln('$prog_name filename')
		exit(1)
	}
	filename := os.args[1]
	data := os.read_lines(filename) or {
		eprintln('cannot read file $filename')
		exit(1)
	}
	mut row_count := 0
	mut headers := []string{}
	mut tag := ''
	mut row := []string{}
	for ix := 0; ix < data.len; ix++ {
		row = split_csv(data[row_count])
		// println('row: $row_count $row')
		row_count++
		if row_count == 1 {
			headers = row
			print('[ ')
			// println('headers: $headers')
			continue
		} else if row_count > 2 {
			print(',')
		}
		print('{ ')
		for jx, val in row {
			tag = headers[jx]
			if jx > 0 {
				print(', ')
			}
			if val in ['true', 'false'] {
				print(' "$tag" : $val ')
			} else {
				if is_number(val) {
					print(' "$tag" : $val ')
					continue
				}
				print(' "$tag" : "$val" ')
			}
		}
		print('}')
	}
	println(']')
}

fn is_number(s string) bool {
	for _, c in s {
		if c == 46 {
			continue // dot
		}
		if c >= 48 && c <= 57 {
			continue // 0..9
		}
		return false
	}
	return true
}

fn split_csv(s string) []string {
	mut w := ''
	mut double_quote := false
	mut words := []string{}
	for _, c in s {
		if c == 34 { // double quote
			if double_quote {
				double_quote = false
				continue
			}
			double_quote = true
			continue
		}
		if double_quote || c != 44 { // comma
			w += c.str()
			continue
		}
		words << w
		w = ''
	}
	return words
}
