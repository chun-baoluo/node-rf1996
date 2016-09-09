const rf1996 = require('./build/Release/rf1996');
rf1996.open('COM5', function(err) {
	var val = null;
	if(!err) {
		console.log(rf1996.device());
		
		rf1996.write({
			brackets: 99,
			group: 125,
			decimal: 123
		});
		
		setInterval(function() {
			var data = rf1996.read();
			if(data.emMarine != val && data.emMarine != '[0000] 000,00000') {
				val = data.emMarine;
				console.log(data);
			} else if(data.emMarine != val && data.emMarine == '[0000] 000,00000' && val != null) {
				val = data.emMarine;
				console.log('No card');
			}
		}, 1000);
	} else {
		console.log(err);
	}
});