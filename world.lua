ground = {
	Position = {-10000, -1},
	CollisionBox = {
		pos = {-10000, -1},
		size = {20000, 1}
	}
}

quitControls = {
	Input = {
		{
			{Event.KeyPressed(Key.Q), Event.WindowClosed},
			exitGame
		}
	},
}

spawn(ground)
spawn(quitControls)
