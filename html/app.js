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
  id: 'com.github.miniquad',
  theme: theme,
  routes: routes
});

// Get the config and store it
// only the servoNames will be used
var miniQuadServoNames;
app.request.json('config.json', function (data) {
  miniQuadServoNames = data;
});
getMiniQuadServoNames = function() {
  return miniQuadServoNames;
}

// Create a WebSocket to get/set simple values quickly
// values are sent as parameter_name[:target]:parameter_value
var ws = new WebSocket('ws://'+document.location.host+'/ws',['arduino']);
ws.binaryType = "arraybuffer";
ws.onopen = function(e){
    console.log("/ws/ <> connected");
};
ws.onmessage = function(e) {
	var data = e.data.split(':');
	var parameter = data[0];
	console.log("[" + (new Date()).getTime() + "] /ws/ > " + (data.length > 2 ? data[0] + ":" + data[1] + "=" + data[2] : data[0] + "=" + data[1]));
	if (data[0] == "battery") {
		var value = parseInt(data[1]);
		$('.mq-battery-text').html((value < 100 ? " " : "") + (value < 10 ? " " : "") + value + "%");
		$('.mq-battery-fill').attr('width', value);
	} else if (data[0] == "speed") {
		var value = parseInt(data[2]);
		if (data[1] == "main") app.range.setValue("#mq-speed", value);
		else app.range.setValue("#mq-custom-move-speed", value);
	} else if (data[0] == "radar") {
		var value = parseInt(data[1]);
    // do something with the radar value
  }
};

sendWSCommand = function(command) {
	console.log("/ws/ < " + command);
	ws.send(command);
};

// sleep function
sleepMs = function sleep(milliseconds) {
  var start = new Date().getTime();
  for (var i = 0; i < 1e7; i++) {
    if ((new Date().getTime() - start) > milliseconds) break;
  }
};

// Load the initial page
// See https://blog.framework7.io/mastering-v2-router-958ea2dbd24f
app.views.create('.view-main', {url: '/home/'});
