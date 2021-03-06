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
	activate_distance = 0.5,

	wait_time = 0.5,
	wait_timer = 0,
	enemies = {},

	hit = false,
	knockback_vec = {x=0,y=0},
	hittimer = 0,
	hittime = 0.2,
	hitstrength = 1,

	player_ptr = 0,

	HP = 2,

	update = function( self,delta,keys )
		self.x, self.y = getPos(self.host)
		local px, py = getPos(self.player_ptr)
		local mypos = { x = self.x, y = self.y}
		local playerpos = {x = px, y = py}

		if self.wait_timer >= self.wait_time then
			self.move_timer = self.move_timer + delta 
			local vel = {x=0, y=0}
			if self.hit == true then
				self.hittimer = self.hittimer + delta
				if self.hittimer >= self.hittime then
					self.hit = false
					self.HP = self.HP - 1
					if self.HP <= 0 then
						--print("I'm Dead")
						deleteGO(self.host)
					--else
						--print("HP: ", self.HP)
					end
				end
				vel = vec2_scalar_mul(self.knockback_vec,((self.hittime - self.hittimer) / self.hittime)*delta)
			elseif self.hit == false then
				local me_2_player = vec2_sub(playerpos,mypos)
				local mag = vec2_mag(me_2_player)

				if mag <= self.activate_distance then
					if self.move_timer >= self.change_after_seconds then
						self.move_timer = 0
						--self.randomDirection(self)
						self.change_after_seconds = math.random() * 3.0 
						self.setDirection(self)
						self.shoot(self)
					end
					if     self.current_direction == DIRECTION.UP    then
						--setAnimation(self.host, "walk_up")
						vel.y = 1
					elseif self.current_direction == DIRECTION.DOWN  then
						--setAnimation(self.host, "walk_down")
						vel.y = -1
					elseif self.current_direction == DIRECTION.LEFT  then
						--setAnimation(self.host, "walk_left")
						vel.x = -1
					elseif self.current_direction == DIRECTION.RIGHT then
						--setAnimation(self.host, "walk_right")
						vel.x = 1
					end

					vel = vec2_scalar_mul(vel,delta*self.move_speed)
						
					
				end
			end
			setVelocity(self.host, vel.x, vel.y)
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

		self.player_ptr = getPlayerPtr(self.host)

	end,
	onInteract = function ( self,other )
		if getGOType(other) == GO_TYPE.PLAYER then

			local mydx, mydy = getVelocity(self.host)
			local dx, dy = getVelocity(other)
			local  x,  y = getPos(other)
			setPos(other, x + (-dx) + mydx, y + (-dy) + mydy)
			local player_lua = getLuaObject(other)
			player_lua.onHit(player_lua,self)
		end

	end,
	randomDirection = function(self)
		local r = math.random(4)
		if     r==1 then
			self.current_direction = DIRECTION.UP
			setAnimation(self.host, "walk_up")
		elseif r==2 then
			self.current_direction = DIRECTION.DOWN
			setAnimation(self.host, "walk_up")
		elseif r==3 then
			self.current_direction = DIRECTION.LEFT
			setAnimation(self.host, "walk_left")
		elseif r==4 then
			self.current_direction = DIRECTION.RIGHT
			setAnimation(self.host, "walk_right")
		end
	end,
	setDirection = function(self)
		local playerptr = getPlayerPtr(self.host)
		local playerpos = {x=0,y=0}
		local mypos     = {x=self.x,y=self.y}
		playerpos.x,playerpos.y = getPos(playerptr)
		local me_2_player = vec2_sub(playerpos,mypos)
		self.current_direction = getNearestCardDir(me_2_player)
		if     self.current_direction == DIRECTION.UP    then
			setAnimation(self.host, "walk_up")
		elseif self.current_direction == DIRECTION.DOWN  then
			setAnimation(self.host, "walk_down")
		elseif self.current_direction == DIRECTION.LEFT  then
			setAnimation(self.host, "walk_left")
		elseif self.current_direction == DIRECTION.RIGHT then
			setAnimation(self.host, "walk_right")
		end
	end,
	onHit = function (self,other)
		if self.hit == false then

			local otherpos = {x = other.x, y = other.y}
			local mypos    = {x = self.x,  y = self.y }
			local other2me = vec2_sub(mypos,otherpos)
			other2me = vec2_normalize(other2me)
			self.knockback_vec = vec2_scalar_mul(other2me, self.hitstrength)
			self.hittimer = 0
			self.hit = true

			
		end

	end,
	shoot = function(self)
		local proj = createScriptableGO(self.host,"scripts/alien_projectile.lua",self.x,self.y)
		local proj_lua = getLuaObject(proj)
		proj_lua.setDirection(proj_lua,self.current_direction)	
	end
}