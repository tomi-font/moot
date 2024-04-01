ground = {
	CollisionBox = {
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

groundSize = ground.CollisionBox.size
spawn(ground, {-groundSize[1] / 2, -groundSize[2]})

spawn(quitControls)
