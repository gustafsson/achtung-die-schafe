
var center=[0,0];
var height;
var width;
var c;
var context;
ws=false;
var scene;
var received_msg; 
kurv_list=[];

function init(){
	c=document.getElementById("myCanvas");
	context=c.getContext("2d");

	scene = new Scene(context);

	context.lineWidth=10;
	context.lineCap="round";

	height = c.height;
	width = c.width;
	center=[width/2,height/2];
	
	c.onkeydown = function(evt) {
		//evt = evt || window.event;
		var keyCode = evt.keyCode || evt.which,
			arrow = {left: 37, right: 39 }, $status = $('#status');
		switch (keyCode) {
		case arrow.left:
			window.console.log('left!');
			ws.send("left");
		break;
		case arrow.right:
			window.console.log('right!');
			ws.send("right");
		break;
		}
	};
	
	c.addEventListener("mousedown", mouseDown, false);	
}

function mouseDown(){
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
		scene.draw_kurv(kurv.p, kurv.color);
};

Scene.prototype.draw_kurv = function(p,color) {
	context.beginPath();
	context.moveTo(p[0][0],p[0][1]);
	context.strokeStyle = '#ff0000';
	for(var i = 0; i < p.length; i++){
		context.lineTo(p[i][0],p[i][1]);
		context.stroke();
	}
};

Scene.prototype.getKurv = function(id) {
	return kurv_list[id];
};

Scene.prototype.append_kurv = function(id,p) {
	kurv_list[id].p = getKurv(id).p.concact(p);
};

function WebSocketTest()
{
  if ("WebSocket" in window || !ws)
  {
	 // Let us open a web socket
	 //var ws = new WebSocket("ws://localhost:9998/echo");
	 ws = new WebSocket("ws://82.115.206.12:10001");
	 ws.onerror = function(evt)
	 {
		window.console.log("error");
	 };
	 ws.onmessage = function (evt) 
	 {
		//window.console.log("got stuff");
		received_msg = eval(evt.data);
		//context.clear();
		for (var i=0;i<received_msg.newTrails.length;i++){
			if (scene.getKurv(received_msg.newTrails[i])) {
				scene.append_kurv(received_msg.newTrails[i].id,received_msg.newTrails[i].p);
			}
			else{
				scene.addKurv(new Kurv(received_msg.newTrails[i]));
			}
		}
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

