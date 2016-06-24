var express = require("express")
var app = express();
var fs = require('fs');
var process = require('child_process');


var bodyparser = require("body-parser");
app.use(bodyparser.json());
app.use(bodyparser.urlencoded({ extended: true }));

app.post("/", function(req, res) {
	if(!req.body.url) {
		res.status(400).send();
		return;
	}

	var test = clean(req.body.url);
	filepath = __dirname + "/intensities/" + clean(req.body.url) + ".json";

	fs.readFile(filepath, 'utf8', function (err,data) {
		if (err) {
	    	fetchIntensities(req, res);
		}
	    else {
	    	res.status(200).send(data);
	    }
	});
});

app.use(express.static('assets'));

function clean(url) {
	return url.replace(/\W+/g, ""); 
}

function fetchIntensities(req, res) {
	var command = "youtube-dl --max-downloads 1 --max-filesize 100m -o \"media.%(ext)s\" -x --verbose --audio-format wav \"" + req.body.url + "\""
	var ytd = process.exec(command, function (error, stdout, stderr) {
		if(error)
	 		res.status(500).send(stdout + stderr);
	 	else {
	 		var wav2r = process.exec("Wav2R media.wav", function (error, stdout, stderr) {
				if(error)
			 		res.status(500).send(stdout + stderr);
			 	else {
			 		fs.readFile('intensities.json', 'utf8', function (err,data) {
						if (err)
					    	res.status(500).send(err);
					    else {
					    	res.status(200).send(data);
					    	var dir = __dirname + "/intensities/";
					    	if (!fs.existsSync(dir)){
							    fs.mkdirSync(dir);
							}

					    	move(__dirname + "/intensities.json", dir + clean(req.body.url) + ".json",function(err) {
					    			if(err)
					    				console.log(err);
					    	});
					    }
					});
			 	}
			});
	 	}
	});
}

app.listen(3000, function() {
	console.log("Listening on port 3000 now.");
})

function move(oldPath, newPath, callback) {
    fs.rename(oldPath, newPath, function (err) {
        if (err) {
            if (err.code === 'EXDEV') {
                copy();
            } else {
                callback(err);
            }
            return;
        }
        callback();
    });

    function copy () {
        var readStream = fs.createReadStream(oldPath);
        var writeStream = fs.createWriteStream(newPath);

        readStream.on('error', callback);
        writeStream.on('error', callback);
        readStream.on('close', function () {

        fs.unlink(oldPath, callback);
    });

    readStream.pipe(writeStream);

    }
}