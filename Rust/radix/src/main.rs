extern crate radix_trie;

use radix_trie::{Trie, TrieCommon, TrieKey};
use clap::{Arg, App};
use std::fs::File;
use std::io::{self, prelude::*, BufRead, BufReader};
use std::path::Path;
use std::collections::HashMap;

const WORDLIST1: [&'static str; 11] = [
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
    "ratcatdogcat",
];


fn main() {
    let matches = App::new("Find longest compound word")
        .version("0.1.0")
        .author("Unknown author")
        .about("Find the longest compound word from a list")
        .arg(Arg::with_name("file")
            .short("f")
            .long("file")
            .value_name("FILE")
            .takes_value(true)
            .help("A file containing words"))
        .get_matches();

    let filename = matches.value_of("file").unwrap_or("words.txt");
    if !Path::new(filename).exists() {
        panic!("File {} does not exist.", filename);
    }
    println!("using file {} as input", filename);
    test_hashmap(filename);
}

fn test_hashmap(filename: &str) ->io::Result<()> {
    let mut hm = HashMap::new();
    let file = File::open(filename)?;
    let reader = BufReader::new(file);

    let mut ind = 1;
    for line in reader.lines() {
        hm.insert(line.unwrap(), ind);
        ind += 1;
    }
    println!("{} words", hm.len());
    let mut compound_words = vec![];
    for (k, v) in  hm.iter() {
        if compounded(&hm, k, 1) {
            compound_words.push(k);
        }
    }
    compound_words.sort_by(|a,b| b.len().cmp(&a.len()));
    println!("Compound words are: {:?}", compound_words);

    Ok(())
}

fn compounded(hm: &HashMap<String, i32>, word : &String, level: i32) -> bool {
    for i in 1..word.len() {
        let prefix : String = word.chars().take(i).collect();
        if !hm.contains_key(&prefix) {
            continue;
        }
        //println!("prefix {}", &prefix);
        let rest : String = word.chars().skip(i).collect();
        //println!("rest {}", &rest);
        if compounded(hm, &rest, level + 1) {
           return true;
        }
    }
    if hm.contains_key(word) && level > 1 {
        return true;
    }
    false
}

fn test_radix(filename: &str) {
    //let  trie = build_trie();

    let trie = build_trie_from_file(filename);

    let mut compound_words = vec![];

    for (k, v) in trie.iter() {
        let mut word = String::new();
        for ch in k.chars() {
            word.push(ch);
        }
        //println!("=====> {}", &word);
        
        if is_compound(&trie, &word, 1) {
            compound_words.push(word.to_string());
        }
    }
    compound_words.sort_by(|a,b| b.len().cmp(&a.len()));
    println!("Compound words are: {:?}", compound_words);
}

fn is_compound<'a>(trie: &Trie<String, i32> , word: &'a String, level: i32) -> bool {
    //println!("is compound? {}", &word);
    for i in 1..word.len() {
        let prefix : String = word.chars().take(i).collect();
        if trie.get(&prefix).is_none() {
            continue;
        }
        //println!("prefix {}", &prefix);
        let rest : String = word.chars().skip(i).collect();
        //println!("rest {}", &rest);
        if is_compound(trie, &rest, level + 1) {
           return true;
        }
    }
    if trie.get(word).is_some() && level > 1 {
        return true;
    }
    //println!("not compound {}", &word);

    false
}

fn test_radix_fixed_input<'a>(trie: Trie<&'a str, i32>) {
    for (k,v) in trie.iter() {
        println!("{}: {}", k,v);
    }

    let words: [&str; 5] = ["cat", "cats", "dog", "rat", "hip"];

    for w in &words {
        list_children(&trie, w);
    }

    let mut testvec = vec![];
    testvec.push("buford");
    testvec.push("watermelt");

    for tv in &testvec {
        println!("{:?}", tv);
    }
}

fn list_children<'a>(trie: &Trie<&'a str, i32>, word: &'a str){
    let stw = trie.subtrie(word);

    match stw {
        Some(ref _p) => (),
        None => {
            println!("None for {}", word);
            return ;
        }
    }
    let mut st = stw.unwrap();
    let slen = st.len();
    println!("key {:?} val {:?}", st.key(), st.value());

    for _i in 1..slen {
        let stn = st.children().next();
        match stn {
            Some(ref _p) => {}
            None => {
                continue;
            }
        }
        st = stn.unwrap();
        println!("child {:?} {:?}", st.key(), st.value());
    }
}

fn read_lines<P>(filename: P) -> io::Result<io::Lines<io::BufReader<File>>>
where P: AsRef<Path>, {
    //let file = File::open(filename)?;
    let file = File::open(filename).expect("Cannot open file");
    //let mut data = String::new();
    //file.read_to_string(&mut data).expect("Cannot read file into string");
    Ok(io::BufReader::new(file).lines())
}

fn build_trie_from_file(filename: &str) -> Trie<String, i32> {
    let mut trie  = Trie::new();
    let mut ind = 1;

    if let Ok(lines) = read_lines(filename) {
        for line in lines {
            if let Ok(word) = line {
                //println!("{} inserted {}", ind, &word);
                trie.insert(word, ind);
                ind += 1;
                assert!(trie.check_integrity());
            }
        }
    }
    println!("inserted {} lines", ind);
    trie
}

fn build_trie() -> Trie<String, i32> {
    let mut trie = Trie::new();
    let mut ind = 1;

    for &word in &WORDLIST1 {
        let w = word.to_string();
        trie.insert(w, ind);
        ind += 1;
        assert!(trie.check_integrity());
    }
    trie
}

fn print_type_of<T>(_: &T) {
    println!("type {}", std::any::type_name::<T>())
}

fn scratch() {
    let mut trie = Trie::new();
    let mut ind = 1;

    print_type_of(&trie);

    for &word in &WORDLIST1 {
        trie.insert(word, ind);
        ind += 1;
        assert!(trie.check_integrity());
    }

    assert!(trie.get(&"nonexistent").is_none());

    println!("catsdogcats {:?}", trie.get(&"catsdogcats"));
    println!("cats {:?}", trie.get(&"cats"));
    println!("cat {:?}", trie.get(&"cat"));
    println!("dog {:?}", trie.get(&"dog"));
    println!("cats {:?}", trie.get_ancestor_value(&"cats"));
    println!("ancestor dog {:?}", trie.get_ancestor(&"dog"));

    let subtrie  = trie.subtrie(&"rat").unwrap();

    println!("prefix {:?}", subtrie.prefix());
    let slen = subtrie.len();
    println!("subtrie len {:?}", slen);
    println!("key {:?}", subtrie.key());
    print_type_of(&subtrie);

    let c1 = subtrie.children().next().unwrap();
    println!("c1 {:?}", c1.key());
    let c2 = c1.children().next().unwrap();
    println!("c2 {:?}", c2.key());
    let c3 = c2.children().next().unwrap();
    println!("c3 {:?}", c3.key());
    let c4 = c3.children().next().unwrap();
    println!("c4 {:?}", c4.key());

    let mut st = trie.subtrie(&"rat").unwrap();
    for _i in 1..slen {
        st = st.children().next().unwrap();
        println!("child {:?}", st.key());
    }
}
