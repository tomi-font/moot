ground = {
	CollisionBox = {
		size = {20000, 1}
	}
}

quitControls = {
	Input = {
		{
			{Event.KeyPress(Key.Q), Event.WindowClose},
			exitGame
		}
	},
}

player = {
	Position = {0, 900},
	Render = {
		size = {100, 100},
		color = {0, 128, 0}
	},
	Move = {
		speed = 1000
	},
	CollisionBox = {
		size = {100, 100}
	},
	Rigidbody = {},
	View = {
		size = {800, 450},
		limits = {{-800, 0}, {1600, 900}}
	},
	Input = {
		{
			{Event.KeyPress(Key.D), Event.KeyPress(Key.A),
			 Event.KeyRelease(Key.D), Event.KeyRelease(Key.A)},
			function(this)
				move = isKeyPressed(Key.D) - isKeyPressed(Key.A)
				this.move:setXMotion(move)
			end
		},
		{
			{Event.KeyPress(Key.W), Event.KeyPress(Key.S)},
			function(this, event)
				dir = (event.key.code == Key.W) and 1 or -1
				this.rigidbody:applyYForce(dir * 750)
			end
		},
		{
			{Event.MouseWheelScroll},
			function(this, event)
				this.view:zoom(1 - event.mouseWheel.delta / 4)
			end
		}
	}
}

groundSize = ground.CollisionBox.size
spawn(ground, {-groundSize[1] / 2, -groundSize[2]})

spawn(quitControls)

spawn(player)
