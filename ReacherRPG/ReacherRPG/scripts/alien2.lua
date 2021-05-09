GameObject = {

	collider = {
		left_offset = 6.0,
		right_offset = 6.0,
		top_offset = 24,
		bottom_offset = 0.0,
		pixelswidth = 24.0,
		pixelsheight = 32.0
	},
	x = 0,
	y = 0,
	current_direction = DIRECTION.NONE,
	move_timer = 0,
	change_after_seconds = 0.5,
	move_speed = 0.2,
	wait_time = 3,
	wait_timer = 0,
	player_ptr = 0,
	attack_distance = 0.1,

	update = function( self,delta,keys )
		self.x, self.y = getPos(self.host)
		local px, py = getPos(self.player_ptr)
		local mypos = { x = self.x, y = self.y}
		local playerpos = {x = px, y = py}

		if self.wait_timer >= self.wait_time then
			local me_2_player = vec2_sub(playerpos,mypos)

			local mag = vec2_mag(me_2_player)

			if mag <= self.attack_distance then

				me_2_player = vec2_normalize(me_2_player)

				self.setAnimation(self,me_2_player)

				me_2_player = vec2_scalar_mul(me_2_player, delta * self.move_speed)
				setVelocity(self.host, me_2_player.x, me_2_player.y)
			else
				setVelocity(self.host, 0, 0)
				animatorStop(self.host)
			end
		else
			self.wait_timer = self.wait_timer + delta
		end
	end,
	init = function( self )
		print("this is aliens")
		local tileset = getTilesetByName(self.host,"aliens")
 		local tile_w = 24
 		local tile_h = 24

 		local scalex = tile_w / SCREEN_WIDTH
 		local scaley = tile_h / SCREEN_HEIGHT

 		setScale(self.host,scalex,scaley)
 		pushAnimation(self.host, "walk_up",    tileset, 10, true, { 3,  4,  5  })
 		pushAnimation(self.host, "walk_left",  tileset, 10, true, { 15, 16, 17 })
 		pushAnimation(self.host, "walk_down",  tileset, 10, true, { 27, 28, 29 })
 		pushAnimation(self.host, "walk_right", tileset, 10, true, { 39, 40, 41 })


 		setAnimation(self.host, "walk_down")
 		
 		setCollidableBG(self.host, true)
 		--setCollidableGO(self.host, true)
 		setDrawable(self.host, true)
 		setFloorCollider(self.host, self.collider)
 		setGOType(self.host, GO_TYPE.ENEMY)

 		animatorStart(self.host)



		self.player_ptr = getPlayerPtr(self.host)
	end,
	onInteract = function ( self,other )
	--[[
		if getGOType(other) == GO_TYPE.PLAYER then
			local mydx, mydy = getVelocity(self.host)
			local dx, dy = getVelocity(other)
			local  x,  y = getPos(other)
			setPos(other, x + (-dx) + mydx, y + (-dy) + mydy)
		end
    ]]--
	end,

	setAnimation = function( self, dir )
		local compass = {
			[DIRECTION.UP]    = {x =  0, y =  1},
			[DIRECTION.DOWN]  = {x =  0, y = -1},
			[DIRECTION.LEFT]  = {x = -1, y =  0},
			[DIRECTION.RIGHT] = {x =  1, y =  0}
		}
		local max_dot = -math.huge
		local my_dir = 0
		for index, value in ipairs(compass) do
		    local dot = vec2_dot(dir, value)
		    if dot > max_dot then 
		    	max_dot = dot
		    	my_dir = index
		    end
		end
		if     my_dir == DIRECTION.UP    then
			setAnimation(self.host,"walk_up")
		elseif my_dir == DIRECTION.DOWN  then
			setAnimation(self.host,"walk_down")
		elseif my_dir == DIRECTION.LEFT  then
			setAnimation(self.host,"walk_left")
		elseif my_dir == DIRECTION.RIGHT then
			setAnimation(self.host,"walk_right")
		end
		animatorStart(self.host)
	end
}