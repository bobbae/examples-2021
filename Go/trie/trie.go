package trie

// most code  from  https://github.com/dghubble/trie

type Trie struct {
	value    interface{}
	children map[rune]*Trie
}

func NewTrie() *Trie {
	return new(Trie)
}

func (trie *Trie) Get(key string) interface{} {
	node := trie
	for _, r := range key {
		node = node.children[r]
		if node == nil {
			return nil
		}
	}
	return node.value
}

func (trie *Trie) Put(key string, value interface{}) bool {
	node := trie
	for _, r := range key {
		child, _ := node.children[r]
		if child == nil {
			if node.children == nil {
				node.children = map[rune]*Trie{}
			}
			child = new(Trie)
			node.children[r] = child
		}
		node = child
	}
	isNewVal := node.value == nil
	node.value = value
	return isNewVal
}

func (trie *Trie) Delete(key string) bool {
	path := make([]nodeRune, len(key))
	node := trie
	for i, r := range key {
		path[i] = nodeRune{r: r, node: node}
		node = node.children[r]
		if node == nil {
			return false
		}
	}
	node.value = nil
	if node.isLeaf() {
		for i := len(key) - 1; i >= 0; i-- {
			parent := path[i].node
			r := path[i].r
			delete(parent.children, r)
			if !parent.isLeaf() {
				break
			}
			parent.children = nil
			if parent.value != nil {
				break
			}
		}
	}
	return true
}

type WalkFunc func(key string, value interface{}) error

func (trie *Trie) Walk(walker WalkFunc) error {
	return trie.walk("", walker)
}

func (trie *Trie) WalkPath(key string, walker WalkFunc) error {
	if trie.value != nil {
		if err := walker("", trie.value); err != nil {
			return err
		}
	}

	for i, r := range key {
		if trie = trie.children[r]; trie == nil {
			return nil
		}
		if trie.value != nil {
			if err := walker(string(key[0:i+1]), trie.value); err != nil {
				return err
			}
		}
	}
	return nil
}

type nodeRune struct {
	node *Trie
	r    rune
}

func (trie *Trie) walk(key string, walker WalkFunc) error {
	if trie.value != nil {
		if err := walker(key, trie.value); err != nil {
			return err
		}
	}
	for r, child := range trie.children {
		if err := child.walk(key+string(r), walker); err != nil {
			return err
		}
	}
	return nil
}

func (trie *Trie) isLeaf() bool {
	return len(trie.children) == 0
}
