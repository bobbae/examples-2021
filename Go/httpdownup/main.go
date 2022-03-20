package main

import (
	"bytes"
	"fmt"
	"io"
	"mime/multipart"
	"net/http"
	"os"
	"strings"
)

func main() {
	if len(os.Args) < 5 {
		fmt.Printf("usage: %s user pass example.com:3444/dir1 fname\n", os.Args[0])
		os.Exit(1)
	}
	fname := os.Args[4]
	url := fmt.Sprintf("https://%s:%s@%s/%s", os.Args[1], os.Args[2], os.Args[3], fname)
	err := downloadFile(url, fname)
	if err != nil {
		fmt.Println("download error", err)
		os.Exit(1)
	}
	fname = "hello.txt"
	url = fmt.Sprintf("https://%s:%s@%s", os.Args[1], os.Args[2], os.Args[3])
	err = uploadFile(url, fname)
	if err != nil {
		fmt.Println("upload error", err)
		os.Exit(1)
	}
	os.Exit(0)
}

func uploadFile(url, fname string) error {
	//TODO token support without openid, need to fix the gohttpserver
	fmt.Println("uploading", url, fname)
	rfile, err := os.Open(fname)
	if err != nil {
		return fmt.Errorf("cannot open local file %s, %v", fname, err)
	}
	values := map[string]io.Reader{
		"file":     rfile,
		"filename": strings.NewReader(fname),
	}
	var b bytes.Buffer
	w := multipart.NewWriter(&b)
	for key, r := range values {
		var fw io.Writer
		if x, ok := r.(io.Closer); ok {
			defer x.Close()
		}
		if x, ok := r.(*os.File); ok {
			if fw, err = w.CreateFormFile(key, x.Name()); err != nil {
				return fmt.Errorf("can't create form file, %v, %v", key, err)
			}
		} else {
			if fw, err = w.CreateFormField(key); err != nil {
				return fmt.Errorf("can't create form field %v, %v", key, err)
			}
		}
		if _, err = io.Copy(fw, r); err != nil {
			return err
		}
	}
	w.Close()
	req, err := http.NewRequest("POST", url, &b)
	if err != nil {
		return fmt.Errorf("can't make new POST, %v", err)
	}
	req.Header.Set("Content-Type", w.FormDataContentType())
	client := &http.Client{}
	res, err := client.Do(req)
	if err != nil {
		return fmt.Errorf("http client error: %v", err)
	}

	if res.StatusCode != http.StatusOK {
		return fmt.Errorf("bad status: %s", res.Status)
	}
	return nil
}

func downloadFile(url, fname string) error {
	fmt.Println("attempt to retrieve file from url", "file", fname, "URL", url)
	resp, err := http.Get(url)
	if err != nil {
		return fmt.Errorf("cannot get file from URL %s, %v", url, err)
	}
	fileout, err := os.Create(fname)
	if err != nil {
		return fmt.Errorf("can't create file %s,%v", fname, err)
	}
	_, err = io.Copy(fileout, resp.Body)
	if err != nil {
		return fmt.Errorf("can't write file %s, %v", fname, err)
	}
	resp.Body.Close() // nolint
	fileout.Close()   // nolint
	fmt.Println("retrieved file from URL", "URL", url, "file", fname)
	return nil
}
