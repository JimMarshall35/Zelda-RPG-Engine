GameObject = {

	collider = {
		left_offset = 0.0,
		right_offset = 0.0,
		top_offset = 0.0,
		bottom_offset = 0.0,
		pixelswidth = 24.0,
		pixelsheight = 24.0
	},
	x = 0,
	y = 0,


	update = function( self,delta,keys )
	end,
	init = function( self )
	end,
	onInteract = function ( self,other )
		--print("collision!")
	end
}