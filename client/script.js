var game;

function init(){	
	var scene = new Scene(document.getElementById("myCanvas"));
	game = new Game(scene);
}


function startGame(){
    game.start();
    var playbutton = document.getElementById("playbutton");
    playbutton.style.display = "none";
}


function Game(scene) {
    this.serverMessage = document.getElementById("serverMessage");
    this.serverMessage.innerHTML = "Achtung, die Schafe!";
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

    var MouseWheelHandler = function(e) {
        var e = window.event || e; // old IE support  
        var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
        delta = delta > 0 ? 1 : delta < 0 ? -1 : 0;
        game.scene.scale = game.scene.scale * Math.exp(0.1*delta);
    };
    
    if (this.scene.canvas.addEventListener) {  
        // IE9, Chrome, Safari, Opera  
        this.scene.canvas.addEventListener("mousewheel", MouseWheelHandler, false);  
        // Firefox  
        this.scene.canvas.addEventListener("DOMMouseScroll", MouseWheelHandler, false);
    }
    
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
     var game = this;
     
	 this.server.onerror = function(evt)
	 {
		window.console.log("error");
	 };
	 this.server.onmessage = function (evt) 
	 {
    	// game.serverMessage.innerHTML = evt.data;
    	
		var message = eval(evt.data);


		if (message.clientPlayerId !== undefined)
		    scene.clientPlayerId = message.clientPlayerId;


		if (message.players !== undefined)
		    for (var i=0; i<message.players.length; ++i)
		    {
		        // TODO could use this information to update trails
		        
		        var msgplayer = message.players[i];
		        if (scene.player_list[msgplayer.id] === undefined)
        		    scene.player_list[msgplayer.id] = new Player(msgplayer.id, msgplayer.color);
        		if (msgplayer.status == 'disconnected')
        		{
        		    delete(scene.player_list[msgplayer.id]);
        		    continue;
		        }
		        
		        var player = scene.player_list[msgplayer.id];
		        player.pos = msgplayer.pos;
		        player.status = msgplayer.status;
		    }


		if (message.newTrails !== undefined)
		    for (var i=0; i<message.newTrails.length; i++)
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
                
                scene.blockPainter(trail.blockX, trail.blockY, function(ctx) {
                    drawKurv(ctx, plotp, plotcolor);
                });
		    }


        if (scene.clientPlayerId !== undefined && scene.player_list[scene.clientPlayerId] !== undefined)
            scene.draw();
            
		server.send(""); // Not always necessary
	 };
	 this.server.onclose = function()
	 {
    	game.serverMessage.innerHTML = "Connection to server lost. Reload the page to try again";
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
    this.scale = 1;
    this.camera = [0, 0];
    this.blocks = [[]];
    this.clientPlayerId = undefined;
    this.player_list = [];

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
	var wantedPos = this.player_list[ this.clientPlayerId ].pos;
	this.camera[0] = this.camera[0] + (wantedPos[0]-this.camera[0])*0.01;
	this.camera[1] = this.camera[1] + (wantedPos[1]-this.camera[1])*0.01;
	this.context.translate(-this.camera[0], -this.camera[1]);
	
	// Draw all prerendered image blocks
	for (var x=Math.floor((this.camera[0] - this.context.canvas.width/(2*this.scale))/this.blockSize);
	         x<=Math.ceil((this.camera[0] + this.context.canvas.width/(2*this.scale))/this.blockSize); x++)
	for (var y=Math.floor((this.camera[1] - this.context.canvas.height/(2*this.scale))/this.blockSize);
	         y<=Math.ceil((this.camera[1] + this.context.canvas.height/(2*this.scale))/this.blockSize); y++)
    {
        var block = this.getBlock(x,y);
        if (block === undefined)
            continue;

        this.context.drawImage(block.preRendered, x*this.blockSize, y*this.blockSize);
    }

    // Draw all players
    for (var playeri in this.player_list)
    {
        var player = this.player_list[playeri];
        player.render(this.context);
    }
};


Scene.prototype.getBlockCoordinates = function(p) {
    return [Math.floor(p[0]/this.blockSize), Math.floor(p[1]/this.blockSize)];
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


Scene.prototype.blockPainter = function(X,Y,f) {
    for(var x = X-1; x <= X+1; ++x)
        for(var y = Y-1; y <= Y+1; ++y)
    {
        f(this.getBlockOrCreate(x,y).preRenderedContext);
    }
};


function Player(id,color) {
    this.pos = [0,0];
    this.id = id;
    this.color = color;
    this.status = '';
};


Player.prototype.render = function(ctx) {
	ctx.beginPath();
	ctx.strokeStyle = this.color;
    ctx.arc(this.pos[0], this.pos[1], 5, 0 , 2 * Math.PI, false);
	ctx.fill();
};

