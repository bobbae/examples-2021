extern crate radix_trie;

use radix_trie::{Trie};
    
const WORDLIST1: [ &'static str ; 11] = [
    "cat",
    "dog",
    "cats",
    "catsdogcats",
    "catxdogcatsrat",
    "dogcatsdog",
    "hippopotamuses",
    "rat",
    "ratcat",
    "ratcatdog",
    "ratcatdogcat"
];

fn main() {
    let mut trie = Trie::new();
    let mut ind = 1;

    for &word in &WORDLIST1 {
        trie.insert(word, ind);
        ind += 1;
        assert!(trie.check_integrity());
    }

    assert!(trie.get(&"nonexistent").is_none());
    assert!(trie.get(&"catsdogcats").is_some());

    println!("Hello, world!");
}
