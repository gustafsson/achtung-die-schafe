var game;

function init(){	
	var scene = new Scene(document.getElementById("myCanvas"));
	game = new Game(scene);
}


function startGame(){
    game.start();
}


function Game(scene) {
    this.scene = scene;

	loadImage();
    
    var context = this.scene.context;

    function loadImage(){
        try {
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
        } catch (err) {}
    }
}


Game.prototype.start = function() {
    this.scene.canvas.focus();
    var game = this;
    
	this.scene.canvas.onkeydown = function(evt) {
	    //evt = evt || window.event;
	    var keyCode = evt.keyCode || evt.which,
		    arrow = {left: 37, right: 39 }, $status = $('#status');

	    switch (keyCode) {
	    case arrow.left:
		    window.console.log('leftdown!');
		    game.server.send("leftdown");
	    break;
	    case arrow.right:
		    window.console.log('rightdown!');
		    game.server.send("rightdown");
	    break;
	    }
    };
    
	this.scene.canvas.onkeyup = function(evt) {
	    //evt = evt || window.event;
	    var keyCode = evt.keyCode || evt.which,
		    arrow = {left: 37, right: 39 }, $status = $('#status');

	    switch (keyCode) {
	    case arrow.left:
		    window.console.log('leftup!');
		    game.server.send("leftup");
	    break;
	    case arrow.right:
		    window.console.log('rightup!');
		    game.server.send("rightup");
	    break;
	    }
    };

  /*  	 window.console.log("running pathX");
    this.scene.context.beginPath();
    this.scene.context.strokeStyle = "#FF0000";
    this.scene.context.moveTo(0,0);
    this.scene.context.lineTo(200,100);
    this.scene.context.stroke();
*/
	this.ServerConnection();

};

	
Game.prototype.ServerConnection = function() {
  if ("WebSocket" in window)
  {
    if (this.server !== undefined)
        return;

	 this.server = new WebSocket("ws://localhost:10001");
	 //this.server = new WebSocket("ws://82.115.206.11:10001");
	 //this.server = new WebSocket("ws://82.115.206.12:10001");
     var server = this.server;
     var scene = this.scene;
     
	 this.server.onerror = function(evt)
	 {
		window.console.log("error");
	 };
	 this.server.onmessage = function (evt) 
	 {
		var message = eval(evt.data);
		
		scene.cameraX = message.playerPosition[0];
		scene.cameraY = message.playerPosition[1];
		
		for (var i=0;i<message.newTrails.length;i++)
		{
		    var trail = message.newTrails[i];
		    if (0 === trail.length)
		        continue;

            var block = scene.getBlockOrCreate(trail.blockX, trail.blockY);
            
            // addKurv and appendKurv will also render to the blocks off-screen target (but not to the displayed canvas on-screen)
            var kurv = block.getKurv(trail.id);
            var plotp;
            var plotcolor;
            if (kurv === undefined)
            {
				plotp = block.addKurv(new Kurv(trail));
				plotcolor = trail.color;
			}
            else 
            {
                plotp = block.appendKurv(trail.id, trail.p);
				plotcolor = block.getKurv(trail.id).color;
            }
            
            for(var x = trail.blockX-1; x <= trail.blockX+1; ++x)
                for(var y = trail.blockY-1; y <= trail.blockY+1; ++y)
            {
                drawKurv(scene.getBlockOrCreate(x,y).preRenderedContext, plotp, plotcolor);
            }
		}
		
		scene.draw();
		server.send(""); // Not always necessary
	 };
	 this.server.onclose = function()
	 { 
		window.console.log("Connection is closed..."); 
	 };
	 this.server.onopen = function()
	 {
		server.send("I'm in");
	 };
  }
  else
  {
	 // The browser doesn't support WebSocket
	 window.console.log("WebSocket NOT supported by your Browser!");
  }
};


function Kurv(trail) {
    // Constructor for new sausages
    this.id = trail.id;
    this.p = trail.p;
    this.color = trail.color;
}


function Block(blockX,blockY,size) {
    this.blockX = blockX;
    this.blockY = blockY;
    this.preRendered = document.createElement('canvas');
    this.preRendered.width = size;
    this.preRendered.height = size;
    this.preRenderedContext = this.preRendered.getContext('2d');
	this.preRenderedContext.clearRect(0, 0, size, size);
	this.preRenderedContext.translate(-blockX*size,-blockY*size);
	this.preRenderedContext.lineWidth=10;
	this.preRenderedContext.lineCap="round";
    
    this.kurv_list = [];
}


Block.prototype.getKurv = function(id) {
	return this.kurv_list[id];
};


Block.prototype.addKurv = function(kurv) {
    this.kurv_list[kurv.id] = kurv;
    
    // drawKurv(this.preRenderedContext, kurv.p, kurv.color);
    
    return kurv.p;
};


Block.prototype.appendKurv = function(id,p) {
    var kurv = this.kurv_list[id];
    lastp = kurv.p[kurv.p.length-1];
    kurv.p = kurv.p.concat(p);

    p = [lastp].concat(p);
    // drawKurv(this.preRenderedContext, p, kurv.color);
    return p;
};


function drawKurv(ctx,p,color)
{
	ctx.beginPath();
	ctx.strokeStyle = color;
		
	ctx.moveTo(p[0][0],p[0][1]);
	for (var j=1; j<p.length; j++)
		ctx.lineTo(p[j][0],p[j][1]);
	
	ctx.stroke();
}


function Scene(canvas) {
	this.canvas = canvas;
	this.context = canvas.getContext('2d');
	this.blockSize = 400;
	this.cameraX = 0;
	this.cameraY = 0;
    this.scale = 1;
    this.blocks = [[]];

	this.context.lineWidth=10;
	this.context.lineCap="round";

	this.context.translate(this.context.canvas.width/2, this.context.canvas.height/2);
}


Scene.prototype.draw = function() {
	
	// Reset the transformation matrix
	this.context.setTransform(1, 0, 0, 1, 0, 0);
	
	// Clean everything
	this.context.clearRect(0, 0, this.context.canvas.width, this.context.canvas.height);

	// Translate the whole scene according to the player's current location
	this.context.translate(this.context.canvas.width/2, this.context.canvas.height/2);
	this.context.scale(this.scale, this.scale);
	this.context.translate(-this.cameraX, -this.cameraY);
	
	// Draw all prerendered image blocks
	for (var x=Math.floor((this.cameraX - this.context.canvas.width/(2*this.scale))/this.blockSize);
	         x<=Math.ceil((this.cameraX + this.context.canvas.width/(2*this.scale))/this.blockSize); x++)
	for (var y=Math.floor((this.cameraY - this.context.canvas.height/(2*this.scale))/this.blockSize);
	         y<=Math.ceil((this.cameraY + this.context.canvas.height/(2*this.scale))/this.blockSize); y++)
    {
        var block = this.getBlock(x,y);
        if (block === undefined)
            continue;

        this.context.drawImage(block.preRendered, x*this.blockSize, y*this.blockSize, this.blockSize, this.blockSize);
    }
};


Scene.prototype.getBlockCoordinates = function(x,y) {
    return [Math.floor(x/this.blockSize), Math.floor(y/this.blockSize)];
}


Scene.prototype.getBlock = function(x,y) {
    var blockOffset = 1000000;
    var yblocks = this.blocks[x+blockOffset];
    return yblocks!==undefined ? yblocks[y+blockOffset] : undefined;
};


Scene.prototype.setBlock = function(x,y,block) {
    var blockOffset = 1000000;
    var yblocks = this.blocks[x+blockOffset];
    if (yblocks===undefined)
        this.blocks[x+blockOffset] = yblocks = [];
    yblocks[y+blockOffset] = block;
};


Scene.prototype.getBlockOrCreate = function(x,y) {
    var block = this.getBlock(x, y);
    if (block === undefined)
    {
        block = new Block(x, y, this.blockSize);
        this.setBlock(x, y, block);
    }
    return block;
};
