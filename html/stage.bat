@echo off
cd ..\arduino\MiniQuad\data
del *.html *.js *.css *.png *.svg
cd ..\..\..\html
copy *.html ..\arduino\MiniQuad\data
copy *.js ..\arduino\MiniQuad\data
copy *.css ..\arduino\MiniQuad\data
copy *.png ..\arduino\MiniQuad\data
copy MiniQuad.svg ..\arduino\MiniQuad\data

