# html application

This is the testing environment for the html application which will run from the arduino platform.

## How to use
- Run the test environment: go to MiniQuadHTTPD and launch the server by clicking on MiniQuadHTTPD.bat.
- Then point your browser to http://localhost. You can also use your smartphone to try the application by browsing to the indicated IP address on the same LAN.
- Once you are happy with the application, run stage.bat to copy relevant files to the arduino application.

## Description
![HtmlTree](https://raw.githubusercontent.com/wibauxl/MiniQuad/master/docs/html-tree.png)
The client calls index.html which loads the Framework7 application.
home is loaded automatically by the application from index through ajax.
All subsequent pages are loaded through ajax requests.
After the initial loading phase, the server on the ESP only receives short WebSockets messages and requests for json files.
