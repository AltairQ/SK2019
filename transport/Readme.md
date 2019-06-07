# transport
## C UDP file downloader

Usage: `./transport <IP addr> <port> <filename> <byte count>`.

Designed to use simple text-based protocol: sends a `GET <start> <len>\n` command to the server and expects to receive a response of the form `DATA <start> <len>\n`, followed by the payload.

The program is designed to be highly resistant to packet loss and delays by implementing TCP window approach - see `nodes.h` for the definition of the structure of a single entry in the window, and `main.c:7` for the `WINDOW_SIZE` parameter.
