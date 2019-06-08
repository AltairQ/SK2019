# webserver
## Simple C HTTP server using Linux sockets

Use the makefile in this directory
Usage: `./webserver <port> <root dir>`

The webserver binds to 0.0.0.0:port and serves content from `<host>/<URL>`. This means that a GET request for `http://localhost:port/foo/bar.html` references a file under `<root dir>/localhost/foo/bar.html`.

Responds only to GET requests, does not support caching nor chunked transport. Implements protection against path traversal.