Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://pebble.akwaryoum.fr/simpleface/index.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
	Pebble.sendAppMessage('configuration', configuration);
});