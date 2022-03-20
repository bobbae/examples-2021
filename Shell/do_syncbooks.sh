# sudo mount  -t drvfs Z: /mnt/z
# make sure Z: on host points to correct drive
rsync -avz /home/bob/bobba/books /mnt/z/
