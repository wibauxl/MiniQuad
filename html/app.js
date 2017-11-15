// Dom7
var $ = Dom7;

// Theme
var theme = 'auto';
if (document.location.search.indexOf('theme=') >= 0) {
  theme = document.location.search.split('theme=')[1].split('&')[0];
}

// Init App
var app = new Framework7({
  root: '#app',
  theme: theme,
  routes: routes,
});

// Create a WebSocket to get/set simple values quickly
// values are sent as parameter_name:parameter_value
var ws = new WebSocket('ws://'+document.location.host+'/ws',['arduino']);
ws.binaryType = "arraybuffer";
ws.onopen = function(e){
    console.log("/ws/ <> connected");
};
ws.onmessage = function(e) {
	var data = e.data.split(':');
	var parameter = data[0];
	var value = parseInt(data[1]);
	console.log("[" + (new Date()).getTime() + "] /ws/ > " + parameter + "=" + value);
	if (parameter == "battery") {
		$('.mq-battery-text').html((value < 100 ? " " : "") + (value < 10 ? " " : "") + value + "%");
		$('.mq-battery-fill').attr('width', value);
	} else if (parameter == "speed") {
		app.range.setValue("#mq-speed", value);
	} else if (parameter == "moveSpeed") {
		app.range.setValue("#mq-moves-speed", value);
	}
};

sendWSCommand = function(command) {
	console.log("/ws/ < " + command);
	ws.send(command);
};

// Load the initial page
// See https://blog.framework7.io/mastering-v2-router-958ea2dbd24f
app.views.create('.view', {url: '/home/'});
