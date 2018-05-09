# MiniQuad documents

## Html
![HtmlTree](https://raw.githubusercontent.com/wibauxl/MiniQuad/master/docs/html-tree.svg)
The client calls index.html which loads the Framework7 application.
home is loaded automatically by the application inside index through ajax.
All subsequent pages are loaded through ajax requests.
After the initial loading phase, the server on the ESP only receives short WebSockets messages and requests for json files.
 
