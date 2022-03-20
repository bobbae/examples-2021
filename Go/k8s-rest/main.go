package main

import (
	"bytes"
	"encoding/json"
	"flag"
	"io/ioutil"
	"log"
	"net/http"
	"os"

	"github.com/bobbae/q"
	"github.com/oliveagle/jsonpath"
)

// Assumes kubectl proxy at 8080
var API = "http://localhost:8080/api/v1/"

func main() {
	namespace := flag.String("namespace", "", "namespace")
	deployment := flag.String("deployment", "", "deployment")

	flag.Parse()

	topics := []string{
		"nodes", "pods", "namespaces", "services",
		"configmaps", "endpoints", "secrets",
	}

	for _, topic := range topics {
		if err := listNames(topic); err != nil {
			log.Fatalf("%s, %v", topic, err)
		}
	}

	if *deployment != "" {
		if *namespace == "" {
			log.Fatalf("namespace must be provided")
		}
		if err := createDeployment(*namespace, *deployment); err != nil {
			log.Fatalf("can't deploy, %v", err)
		}
	} else if *namespace != "" {
		if err := addNamespace(*namespace); err != nil {
			log.Fatalf("can't add namespace, %v", err)
		}
	}

	os.Exit(0)
}

func listNames(endpoint string) error {
	body, err := getResp(endpoint)
	if err != nil {
		return err
	}

	err = printResult(body)
	if err != nil {
		return err
	}

	return nil
}

func getResp(endpoint string) ([]byte, error) {
	resp, err := http.Get(API + endpoint)
	if err != nil {
		return nil, err
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	return body, nil
}

func printResult(body []byte) error {
	var json_data interface{}
	if err := json.Unmarshal(body, &json_data); err != nil {
		return err
	}

	res, err := jsonpath.JsonPathLookup(json_data, "$.kind")
	if err != nil {
		return err
	}

	q.Q(res)

	res, err = jsonpath.JsonPathLookup(json_data, "$.items[0:-1].metadata.name")
	if err != nil {
		return err
	}

	q.Q(res)

	return nil
}

func doPost(fn, endpoint string) error {
	content, err := ioutil.ReadFile(fn)
	if err != nil {
		return err
	}

	req, err := http.NewRequest("POST", endpoint, bytes.NewBuffer(content))
	req.Header.Set("Content-Type", "application/yaml")

	client := &http.Client{}

	resp, err := client.Do(req)
	if err != nil {
		return err
	}
	defer resp.Body.Close()

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return err
	}

	q.Q(endpoint, fn, string(body))

	return nil
}

func addNamespace(fn string) error {
	if err := doPost(fn, API+"namespaces"); err != nil {
		return err
	}
	return nil
}

func createDeployment(namespace, fn string) error {
	if err := doPost(fn, API+"namespaces/"+namespace+"/replicationcontrollers"); err != nil {
		return err
	}
	return nil
}
