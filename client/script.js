
var center=[0,0];
var height;
var width;
var c;
var context;
var ws;
var scene;

function init(){
	c=document.getElementById("myCanvas");
	context=c.getContext("2d");

	scene = new Scene(context);

	context.lineWidth=10;
	context.lineCap="round";

	height = c.height;
	width = c.width;
	center=[width/2,height/2];

	var p = [center[0],center[1]];
	for (j = 1; j < 100; j++) {
		p[j] = [center[0]+j,center[1]+j];
	}
	scene.addKurv(new Kurv(1,p,'#FF0000'));
	for (j = 1; j < 100; j++) {
		p[j] = [center[0]+100+j,center[1]];
	}
	scene.addKurv(new Kurv(1,p,'#00FF00'));
	
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

function Kurv(id, p, color) {
  // Constructor for new sausages
  this.id = id;
  this.p = p;
  this.color = color || '#AAAAAA';
}

function Scene(context) {
	this.context = context;
}

Scene.prototype.addKurv = function(kurv) {
		this.kurv = kurv;
		scene.draw_kurv(kurv.p, kurv.color);
};

Scene.prototype.draw_kurv = function(p,color) {
	context.beginPath();
	context.moveTo(p[0][0],p[0][1]);
	context.strokeStyle = color;
	for(var i = 0; i < p.length; i++){
		context.lineTo(p[i][0],p[i][1]);
		context.stroke();
	}
};

function WebSocketTest()
{
  if ("WebSocket" in window)
  {
	 // Let us open a web socket
	 //var ws = new WebSocket("ws://localhost:9998/echo");
	 ws = new WebSocket("ws://85.115.206.12:39907");
	 ws.onerror = function(evt)
	 {
		window.console.log("error");
	 };
	 ws.onmessage = function (evt) 
	 {
		window.console.log("got stuff");
		var received_msg = eval(evt.data);
		window.console.log("Message is received...");
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

