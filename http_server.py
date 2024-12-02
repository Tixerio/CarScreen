from http.server import BaseHTTPRequestHandler, HTTPServer

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        # Respond to GET requests
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        self.wfile.write(b"success get")  # Send response

    def do_POST(self):
        # Retrieve the "Name" header value sent by the client
        name_value = self.headers.get("Name", "Unknown")
        
        # Respond to POST requests
        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        
        # Send a response including the header value
        response = f"success post, received Name: {name_value}".encode("utf-8")
        print(name_value)
        self.wfile.write(response)

def run(server_class=HTTPServer, handler_class=SimpleHTTPRequestHandler, port=8000):
    server_address = ('', port)
    httpd = server_class(server_address, handler_class)
    print(f"Starting server on port {port}")
    httpd.serve_forever()

if __name__ == "__main__":
    run()
