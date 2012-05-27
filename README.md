achtung-die-schafe
==================

Achtung, die Schafe!


Build the server using Qt (http://qt.nokia.com/products/)

To build:
* cd server
* qmake
* make

In windows, instead of the last make step, open the project created by qmake in Visual C++ and build from there instead.


To run the server after building it:
* server/server
* Windows: server\Release\Win32\server.exe


To run the client locally:
* Point a browser to client/index.html?host=localhost


To play with others:
* Make the client folder accessible through a web server. Figure out which ip address you've got and send away the link such as:
http://myachtungserver.com/index.html?host=123.45.67.123
