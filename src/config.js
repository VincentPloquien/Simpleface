Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
	var language = localStorage.getItem('language');
	if (language === null || language === "") {
		language = "auto";
	}
	
	var bluetooth = localStorage.getItem('bluetooth');
	if (bluetooth === null || bluetooth === "") {
		bluetooth = "off";
	}
	
	var url = encodeURI('http://pebble.akwaryoum.fr/simpleface/index.html?language=' + language + '&bluetooth=' + bluetooth);
	Pebble.openURL(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    var response = JSON.parse(decodeURIComponent(e.response));
	
	localStorage.setItem('language', response.language);
	localStorage.setItem('bluetooth', response.bluetooth);
	
	if (!response.language || !response.bluetooth) {
		return;
	}
	
	Pebble.sendAppMessage( { 'language': response.language, 'bluetooth': response.bluetooth	},
		function(e) {
			console.log('Send successful.');
		},
		function(e) {
			console.log('Send failed! T-ID: ' + e.data.transactionId + ' Error is: ' + e.error.message);
		}
	);
});

Pebble.addEventListener('ready', function(e) {
	console.log('Ready.');
});