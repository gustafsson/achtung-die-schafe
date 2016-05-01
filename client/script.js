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

function isMobile() {
    var index = navigator.appVersion.indexOf("Mobile");
    return (index > -1);
}

function startGame(){
    game.start();
    if (isMobile()){
        fullScreen();
    }
    document.getElementById("playbutton").style.display = "none";
    document.getElementById("form").style.display = "none";
}


function Game(scene) {
	this.sheep_death_toll = document.getElementById("sheep_death_toll");
    this.count_death=0;
    this.serverMessage = document.getElementById("serverMessage");
    this.serverMessage.innerHTML = "Achtung, die Schafe... Press play then space to start!";
    this.scene = scene;

	this.loadImage(scene.context,"welcome.png",[-400, -300]);
}

function fullScreen(){
    document.getElementById("myCanvas").webkitRequestFullScreen();
    document.getElementById("myCanvas").mozRequestFullScreen()
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

    var updatePlayerAction = function() {
        var player = game.scene.player_list[game.scene.clientPlayerId];
        if (player===undefined) // Not initiated
            return;

        if (game.scene.keys[37] === game.scene.keys[39])
            player.action = '';
        else if (game.scene.keys[37])
            player.action = 'l';
        else if (game.scene.keys[39])
            player.action = 'r';
    }

    // Listen to touch events and send to server
    game.scene.canvas.addEventListener('touchstart', function(event) {
        var player = game.scene.player_list[game.scene.clientPlayerId];
        if (event.targetTouches.length === 1)
        {
            var touch = event.targetTouches[0];

            if (player.alive === true)
            {
                if (touch.pageX > game.scene.canvas.style.left+game.scene.canvas.width/2)
                    keyCode = 39;
                else
                    keyCode = 37;
            }
            else
                keyCode = 32;

            game.server.send('+'+keyCode);
            game.scene.keys[keyCode] = false;

            updatePlayerAction();
        }
    }, false);

    // Listen to touch events and send to server
    game.scene.canvas.addEventListener('touchend', function(event) {
        game.server.send('-'+39);
        game.server.send('-'+37);
        updatePlayerAction();
    }, false);

	this.scene.canvas.onkeydown = function(evt) {
	    //evt = evt || window.event;
        evt.preventDefault();

	    var keyCode = evt.keyCode || evt.which;
	    game.server.send('+'+keyCode);

        game.scene.keys[keyCode] = true;
        updatePlayerAction();
    };

	this.scene.canvas.onkeyup = function(evt) {
	    //evt = evt || window.event;
        evt.preventDefault();

	    var keyCode = evt.keyCode || evt.which;
	    game.server.send('-'+keyCode);

        game.scene.keys[keyCode] = false;
        updatePlayerAction();
    };

    var MouseWheelHandler = function(e) {
        var e = window.event || e; // old IE support
        var delta = Math.max(-1, Math.min(1, (e.wheelDelta || -e.detail)));
        delta = delta > 0 ? 1 : delta < 0 ? -1 : 0;
        game.scene.scale = game.scene.scale * Math.exp(0.1*delta);

        e.preventDefault();
    };

    if (this.scene.canvas.addEventListener) {
        // IE9, Chrome, Safari, Opera
        this.scene.canvas.addEventListener("mousewheel", MouseWheelHandler, false);
        // Firefox
        this.scene.canvas.addEventListener("DOMMouseScroll", MouseWheelHandler, false);
    }

    this.scene.canvas.onmousedown = function(evt) {
        game.scene.clickPos = [evt.clientX, evt.clientY];
		//window.console.log("onmousedown: " + evt.clientX + "," + evt.clientY);
    };
    this.scene.canvas.onmousemove = function(evt) {
        var player = game.scene.player_list[game.scene.clientPlayerId];
        if (player.alive === false){
            if (game.scene.clickPos === undefined)
                return;

            var d = [(evt.clientX - game.scene.clickPos[0])/game.scene.scale,
                     (evt.clientY - game.scene.clickPos[1])/game.scene.scale];
            game.scene.clickPos = [evt.clientX, evt.clientY];
            game.scene.camera[0] -= d[0];
            game.scene.camera[1] -= d[1];
            var msg = 'm' + d[0] + "," + d[1];
            game.server.send(msg);
            //window.console.log("onmousemove: " + msg);
        }
    };
    this.scene.canvas.onmouseup = function(evt) {
        game.scene.clickPos = undefined;
		//window.console.log("onmouseup: " + evt.clientX + "," + evt.clientY);
    };
    this.scene.canvas.onmouseout = this.scene.canvas.onmouseup;

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
        host = '82.115.196.222';

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
		    scene.text_to_display=message.serverAlert;

		if (message.clientPlayerId !== undefined)
		    scene.clientPlayerId = message.clientPlayerId;

		if (message.playerDisconnected !== undefined)
		{
		    delete(scene.player_list[message.playerDisconnected]);
        }

		if (message.players !== undefined)
		{
		    var scoreChanged = false;

			$.each(message.players, function(n, msgplayer) {
		        if (msgplayer === undefined)
		            return;

		        if (scene.player_list[msgplayer.id] === undefined) {
                    scene.player_list[msgplayer.id] = new Player(msgplayer.id, msgplayer.color);
                }

		        var player = scene.player_list[msgplayer.id];
                if (msgplayer.pos !== undefined) {
                    if (player.lastPos !== undefined && player.gap === false && player.alive === true) {
                        var plotp = [player.lastPos, msgplayer.pos];
				        var plotcolor = player.color;
                        var blockCoord = scene.getBlockCoordinates(msgplayer.pos);

                        scene.blockPainter(blockCoord[0], blockCoord[1], function(ctx) {
                            drawKurv(ctx, plotp, plotcolor);
                        });

                    }
		            player.pos = msgplayer.pos.slice();
                    player.lastPos = msgplayer.pos.slice();
                }
		        if (msgplayer.gap !== undefined)    player.gap = msgplayer.gap;
		        if (msgplayer.dir !== undefined)    player.dir = msgplayer.dir;
		        if (msgplayer.action !== undefined) player.action = msgplayer.action;
		        if (msgplayer.alive !== undefined)  {player.alive = msgplayer.alive; scoreChanged = true;}
		        if (msgplayer.score !== undefined)  {player.score = msgplayer.score; scoreChanged = true;}
		        if (msgplayer.name !== undefined)   {player.name = msgplayer.name; scoreChanged = true;}
		        if (msgplayer.color !== undefined)  {player.color = msgplayer.color; scoreChanged = true;}
		    });

            if (scoreChanged)
            {
                var htmlscore="Scoreboard:<br/>";
                var sortedPlayers = [];
                $.each(scene.player_list, function(n,player) {
                    sortedPlayers.push(player);
                });
                sortedPlayers.sort(function(a,b){return a.score<b.score;});
                $.each(sortedPlayers, function(n,player) {
                    if (player===undefined)
                        return;

                    htmlscore += "<span style='color:" + player.color + "'>" + player.name + "</span>: " + player.score;
                    if (!player.alive)
	                    htmlscore += " (observer)";
                    htmlscore += "<br/>";
                });
                document.getElementById("scoreboard").innerHTML = htmlscore;
            }
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

		if (message.deathBySheep !== undefined){
			game.serverMessage.innerHTML = "Told you to stay in the fight... You got trampled by a sheep!<p></p><p></p><p><strong>Press space to restart!</strong></p>";
			max_death=10;
			if (game.count_death<=max_death){
				game.sheep_death_toll.innerHTML += '<img src="deathBySheep.png" alt="SheepKilledYou" width="80" height="60"/> ';
				game.count_death++;
			}
			game.loadImage(scene.context,"deathBySheep.png",[171, 189]);
            sendMessage('Lonely, I\'m so lonely');
		}

		if (message.deathByWall !== undefined){
			game.serverMessage.innerHTML = "Straight into the wall... Press space to try again<p></p><p></p><p>Hint: you're an observer... try pressing up and down arrows!";
            sendMessage('Oups!');
		}

        if (scene.clientPlayerId !== undefined && scene.player_list[scene.clientPlayerId] !== undefined)
        {
            if (scene.queuedDrawing===undefined)
                scene.continiousDraw();
        }

		server.send(''); // Not always necessary, but works better on some browsers
	 };
	 this.server.onclose = function()
	 {
    	game.serverMessage.innerHTML = "Connection to server lost. Reload the page to try again";
    	if (game.scene.queuedDrawing !== undefined)
    	{
    	    // Stop drawing with a high framerate
    	    window.clearTimeout(game.scene.queuedDrawing);
	    }
		window.console.log("Connection is closed...");
	 };
	 this.server.onopen = function()
	 {
		//alert($('<div/>').text(document.getElementById("nameInput").value).html());
		server.send('name=' + document.getElementById("nameInput").value);
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
	this.context = this.canvas.getContext('2d');
	this.blockSize = 400;
    this.scale = 1;
    this.camera = [0, 0];
    this.blocks = {};
    this.clientPlayerId = undefined;
    this.player_list = {};
	this.text_to_display = "";
    this.keys = [];

    this.queuedDrawing = undefined;

	this.context.lineWidth=10;
	this.context.lineCap="round";

	this.context.translate(this.context.canvas.width/2, this.context.canvas.height/2);
	this.context.clearRect(0, 0, canvas.width, canvas.height);
}

Scene.prototype.continiousDraw = function() {
    var targetFps = 60;

    var start = new Date();
        game.scene.simulate(1/targetFps);
        game.scene.draw();
    var stop = new Date();

    var diff = Math.max(2, 1000/targetFps - (stop-start));
    game.scene.queuedDrawing = window.setTimeout(game.scene.continiousDraw, diff);
}

Scene.prototype.simulate = function(dt) {
    if (this.keys[38]) // up arrow
        this.scale = this.scale * Math.exp(0.1);
    if (this.keys[40]) // down arrow
        this.scale = this.scale * Math.exp(-0.1);
	//if (this.player_list[ this.clientPlayerId ].alive)
	//    this.scale = 1;
	this.scale = this.scale*0.99 + 1*0.01;
	this.scale = Math.max(0.01, Math.min(5, this.scale));

	var wantedPos = this.player_list[ this.clientPlayerId ].pos;
	var d = [(wantedPos[0]-this.camera[0])*this.scale, (wantedPos[1]-this.camera[1])*this.scale];
	this.camera[0] = this.camera[0] + d[0]*Math.min(0.1, 0.0000006*d[0]*d[0]);
	this.camera[1] = this.camera[1] + d[1]*Math.min(0.1, 0.0000006*d[1]*d[1]);

    // Do client prediction for all players
    $.each(this.player_list, function(n, player) {
        player.simulate(dt);
    });
}

Scene.prototype.draw = function() {

	// Reset the transformation matrix
	this.context.setTransform(1, 0, 0, 1, 0, 0);

	// Clean everything
	this.context.clearRect(0, 0, this.context.canvas.width, this.context.canvas.height);

	// Translate the whole scene according to the player's current location
	this.context.translate(this.context.canvas.width/2, this.context.canvas.height/2);
	this.context.scale(this.scale, this.scale);
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
    var context = this.context;
    $.each(this.player_list, function(n, player) {
        player.render(context);
    });

	if (this.text_to_display !== ""){
        this.context.setTransform(1, 0, 0, 1, 0, 0);
        this.context.scale(2, 2);
		this.context.fillText(this.text_to_display, 100, 100);
	}
};

//will probably need some kind of timer to display for a certain amount of time
Scene.prototype.writeOnCanvas = function(text) {
    this.context.fillText(text,100,100);
}

Scene.prototype.getBlockCoordinates = function(p) {
    return [Math.floor(p[0]/this.blockSize), Math.floor(p[1]/this.blockSize)];
}


Scene.prototype.getBlock = function(x,y) {
    return this.blocks[x + ':' + y];
};


Scene.prototype.setBlock = function(x,y,block) {
    this.blocks[x + ':' + y] = block;
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
    this.lastPos = undefined;
    this.action = ''; // 'l', 'r' or ''
    this.pos = [0,0];
    this.dir = 0;
    this.id = id;
	this.name = "";
    this.color = color;
    this.alive = false;
    this.isSelf = false;
    this.score = 0;
    this.gap = true;
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


Player.prototype.simulate = function(dt) {
    if (!this.alive)
        return;

    if (this.action=='l')
        this.dir -= 3.14*dt;
    if (this.action=='r')
        this.dir += 3.14*dt;

    this.pos[0] += dt*130*Math.cos(this.dir);
    this.pos[1] += dt*130*Math.sin(this.dir);
};

