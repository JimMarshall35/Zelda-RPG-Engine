GameObject = {
	x = 0,
	y = 3,
	host = 0,
	update = function( self,delta,keys )
		self.x = self.x + 1
	end,
	init = function( self )
		print("this is test2.lua");
	end,
	onInteract = function ( self,other )

	end
}
