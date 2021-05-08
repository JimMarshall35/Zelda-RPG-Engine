GameObject = {
	UP_BIT = 0,
	DOWN_BIT = 1,
	LEFT_BIT = 2,
	RIGHT_BIT = 3,
	ZOOM_IN_BIT = 4,
	ZOOM_OUT_BIT = 5,
	SELECT_BIT = 6,

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
	direction = 0,
	lastdirection = 0,
	cam_zoom = 0.0,
	attacking = false,
	canattack = true,

	collider = {
		left_offset = 7.0,
		right_offset = 8.0,
		top_offset = 18.0,
		bottom_offset = 0.0,
		pixelswidth = 24.0,
		pixelsheight = 24.0
	},


	host = 0,
	
	update = function( self,delta,keys )
		--print(self.lastdirection)
		if self.attacking == false then
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
			   	if self.direction ~= self.NONE then
			   		self.lastdirection = self.direction
			   	end
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

			if keys & (1<<self.SELECT_BIT) > 0  and self.canattack == true then
				self.attacking = true
				self.canattack = false
				local direc = self.direction
				if self.direction == self.NONE then 
					direc = self.lastdirection 
				end

				if direc == self.UP then
					setAnimation(self.host, "attack_up")
					animatorStart(self.host)
				elseif direc == self.DOWN then
					setAnimation(self.host, "attack_down")
					animatorStart(self.host)
				elseif direc == self.LEFT then
					setAnimation(self.host, "attack_left")
					animatorStart(self.host)
				elseif direc == self.RIGHT then
					setAnimation(self.host, "attack_right")
					animatorStart(self.host)
				end
				self.lastdirection = direc
			elseif keys & (1<<self.SELECT_BIT) == 0  and self.canattack == false then
				self.canattack = true
			end

			setCamClamped(self.x,self.y)
		elseif self.attacking == true then
			local isanimating = getIsAnimating(self.host)
			if isanimating == false then
				self.attacking = false
				if self.lastdirection == self.UP then
					setAnimation(self.host, "walk_up")
				elseif self.lastdirection == self.DOWN then
					setAnimation(self.host, "walk_down")
				elseif self.lastdirection == self.LEFT then
					setAnimation(self.host, "walk_left")
				elseif self.lastdirection == self.RIGHT then
					setAnimation(self.host, "walk_right")
				end
				animatorStop(self.host)
			end
			setVelocity(self.host,0,0)
		end
		self.lastkeys = keys
	end,
	init = function( self )
		print("this is player.lua");
 		local tileset = getTilesetByName(self.host,"24by24ModernRPGGuy_edit")
 		pushAnimation(self.host, "walk_up", tileset, 10, true, {0,1,2,3})
 		pushAnimation(self.host, "walk_down", tileset, 10, true, {4,5,6,7})
 		pushAnimation(self.host, "walk_right", tileset, 10, true, {8,9,10,11})
 		pushAnimation(self.host, "walk_left", tileset, 10, true, {12,13,14,15})

 		pushAnimation(self.host, "attack_up", tileset, 12, false, {16,17})
 		pushAnimation(self.host, "attack_down", tileset, 12, false, {18,19})
 		pushAnimation(self.host, "attack_right", tileset, 12, false, {20,21})
 		pushAnimation(self.host, "attack_left", tileset, 12, false, {22,23})

 		setAnimation(self.host, "walk_down")
 		
 		setCollidableBG(self.host, true)
 		setCollidableGO(self.host, true)
 		setDrawable(self.host, true)
 		setFloorCollider(self.host,self.collider)
 		self.direction = self.DOWN
 		self.lastdirection = self.DOWN
 		self.cam_zoom = getCamZoom();
	end,
	onInteract = function ( self,other )
		print("collision!")
	end
}