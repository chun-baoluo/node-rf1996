# rf1996

IronLogic RF1996 (Z2-USB 1996) library wrapper for Node.js. Works on Windows only and no support for 32bit Node.js, due to library's restrictions.

## Installation

```sh

$ npm install rf1996

```

## Methods

### .open(port, callback)

Establishes connection with rf1996. Executes callback after method execution. Error (if any) is passed to the callback as an argument.

### .device()

Reads connected device info.

### .read()

Reads card data.

### .write(object)

Writes EmMarine data to a card. Returns an object with information about operation's status. Expects first argument to be an object.

```js

	// Object example
	// Writes [0099] 125,00123
	{
		brackets: 99,
		group: 125,
		decimal: 123
	}

```

### .clear()

Wipes out data from a card.

## Example usage

```js

const rf1996 = require('rf1996');
rf1996.open('COM5', function(err) { // Open device connected to certain COM port
	var val = null;
	if(!err) {
		var device = rf1996.device(); // Read connected device info
		console.log(device);
		/*
			Example output: { 
				port: '\\\\.\\COM5',
				device: 'Adapter RF-1996-125 Khz',
				serial: '820',
				model: '2081',
				firmware: '418',
				license: 'License 01.01.2016',
				cards: '10'
			}
		*/

		rf1996.clear();

		rf1996.write({ // Writes [0099] 125,00123
			brackets: 99,
			group: 125,
			decimal: 123
		});

		setInterval(function() {
			var data = rf1996.read(); // Read card data
			if(data.emMarine != val && data.emMarine != '[0000] 000,00000') {
				val = data.emMarine;
				console.log(data);
				/*
					Example output: { 
						card: 'Temic protected by password',
						temic: 'b3 c5 da 4F fc b2 aa b5 ',
						emMarine: '[0000] 000,00582',
						locksLimit: '1',
						locks: '0',
						battery: 'Discharged'
					}
				*/ 
			} else if(data.emMarine != val && data.emMarine == '[0000] 000,00000' && val != null) {
				val = data.emMarine;
				console.log('No card');
			}
		}, 1000);
	} else {
		console.log(err);
	}
});

```

## License

This is only a wrapper for Ironlogic's RF1996 library from their publicly distributed SDK. All rights for the library belong to IronLogic company and other respective owners.