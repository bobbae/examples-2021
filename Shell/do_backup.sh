#
# sudo mount -t drvfs Z: /mnt/z
find ~/bobba/books/ -type f -print0 | xargs -0 md5sum  |   while read line
do
	csum=`echo "$line" | awk '{print $1}'`
	fn=`echo "$line" | awk '$1=""; {print $0}'`
	fn=`echo "$fn" | awk '{$1=$1;print}'`
	srcfn="$fn"
	dstfn=/mnt/z/books/`basename "$fn"`
	stat "$dstfn"
	if [ $? -eq 1 ]; then
		echo cp  "$srcfn" "$dstfn"
		cp  "$srcfn" "$dstfn"
	fi
done
