#!/usr/bin/awk

BEGIN { 
	num = 0; 
} 

function ex1() { 
	print(num, "words"); 
	for (i = 0; i < num; i++) {
		#print(i,words[i]);
		if (is_compound(words[i], 0, 1, length(words[i])) == "1") {
			print(words[i]);
		} else {
			#print("not compound", words[i]);
		}
	}
} 

# recursion: place variables j, wlen, as parameters to protect.
function is_compound(word, level, j, wlen) {
	for ( ; j <= wlen; j++) {
		frag = substr(word, 0, j);
		#print(j, "frag", frag);
		if (frag in dict) {
			rest = substr(word, j + 1, wlen);
			if (rest == "" ) {
				if ( level > 0) {
					return "1";
				}
				break;
			}
			#print("rest",rest);
			if (is_compound(rest, level + 1, 1, length(rest)) == "1") {
				return "1";
			} else {
				#print(j,"continue");
			}	
		}
	}
	#print("return empty");
	return "";
}

{ 
	words[num] = $1;
	dict[$1]=num;
	num++; 
} 

END { 
	ex1();
}
