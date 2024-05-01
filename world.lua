ground = {
	CollisionBox = {
		size = {20000, 1}
	}
}
groundSize = ground.CollisionBox.size
spawn(ground, {-groundSize[1] / 2, -groundSize[2]})

quitControls = {
	Input = {
		{
			{Event.KeyPress(Key.Q), Event.WindowClose},
			exitGame
		}
	},
}
spawn(quitControls)

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
spawn(player)

platformBuilder = {
	Input = {
		{
			{Event.MouseButtonPress(MouseButton.Left)},
			function(this, event)
				if findEntity("platformInConstruction") then
					return
				end
				local platform = {
					HUD = {
						pos = mapPixelToHud(event.mouseButton),
						color = Color.Black
					},
					Name = "platformInConstruction"
				}
				spawn(platform)
			end
		},
		{
			{Event.MouseMove},
			function(this, event)
				local platform = findEntity("platformInConstruction")
				if not platform then
					return
				end
				local hud = platform.hud
				hud:resize(mapPixelToHud(event.mousePos) - hud.pos)
			end
		},
		{
			{Event.MouseButtonRelease(MouseButton.Left)},
			function(this, event)
				local platform = findEntity("platformInConstruction")
				if not platform then
					return
				end

				local pos = mapHudToWorld(platform.hud.pos)
				local size = mapPixelToWorld(event.mouseButton) - pos
				platform:add(Component.Position, pos)
				platform:add(Component.CollisionBox, { size = size })
				platform:add(Component.Render, { size = size, color = Color.Black })
				platform:remove(Component.HUD)
				platform:remove(Component.Name)
			end
		}
	}
}
spawn(platformBuilder)
