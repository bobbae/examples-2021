find ~/bobba/books/ -type f -print0 | xargs -0 md5sum  | sort -k1 | awk 'seen[$1]++' | while read line
do
	fn=`echo "$line" | awk '$1=""; {print $0}'`
	fn=`echo "$fn" | awk '{$1=$1;print}'`
	#ls -l "$fn"
	echo \"$fn\"
done
