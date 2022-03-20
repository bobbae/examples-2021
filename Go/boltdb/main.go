package main

import (
	"fmt"
	"github.com/boltdb/bolt"
	"log"
)

func main() {
	// Open the my.db data file in your current directory.
	// It will be created if it doesn't exist.
	db, err := bolt.Open("my.db", 0600, nil)
	if err != nil {
		log.Fatal(err)
	}
	defer db.Close()
	db.Update(func(tx *bolt.Tx) error {
		_, err := tx.CreateBucket([]byte("MyBucket"))
		if err != nil {
			return fmt.Errorf("create bucket: %s", err)
		}
		return nil
	})

	db.Update(func(tx *bolt.Tx) error {
		b := tx.Bucket([]byte("MyBucket"))
		iter := 1
		var err error
		for iter < 1000 {
			err = b.Put([]byte(fmt.Sprintf("answer-%d", iter)), []byte("42"))
			if err != nil {
				fmt.Errorf("put error, %v", err)
				break
			}
			iter++
		}
		if err != nil {
			return err
		}

		fmt.Printf("wrote %d items\n", iter)
		return nil
	})
}
