Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
  Pebble.openURL('http://pebble.akwaryoum.fr/simpleface/index.html');
});

Pebble.addEventListener('webviewclosed', function(e) {
    var configuration = JSON.parse(decodeURIComponent(e.response));
	
    var language = encodeURIComponent(configuration.language);
	if (!language) {
		language = 'auto';
	}
	window.localStorage.setItem('language', language);
  }
);