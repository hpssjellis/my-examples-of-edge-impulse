# Shower timer with Machine Learning and WebAssembly - web application

To run this you'll need Python 2.7 and [ngrok](https://ngrok.com).

1. Start a web server (this uses a custom Python script as it requires the `wasm` file to be served with the right content type):

    ```
    $ python webserver.py
    ```

1. Run ngrok, as only web applications served over HTTPS are allowed to use the microphone.

    ```
    $ ngrok http 80
    ```

1. In the ngrok output look for the `Forwarding` line that has `https` in it.

    ```
    Session Status                online
    Account                       XXX
    Version                       2.3.35
    Region                        United States (us)
    Web Interface                 http://127.0.0.1:4040
    Forwarding                    http://4fc86e9e.ngrok.io -> http://localhost:8000
    Forwarding                    https://4fc86e9e.ngrok.io -> http://localhost:8000
    ```

1. Go to the URL from your phone (in the example above: https://4fc86e9e.ngrok.io).
