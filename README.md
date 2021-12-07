# cserver

## A very simple TCP socket server implementation

Currently, compiling server.c and running the executable will begin a server listening on port 8000. When the server gets a request for `/`, it will return the contents of `index.html`. If it gets a request for `/styles.css` it will return the contents of `styles.css`. It does not do much, but there are plans to possible make it more extensible with custom routes.