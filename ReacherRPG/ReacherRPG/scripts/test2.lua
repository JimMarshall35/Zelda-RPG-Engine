GameObject = {
	x = 0,
	y = 3,
	host = 0,
	update = function( self,delta,keys )
		--print("lua object 2 x: ".. self.x)
		self.x = self.x + 1
	end,
	init = function( self )
		--print("object2 initialized")
	end
}
