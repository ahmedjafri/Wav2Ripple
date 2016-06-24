
$(document).ready(function() {

	var tag = document.createElement('script');

	tag.src = "https://www.youtube.com/iframe_api";
	var firstScriptTag = document.getElementsByTagName('script')[0];
	firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

	var intensities = [];
	var chart;

	$("form").submit(function(event) {
		$.ajax({
			url:"/",
			method:"POST",
			dataType: "json",
			data: JSON.stringify({ url: $("#url").val() }),
			contentType:"application/json"
		}).done(function(xhr) {

			var dps = [];
			for(var i = 0; i < xhr.length; i++) {
				dps.push({x:i+1, y:xhr[i]*100});
			}
 
			chart = new CanvasJS.Chart("chartContainer",{
				title :{
					text: "Live Data"
				},
				axisX: {						
					title: "Axis X Title"
				},
				axisY: {						
					title: "Units"
				},
				data: [{
					type: "line",
					dataPoints : dps
				}]
			});
			 
			chart.render();

			initializePlayer($("#url").val());
			console.log(xhr);
			intensities = xhr;
		});

		event.preventDefault();
	});

	var player;
	var playing = false;

	var mainCanvas = document.getElementById("myCanvas");
	var mainContext = mainCanvas.getContext("2d");
	 
	var canvasWidth = mainCanvas.width;
	var canvasHeight = mainCanvas.height;
	 
	var requestAnimationFrame = window.requestAnimationFrame || 
                            window.mozRequestAnimationFrame || 
                            window.webkitRequestAnimationFrame || 
                            window.msRequestAnimationFrame;


    var lastDate = new Date();
    var lastTime = 0;

	function drawCircle() {
	    mainContext.clearRect(0, 0, canvasWidth, canvasHeight);
     
	    // color in the background
	    mainContext.fillStyle = "#EEEEEE";
	    mainContext.fillRect(0, 0, canvasWidth, canvasHeight);
	     
	    // draw the circle
	    mainContext.beginPath();
	     
	    var radius = 175;

	    if(playing) {
	    	var time = player.getCurrentTime(); 
	    	if(time == lastTime) {
	    		elaspedTime = (new Date()) - lastDate;
	    		elaspedTime /= 1000; // remove ms
	    		elaspedTime += time;
	    		time = elaspedTime;
	    	} else {
	    		lastTime = time;
	    		lastDate = new Date();
	    	}

	    	var i = Math.floor(time * 48000.0/1024.0);
	    	var min = Math.floor(time/60);
	    	console.log(min + ":" + (time-(min*60)));
	    	radius = 175 * intensities[i];
	    }

	    mainContext.arc(225, 225, radius, 0, Math.PI * 2, false);
	    mainContext.closePath();
	     
	    // color in the circle
	    mainContext.fillStyle = "#006699";
	    mainContext.fill();
	     
	    requestAnimationFrame(drawCircle);
	}

	drawCircle();

	function initializePlayer(url) {

		var videoid = url.match(/(?:https?:\/{2})?(?:w{3}\.)?youtu(?:be)?\.(?:com|be)(?:\/watch\?v=|\/)([^\s&]+)/);
		var videoid = videoid[1];

		if(videoid == null) {
			throw new Exception("Cannot get video id from url");
			return;
		}

		// 3. This function creates an <iframe> (and YouTube player)
		//    after the API code downloads.
		player = new YT.Player('player', {
		  height: '390',
		  width: '640',
		  videoId: videoid,
		  events: {
		    'onReady': onPlayerReady,
		    'onStateChange': onPlayerStateChange
		  }
		});

		// 4. The API will call this function when the video player is ready.
		function onPlayerReady(event) {
			event.target.playVideo();
		}

		// 5. The API calls this function when the player's state changes.
		//    The function indicates that when playing a video (state=1),
		//    the player should play for six seconds and then stop.
		var done = false;
		function onPlayerStateChange(event) {
			if (event.data == YT.PlayerState.PLAYING) {
				if(!done) {
			  		done = true;
				}
				playing = true;
			} else {
				playing = false;
			}
		}
		function stopVideo() {
		player.stopVideo();
		}
	}

});
