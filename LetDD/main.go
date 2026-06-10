package main

import (
	"fmt"
	"net/http"
)

func helloHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Fprintf(w, "Hello, World!")
}

func main() {
	mux := http.NewServeMux()

	mux.HandleFunc("/hello", helloHandler)

	Something()

	fmt.Println("Starting server on 8080...")
	if err := http.ListenAndServe(":8080", mux); err != nil {
		fmt.Printf("Server fails: %s\n", err)
	}
}
