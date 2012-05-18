
var height;
var width;
var c;
var context;
ws=false;
var scene;
var received_msg; 
kurv_list=[];
offset=[0,0];
playerX=400;
playerY=300;

function init(){
	c=document.getElementById("myCanvas");
	context=c.getContext("2d");
	
	loadImage();
	
	scene = new Scene(context);

	context.lineWidth=10;
	context.lineCap="round";

	height = c.height;
	width = c.width;
	offset=[width/2,height/2];
	
	c.onkeydown = function(evt) {
		//evt = evt || window.event;
		var keyCode = evt.keyCode || evt.which,
			arrow = {left: 37, right: 39 }, $status = $('#status');
		switch (keyCode) {
		case arrow.left:
			window.console.log('leftdown!');
			ws.send("leftdown");
		break;
		case arrow.right:
			window.console.log('rightdown!');
			ws.send("rightdown");
		break;
		}
	};
	c.onkeyup = function(evt) {
		//evt = evt || window.event;
		var keyCode = evt.keyCode || evt.which,
			arrow = {left: 37, right: 39 }, $status = $('#status');
		switch (keyCode) {
		case arrow.left:
			window.console.log('leftup!');
			ws.send("leftup");
		break;
		case arrow.right:
			window.console.log('rightup!');
			ws.send("rightup");
		break;
		}
	};
	
}

function loadImage(){
    var request = new XMLHttpRequest();
    request.open("GET", "achtung.jpg", true);
    request.onreadystatechange = function(){
        if (request.readyState == 4) { // Makes sure the document is ready to parse.
            if (request.status == 200) { // Makes sure it's found the file.
                	var imageObj = new Image();
					imageObj.onload = function(){
						context.drawImage(this, -240, -80);
					};
					
					imageObj.src = "achtung.jpg";
            }
        }
    };
    request.send(null);
}

function startGame(){
		context.clearRect(0, 0, c.width, c.height);
		c.focus();
		WebSocketTest();
}

function Kurv(trail) {
  // Constructor for new sausages
  this.id = trail.id;
  this.p = trail.p;
  this.color = trail.color;
}

function Scene(context) {
	this.context = context;
}

Scene.prototype.addKurv = function(kurv) {
		kurv_list[kurv.id] = kurv;
};

Scene.prototype.draw = function() {
	
	//save current context status
	context.save();
	
	//clean everything
	context.fillRect(0, 0, context.canvas.width, context.canvas.height);
	
	//Reset the transformation matrix
	context.setTransform(1, 0, 0, 1, 0, 0);
	
	//Translate the whole scene according to the player's current location
	context.translate(context.canvas.width/2 - playerX, context.canvas.height/2 - playerY)
	
	
	for (var i=1; i<kurv_list.length;i++){
		context.beginPath();	
		for (var j=0; j<kurv_list[i].p.length;j++){
			context.moveTo(kurv_list[i].p[j][0],kurv_list[i].p[j][1]);
			context.strokeStyle = kurv_list[i].color;
			context.lineTo(kurv_list[i].p[j][0],kurv_list[i].p[j][1]);
		}
			context.stroke();
	}
};

Scene.prototype.getKurv = function(id) {
	return kurv_list[id];
};

Scene.prototype.append_kurv = function(id,p) {
	kurv_list[id].p = scene.getKurv(id).p.concat(p);
};

function WebSocketTest()
{
  if ("WebSocket" in window || !ws)
  {
	 // Let us open a web socket
	 //var ws = new WebSocket("ws://localhost:9998/echo");
	 ws = new WebSocket("ws://82.115.206.11:10001");
	 ws.onerror = function(evt)
	 {
		window.console.log("error");
	 };
	 ws.onmessage = function (evt) 
	 {
		//window.console.log("got stuff");
		received_msg = eval(evt.data);
		
		//update player position
		playerX = received_msg.playerPosition[0];
		playerY = received_msg.playerPosition[1];
		
		//context.clear();
		for (var i=0;i<received_msg.newTrails.length;i++){
			if (scene.getKurv(received_msg.newTrails[i].id)) {
				scene.append_kurv(received_msg.newTrails[i].id,received_msg.newTrails[i].p);
			}
			else{
				scene.addKurv(new Kurv(received_msg.newTrails[i]));
			}
			//TODO remember to handle player's position
			
		}
		//TODO draw scene if everything is ready
		scene.draw();
		ws.send("");
		//window.console.log("Message is received...");
	 };
	 ws.onclose = function()
	 { 
		// websocket is closed.
		window.console.log("Connection is closed..."); 
	 };
	 ws.onopen = function()
	 {
		// Web Socket is connected, send data using send()
		ws.send("I'm in");
	 };
  }
  else
  {
	 // The browser doesn't support WebSocket
	 window.console.log("WebSocket NOT supported by your Browser!");
  }
}

