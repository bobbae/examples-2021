package slackgist

import (
	"html/template"
	"log"
	"net/http"
)

type Build struct {
	Commit string
	URL    string
}

type PageData struct {
	Builds []Build
}

var builds []Build

func uiHandler(rw http.ResponseWriter, r *http.Request) {
	tmpl, err := template.ParseFiles("templates/index.html")
	if err != nil {
		log.Panic("Error occured parsing the template", err)
	}
	page := PageData{
		Builds: builds,
	}
	if err = tmpl.Execute(rw, page); err != nil {
		log.Panic("Failed to write template", err)
	}
}

func CreateUIHandler() http.Handler {
	r := http.NewServeMux()
	r.HandleFunc("/", uiHandler)
	return NewM(r)
}
