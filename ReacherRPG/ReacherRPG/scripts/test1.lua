
GameObject = {
	x = 0,
	y = 3,
	host = 0,
	update = function( self,delta,keys )
		--print("lua object 1 x: ".. self.x)
		self.x = self.x + 3
	end,
	init = function( self )
		--print("object1 initialized")
	end
}
