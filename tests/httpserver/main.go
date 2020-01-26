package main

import (
	"context"
	"io/ioutil"
	"log"
	"net/http"
)

type httpHandler struct{}

func redirectMux() *http.ServeMux {
	m := http.NewServeMux()
	m.HandleFunc("/301", func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, "http://localhost:8000/redirected/301", 301)
	})

	m.HandleFunc("/loop", func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, "http://localhost:8000/redirect/loop", 301)
	})

	m.HandleFunc("/count", func(w http.ResponseWriter, r *http.Request) {
		http.Redirect(w, r, "http://localhost:8000/redirect/loop", 301)
	})

	return m
}

func redirectedMux() *http.ServeMux {
	m := http.NewServeMux()
	m.HandleFunc("/301", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("Redirected 301"))
	})
	return m
}

func getRobots() []byte {
	b, _ := ioutil.ReadFile("./tests/httpserver/robots.txt")
	return b
}

func main() {
	m := http.NewServeMux()

	s := http.Server{Addr: ":8000", Handler: m}

	m.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		w.Write([]byte("<html><body><h1>Testing page</h1></body></html>"))
	})

	m.HandleFunc("/json", func(w http.ResponseWriter, r *http.Request) {
		w.Header().Add("Content-Type", "application/json")
		w.Write([]byte("(function _(){return'('+_+')()'})()"))
	})

	m.HandleFunc("/robots.txt", func(w http.ResponseWriter, r *http.Request) {
		w.Write(getRobots())
	})

	m.HandleFunc("/empty", func(w http.ResponseWriter, r *http.Request) {
	})

	m.Handle("/redirect/", http.StripPrefix("/redirect", redirectMux()))
	m.Handle("/redirected/", http.StripPrefix("/redirected", redirectedMux()))

	m.HandleFunc("/shutdown",
		func(w http.ResponseWriter, r *http.Request) {
			w.Write([]byte("OK"))
			go func() { s.Shutdown(context.Background()) }()
		})

	if err := s.ListenAndServe(); err != nil && err != http.ErrServerClosed {
		log.Fatal(err)
	}
}
