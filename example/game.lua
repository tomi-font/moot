properties.clearColor = Color.Gray
properties.gravitationalAcceleration = 999.80665

local ground = {
	CollisionBox = {
		size = {20000, 1}
	}
}
local groundSize = ground.CollisionBox.size
spawn(ground, {-groundSize[1] / 2, -groundSize[2]})

local player = {
	Position = {0, 900},
	ConvexPolygon = {
		vertices = { {0, 0}, {100, 0}, {100, 100}, {0, 100} },
		fillColor = {0, 128, 0}
	},
	Move = {
		speed = 1000
	},
	CollisionBox = {
		size = {100, 100}
	},
	Rigidbody = {},
	View = {},
	Input = {
		{
			{InputEvent.KeyPress(Key.D), InputEvent.KeyPress(Key.A),
			 InputEvent.KeyRelease(Key.D), InputEvent.KeyRelease(Key.A)},
			function(this)
				move = isKeyPressed(Key.D) - isKeyPressed(Key.A)
				this:get(Component.Move):setXMotion(move)
			end
		},
		{
			{InputEvent.KeyPress(Key.W), InputEvent.KeyPress(Key.S)},
			function(this, event)
				dir = (event.key.code == Key.W) and 1 or -1
				this:get(Component.Rigidbody):applyYForce(dir * 750)
			end
		},
		{
			{InputEvent.MouseWheelScroll},
			function(this, event)
				this:get(Component.View):zoom(1 - event.mouseWheel.delta / 4)
			end
		}
	},
	onSpawn = function(this)
		local windowSize = properties.windowSize
		local aspectRatio = windowSize.x / windowSize.y
		local pos = this:get(Component.Position)
		local cView = this:get(Component.View)
		local width = pos.y * aspectRatio

		cView:setSize({width, width / aspectRatio})
		cView:setLimits({ {pos.x - width / 2, 0}, {width, width / aspectRatio} })
	end
}
spawn(player)

local platformBuilder = {
	Input = {
		{
			{InputEvent.MouseButtonPress(MouseButton.Left)},
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
			{InputEvent.MouseMove},
			function(this, event)
				local platform = findEntity("platformInConstruction")
				if not platform then
					return
				end
				local hud = platform:get(Component.HUD)
				hud:resize(mapPixelToHud(event.mousePos) - hud.pos)
			end
		},
		{
			{InputEvent.MouseButtonRelease(MouseButton.Left)},
			function(this, event)
				local platform = findEntity("platformInConstruction")
				if not platform then
					return
				end

				local pos = mapHudToWorld(platform:get(Component.HUD).pos)
				local size = mapPixelToWorld(event.mouseButton) - pos
				platform:add(Component.Position, pos)
				platform:add(Component.CollisionBox, { size = size })
				platform:add(Component.ConvexPolygon, { vertices = { {0, 0}, {size.x, 0}, size, {0, size.y} },
				                                        fillColor = Color.Black })
				platform:remove(Component.HUD)
				platform:remove(Component.Name)
			end
		}
	}
}
spawn(platformBuilder)