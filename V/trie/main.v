
[heap]
struct Node  {
	mut:
		letter rune
		end bool
		counter int
		children []&Node
}


struct Trie {
	root &Node
}

fn init_trie() Trie {
	return Trie{
		root: &Node{}
	}
}

fn (t Trie) insert(word string) {
	mut node := t.root
	for letter in word {
		mut found_in_child := false
	
		for mut child in node.children {
			if child.letter == letter {
				child.counter++
				node = child
				found_in_child = true
				break
			}
		}
		if !found_in_child {
			mut new_node := Node{ letter: letter }
			node.children << &new_node
			println("insert $new_node to $node children")
			node = &new_node
		}
	}
	node.end = true
	println("root $t")
}

fn (t Trie) find(word string) (bool, int) {
	mut node := t.root
	for letter in word {
		mut letter_not_found := true
		for child in node.children {
			if child.letter == letter {
				letter_not_found = false
				node = child
				break
			}
		}
		if letter_not_found {
			println("letter $letter not found")
			for child in t.root.children {
				println("child $child")
			}
			return false, 0
		}
	}
	return true, node.counter
}

fn main() {
	trie := init_trie()
	words := ["cat","dog", "catalog"]
	
	for i := 0; i < words.len; i++ {
		trie.insert(words[i])
	}
	words_to_find := ["cat", "dogg", "cata"]

	for i := 0; i < words_to_find.len; i++ {
		wf := words_to_find[i]
		found, _ := trie.find(wf)
		if found {
			println("word $wf found")
		} else {
			println("word $wf not found")
		}
	}
}