///////////////////////////
//
// To select a server, browse to "index.html?host=192.168.1.1&port=10001"
//
///////////////////////////

var game;

function init(){	
	var scene = new Scene(document.getElementById("myCanvas"));
	game = new Game(scene);
	
	sheepNr=Math.floor((Math.random()*1000000)+1); 
	document.getElementById("nameInput").value="Sheep"+sheepNr;
}


function startGame(){
    game.start();
    var playbutton = document.getElementById("playbutton");
    playbutton.style.display = "none";
}


function Game(scene) {
	this.sheep_death_toll = document.getElementById("sheep_death_toll");
    this.count_death=0;
    this.serverMessage = document.getElementById("serverMessage");
    this.serverMessage.innerHTML = "Achtung, die Schafe... Press play then space to start!";
    this.scene = scene;

	this.loadImage(scene.context,"welcome.png",[-400, -300]);
}

Game.prototype.loadImage = function(ctx,url,offset){
	this.ctx = ctx;
	this.url = url;
	try {
		var imageObj = new Image();
		imageObj.onload = function(){
			ctx.drawImage(this, offset[0], offset[1]);
		};
		imageObj.src = url;
	} catch (err) {}
}



Game.prototype.start = function() {
    this.scene.canvas.focus();
    var game = this;
    
	this.scene.canvas.onkeydown = function(evt) {
	    //evt = evt || window.event;
	    var keyCode = evt.keyCode || evt.which,
		    arrow = {left: 37, right: 39 }, $status = $('#status'),
		    spacebar = 32;

	    switch (keyCode) {
	    case arrow.left:
		    window.console.log('leftdown!');
		    game.server.send("leftdown");
	    break;
	    case arrow.right:
		    window.console.log('rightdown!');
		    game.server.send("rightdown");
	    break;
	    case spacebar:
		    window.console.log('spacedown!');
		    game.server.send("spacedown");
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

    function getParameterByName(name)
    {
      name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
      var regexS = "[\\?&]" + name + "=([^&#]*)";
      var regex = new RegExp(regexS);
      var results = regex.exec(window.location.search);
      if(results == null)
        return undefined;
      else
        return decodeURIComponent(results[1].replace(/\+/g, " "));
    }

    var host = getParameterByName('host');
    var port = getParameterByName('port');

    if (host === undefined)
        host = '192.168.1.5';

    if (port === undefined)
        port = '10001';

    var SERVER_ADDRESS = 'ws://' + host + ':' + port;

    this.server = new WebSocket(SERVER_ADDRESS);

    var server = this.server;
    var scene = this.scene;
    var game = this;

    this.server.onerror = function(evt)
    {
        window.console.log("error");
    };
    this.server.onmessage = function (evt)
    {
		//window.console.log("WebSocket: " + evt.data);
		//var message = eval('(' + evt.data + ')');
        var message;
        try {
            message = JSON.parse(evt.data);
        } catch (err) {
            window.console.log("WebSocket couldn't parse json data. " + err);
            window.console.log("Invalid JSON data was: \"" + evt.data + "\"");
            return;
        }

		if (message.serverMessage !== undefined)
		    game.serverMessage.innerHTML = message.serverMessage;
		    
		if (message.serverAlert !== undefined)
		    alert(message.serverAlert);

		if (message.clientPlayerId !== undefined)
		    scene.clientPlayerId = message.clientPlayerId;

		if (message.playerDisconnected !== undefined)
		{
		    delete(scene.player_list[message.playerDisconnected]);
        }

		if (message.players !== undefined)
		{
		    var htmlscore="Scoreboard:<br/>";
		    for (var i=0; i<message.players.length; ++i)
		    {
		        var msgplayer = message.players[i];
		        if (scene.player_list[msgplayer.id] === undefined)
        		    scene.player_list[msgplayer.id] = new Player(msgplayer.id, msgplayer.color);
		        
		        var player = scene.player_list[msgplayer.id];
		        player.pos = msgplayer.pos;
		        player.alive = msgplayer.alive;
		        player.score = msgplayer.score;
		        htmlscore += "<span style='color:" + player.color + "'>" + msgplayer.name + "</span>: " + player.score;
		        if (!player.alive)
    		        htmlscore += " (observer)";
		        htmlscore += "<br/>";
		    }
            document.getElementById("scoreboard").innerHTML = htmlscore;
        }
		/*
		if (message.players !== undefined)
		{
		    var htmlscore="Scoreboard:<br/><ol id="score_list">";
		    for (var i=0; i<message.players.length; ++i)
		    {
		        var msgplayer = message.players[i];
		        if (scene.player_list[msgplayer.id] === undefined)
        		    scene.player_list[msgplayer.id] = new Player(msgplayer.id, msgplayer.color);
		        
		        var player = scene.player_list[msgplayer.id];
		        player.pos = msgplayer.pos;
		        player.alive = msgplayer.alive;
		        player.score = msgplayer.score;
		        htmlscore += "<li><span style='color:" + player.color + "'>" + msgplayer.name + "</span>: " + player.score;
		        if (!player.alive)
    		        htmlscore += " (observer)";
		        htmlscore += "</li><br/>";
		    }
            document.getElementById("scoreboard").innerHTML = htmlscore+"</ol>";
			
			var score_list = document.getElementById("score_list");
			
			var tmpAry = new Array();
			for (var i=0;i<score_list.length;i++) {
					tmpAry[i] = new Array();
					tmpAry[i][0] = selElem.options[i].text;
			}
			tmpAry.sort();
			while (selElem.options.length > 0) {
				selElem.options[0] = null;
			}
			for (var i=0;i<tmpAry.length;i++) {
					var op = new Option(tmpAry[i][0], tmpAry[i][1]);
					selElem.options[i] = op;
			}
			return;

			
			
        }
		*/

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
		
		if (message.deathBySheep !== undefined){
			game.serverMessage.innerHTML = "Told you to stay in the fight... You got trampled by a sheep!<p></p><p></p><p><strong>Press space to restart!</strong></p>";
			max_death=10;
			if (game.count_death<=max_death){
				game.sheep_death_toll.innerHTML += '<img src="deathBySheep.png" alt="SheepKilledYou" width="80" height="60"/> ';
				game.count_death++;
			}
			game.loadImage(scene.context,"deathBySheep.png",[171, 189]);
		}
		
		if (message.deathByWall !== undefined){
			game.serverMessage.innerHTML = "Straight into the wall... Press space to try again<p></p><p></p><p>Hint: you're an observer... try scrolling!";
		}		
		
        if (scene.clientPlayerId !== undefined && scene.player_list[scene.clientPlayerId] !== undefined)
        {
            if (scene.queuedDrawing===undefined)
            {
                // By setting the timeout to 0 milliseconds this will execute as soon as websockets is finished with all queued messages
                scene.queuedDrawing = window.setTimeout("game.scene.draw();game.scene.queuedDrawing=undefined",0);
                if (scene.skippedFrames > 0)
                {
		            window.console.log("Skipped " + scene.skippedFrames + " frames");
		            scene.skippedFrames = 0;
	            }
            }
            else
            {
                scene.skippedFrames++;
            }
        }
            
		server.send(""); // Not always necessary
	 };
	 this.server.onclose = function()
	 {
    	game.serverMessage.innerHTML = "Connection to server lost. Reload the page to try again";
		window.console.log("Connection is closed..."); 
	 };
	 this.server.onopen = function()
	 {
		//alert($('<div/>').text(document.getElementById("nameInput").value).html());
		server.send('name=' + $('<div/>').text(document.getElementById("nameInput").value).html());
		document.getElementById("form").style.display = "none";
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

    this.skippedFrames = 0;
    this.queuedDrawing = undefined;

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
	if (this.player_list[ this.clientPlayerId ].alive)
	    this.scale = 1;
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
    this.alive = false;
    this.isSelf = false;
    this.score = 0;
};


Player.prototype.render = function(ctx) {
    if (this.alive)
    {
	    ctx.beginPath();
	    ctx.strokeStyle = this.color;
        ctx.arc(this.pos[0], this.pos[1], 5, 0 , 2 * Math.PI, false);
	    ctx.fill();
	
	    //if (!this.isSelf)
        //	ctx.strokeText(this.id, this.pos[0], this.pos[1] );
    }
};

