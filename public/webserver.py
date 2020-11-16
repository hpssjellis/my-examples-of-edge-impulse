#!/usr/bin/env python
import BaseHTTPServer, SimpleHTTPServer

port=8000
print("Running on port %d" % port)

SimpleHTTPServer.SimpleHTTPRequestHandler.extensions_map['.wasm'] = 'application/wasm'

httpd = BaseHTTPServer.HTTPServer(('localhost', port),
    SimpleHTTPServer.SimpleHTTPRequestHandler)

httpd.serve_forever()
