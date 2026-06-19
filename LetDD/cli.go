package main

import "flag"

type CLI struct {
	connect *string
	serve   *string
}

func (cli *CLI) Parse() {
	cli.connect = flag.String("connect", "localhost:55543", "Address to connect TCP client")
	cli.serve = flag.String("serve", "localhost:5543", "Address to serve HTTP server")

	flag.Parse()
}
