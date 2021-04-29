
GameObject = {
	x = 0,
	y = 3,
	host = 0,
	timer = 0.0,
	spent = false,
	update = function( self,delta,keys )
		self.x = self.x + 3
		self.timer = self.timer + delta
		if self.timer > 20 and self.spent == false then
			enqueueMsgBoxes(self.host,"this is a message box triggered from lua")
			self.spent = true
		end

	end,
	init = function( self )
		print("this is test1");
	end,
	onInteract = function ( other )

	end
}
