@echo off
cd ..\arduino\MiniQuad\data
del *.html *.js *.css *.png *.svg
cd ..\..\..\html
copy app.js ..\arduino\MiniQuad\data
copy routes.js ..\arduino\MiniQuad\data
copy app.css ..\arduino\MiniQuad\data
copy *.html ..\arduino\MiniQuad\data
copy *.png ..\arduino\MiniQuad\data
copy MiniQuad.svg ..\arduino\MiniQuad\data

