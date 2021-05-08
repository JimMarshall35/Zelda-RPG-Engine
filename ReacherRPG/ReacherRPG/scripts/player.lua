GameObject = {
	UP_BIT = 0,
	DOWN_BIT = 1,
	LEFT_BIT = 2,
	RIGHT_BIT = 3,
	ZOOM_IN_BIT = 4,
	ZOOM_OUT_BIT = 5,
	TEST_SPEED = 0.3,
	TEST_ZOOM_SPEED  = 2,

	NONE = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4,

	x = 0,
	y = 3,
	lastkeys = 0,
	direction = "none",
	cam_zoom = 0.0,

	host = 0,
	
	update = function( self,delta,keys )
		self.x, self.y = getPos(self.host)
		local velx = 0
		local vely = 0

		local newkeys = keys & (~self.lastkeys)

		if newkeys == 0 and keys ~= self.lastkeys then
			newkeys = keys;                    
		end

		if newkeys & (1 << self.UP_BIT) > 0 then
			self.direction = self.UP
		elseif newkeys & (1 << self.DOWN_BIT) > 0 then
			self.direction = self.DOWN
		elseif newkeys & (1 << self.LEFT_BIT) > 0 then
			self.direction = self.LEFT
		elseif newkeys & (1 << self.RIGHT_BIT) > 0 then
			self.direction = self.RIGHT
		end

		if (keys & (1 << self.UP_BIT) == 0) and
			(keys & (1 << self.DOWN_BIT) == 0) and
		   	(keys & (1 << self.LEFT_BIT) == 0) and
		   	(keys & (1 << self.RIGHT_BIT) == 0) then
		   	self.direction = self.NONE
		end

		if self.direction == self.NONE then
			velx = 0
		   	vely = 0
		   	animatorStop(self.host)
		elseif self.direction == self.UP then
			velx = 0
			vely = 1
			setAnimation(self.host, "walk_up")
			animatorStart(self.host)
		elseif self.direction == self.DOWN then
			velx = 0
			vely = -1
			setAnimation(self.host, "walk_down")
			animatorStart(self.host)
		elseif self.direction == self.LEFT then
			velx = -1
			vely = 0
			setAnimation(self.host, "walk_left")
			animatorStart(self.host)
		elseif self.direction == self.RIGHT then
			velx = 1
			vely = 0
			setAnimation(self.host, "walk_right")
			animatorStart(self.host)
		end

		velx = velx * delta * self.TEST_SPEED
		vely = vely * delta * self.TEST_SPEED
		
		setVelocity(self.host, velx, vely)

		if keys & (1<<self.ZOOM_IN_BIT) > 0 then
			self.cam_zoom = self.cam_zoom + (self.TEST_ZOOM_SPEED*delta)
			setCamZoom(self.cam_zoom)
		elseif keys & (1<<self.ZOOM_OUT_BIT) > 0 then
			self.cam_zoom = self.cam_zoom - (self.TEST_ZOOM_SPEED*delta)
			if self.cam_zoom < 1.0 then
				self.cam_zoom = 1.0
			end
			setCamZoom(self.cam_zoom)
		end
		setCamClamped(self.x,self.y)

		self.lastkeys = keys
	end,
	init = function( self )
		print("this is player.lua");
 		local tileset = getTilesetByName(self.host,"24by24ModernRPGGuy")
 		pushAnimation(self.host, "walk_up", tileset, 10, true, {0,1,2,3})
 		pushAnimation(self.host, "walk_down", tileset, 10, true, {4,5,6,7})
 		pushAnimation(self.host, "walk_right", tileset, 10, true, {8,9,10,11})
 		pushAnimation(self.host, "walk_left", tileset, 10, true, {12,13,14,15})
 		setAnimation(self.host, "walk_down")
 		animatorStop(self.host)
 		self.cam_zoom = getCamZoom();
	end,
	onInteract = function ( self,other )

	end
}