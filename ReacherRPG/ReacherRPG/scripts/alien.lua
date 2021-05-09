GameObject = {

	collider = {
		left_offset = 0.0,
		right_offset = 0.0,
		top_offset = 24,
		bottom_offset = 0.0,
		pixelswidth = 24.0,
		pixelsheight = 32.0
	},
	x = 0,
	y = 0,
	current_direction = DIRECTION.NONE,
	move_timer = 0,
	change_after_seconds = 2,
	move_speed = 0.1,

	wait_time = 3,
	wait_timer = 0,
	enemies = {},

	HP = 5,

	update = function( self,delta,keys )
		if self.wait_timer >= self.wait_time then
			local velx = 0
			local vely = 0 
			self.move_timer = self.move_timer + delta 
			if self.move_timer >= self.change_after_seconds then
				self.move_timer = 0
				self.current_direction = self.randomDirection(self)
			end

			if     self.current_direction == DIRECTION.UP    then
				velx = 0
				vely = 1
			elseif self.current_direction == DIRECTION.DOWN  then
				velx = 0
				vely = -1
			elseif self.current_direction == DIRECTION.LEFT  then
				velx = -1
				vely = 0
			elseif self.current_direction == DIRECTION.RIGHT then
				velx = 1
				vely = 0
			end

			velx = velx * delta * self.move_speed
			vely = vely * delta * self.move_speed
				
			setVelocity(self.host, velx, vely)
		else
			self.wait_timer = self.wait_timer + delta
		end
	end,
	init = function( self )
		local tileset = getTilesetByName(self.host,"aliens")
 		local tile_w = 24
 		local tile_h = 24

 		local scalex = tile_w / SCREEN_WIDTH
 		local scaley = tile_h / SCREEN_HEIGHT

 		setScale(self.host,scalex,scaley)
 		pushAnimation(self.host, "walk_up",    tileset, 10, true, { 0,  1,  2  })
 		pushAnimation(self.host, "walk_left",  tileset, 10, true, { 12, 13, 14 })
 		pushAnimation(self.host, "walk_down",  tileset, 10, true, { 24, 25, 26 })
 		pushAnimation(self.host, "walk_right", tileset, 10, true, { 36, 37, 38 })


 		setAnimation(self.host, "walk_down")
 		
 		setCollidableBG(self.host, true)
 		setCollidableGO(self.host, true)
 		setDrawable(self.host, true)
 		setFloorCollider(self.host, self.collider)
 		setGOType(self.host, GO_TYPE.ENEMY)

 		animatorStart(self.host)

 		math.randomseed(os.time())
		math.random() math.random() math.random()
		self.current_direction = self.randomDirection(self)



	end,
	onInteract = function ( self,other )
		if getGOType(other) == GO_TYPE.PLAYER then
			local mydx, mydy = getVelocity(self.host)
			local dx, dy = getVelocity(other)
			local  x,  y = getPos(other)
			setPos(other, x + (-dx) + mydx, y + (-dy) + mydy)
			local player_lua = getLuaObject(other)
			player_lua.onHit(player_lua)
		end

	end,
	randomDirection = function(self)
		local r = math.random(4)
		if     r==1 then
			setAnimation(self.host, "walk_up")
			return DIRECTION.UP
		elseif r==2 then
		    setAnimation(self.host, "walk_down")
			return DIRECTION.DOWN
		elseif r==3 then
			setAnimation(self.host, "walk_left")
			return DIRECTION.LEFT
		elseif r==4 then
			setAnimation(self.host, "walk_right")
			return DIRECTION.RIGHT
		end
	end,
	onHit = function (self)
		self.HP = self.HP - 1
		if self.HP <= 0 then
			print("I'm Dead")
			deleteGO(self.host)
		else
			print("HP: ", self.HP)
		end

	end
}