Pebble.addEventListener('showConfiguration', function(e) {
  // Show config page
	var battery = localStorage.getItem('battery');
	if (battery === null || battery === "") {
		battery = "off";
	}
	
	var bluetooth = localStorage.getItem('bluetooth');
	if (bluetooth === null || bluetooth === "") {
		bluetooth = "off";
	}
	
	var isPebbleClassic = false;
	
	var url = "http://pebble.akwaryoum.fr/simpleface/";	

	if (isPebbleClassic) {
		url += "http://pebble.akwaryoum.fr/simpleface/pebble-classic.html?";
	} else {
		url += "http://pebble.akwaryoum.fr/simpleface/pebble-time.html?";
	}
	
	url += 'battery=' + battery + '&bluetooth=' + bluetooth;
	Pebble.openURL(encodeURI(url));
});

Pebble.addEventListener('webviewclosed', function(e) {
    var response = JSON.parse(decodeURIComponent(e.response));
	
	localStorage.setItem('battery', response.battery);
	localStorage.setItem('bluetooth', response.bluetooth);
	
	if (!response.battery || !response.bluetooth) {
		return;
	}
	
	Pebble.sendAppMessage( { 'battery': response.battery, 'bluetooth': response.bluetooth	},
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