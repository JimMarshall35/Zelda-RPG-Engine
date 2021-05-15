GameObject = {
	

	TEST_SPEED = 0.3,
	TEST_ZOOM_SPEED  = 2,

	
	x = 0,
	y = 3,
	lastkeys = 0,
	direction = 0,
	lastdirection = 0,
	cam_zoom = 0.0,
	attacking = false,
	canattack = true,
	enemies = {},
	hitrange = 0.15,
	HP = 5,

	hit = false,
	knockback_vec = {x=0,y=0},
	hittimer = 0,
	hittime = 0.2,
	hitstrength = 1,

	collider = {
		left_offset = 7.0,
		right_offset = 8.0,
		top_offset = 18.0,
		bottom_offset = 0.0,
		pixelswidth = 24.0,
		pixelsheight = 24.0
	},

	delta = 0,

	host = 0,
	setDirection = function( self,keys )
		local newkeys = keys & (~self.lastkeys)

		if newkeys == 0 and keys ~= self.lastkeys then
			newkeys = keys;                    
		end

		if newkeys & (1 << INPUT.UP_BIT) > 0 then
			self.direction = DIRECTION.UP
			setAnimation(self.host, "walk_up")
			animatorStart(self.host)
		elseif newkeys & (1 << INPUT.DOWN_BIT) > 0 then
			self.direction = DIRECTION.DOWN
			setAnimation(self.host, "walk_down")
			animatorStart(self.host)
		elseif newkeys & (1 << INPUT.LEFT_BIT) > 0 then
			self.direction = DIRECTION.LEFT
			setAnimation(self.host, "walk_left")
			animatorStart(self.host)
		elseif newkeys & (1 << INPUT.RIGHT_BIT) > 0 then
			self.direction = DIRECTION.RIGHT
			setAnimation(self.host, "walk_right")
			animatorStart(self.host)
		end

		if (keys & (1 << INPUT.UP_BIT) == 0) and
			(keys & (1 << INPUT.DOWN_BIT) == 0) and
		   	(keys & (1 << INPUT.LEFT_BIT) == 0) and
		   	(keys & (1 << INPUT.RIGHT_BIT) == 0) then
		   	if self.direction ~= self.NONE then
		   		self.lastdirection = self.direction
		   		animatorStop(self.host)
		   	end
		   	self.direction = self.NONE
		end
		self.lastkeys = keys

	end,
	update = function( self,delta,keys )
		self.delta = delta
		self.x, self.y = getPos(self.host)
		




		local vel = {x=0, y=0}
		self.setDirection(self,keys)

		if self.hit == true then
			self.hittimer = self.hittimer + delta
			if self.hittimer >= self.hittime then
				self.hit = false
			end
			vel = vec2_scalar_mul(self.knockback_vec,((self.hittime - self.hittimer) / self.hittime)*delta)

		elseif self.hit == false then
		
			if self.attacking == false then
				if self.direction == DIRECTION.UP then
					vel.y = 1
					
				elseif self.direction == DIRECTION.DOWN then
					vel.y = -1
					
				elseif self.direction == DIRECTION.LEFT then
					vel.x = -1
					
				elseif self.direction == DIRECTION.RIGHT then
					vel.x = 1
					
				end
				vel = vec2_scalar_mul(vel,self.TEST_SPEED*delta)

				

				if keys & (1<<INPUT.ZOOM_IN_BIT) > 0 then
					self.cam_zoom = self.cam_zoom + (self.TEST_ZOOM_SPEED*delta)
					setCamZoom(self.cam_zoom)
				elseif keys & (1<<INPUT.ZOOM_OUT_BIT) > 0 then
					self.cam_zoom = self.cam_zoom - (self.TEST_ZOOM_SPEED*delta)
					if self.cam_zoom < 1.0 then
						self.cam_zoom = 1.0
					end
					setCamZoom(self.cam_zoom)
				end

				if keys & (1<<INPUT.SELECT_BIT) > 0  and self.canattack == true then
					self.attacking = true
					self.canattack = false
					local direc = self.direction
					if self.direction == self.NONE then 
						direc = self.lastdirection 
					end

					if direc == DIRECTION.UP then
						setAnimation(self.host, "attack_up")
						animatorStart(self.host)
					elseif direc == DIRECTION.DOWN then
						setAnimation(self.host, "attack_down")
						animatorStart(self.host)
					elseif direc == DIRECTION.LEFT then
						setAnimation(self.host, "attack_left")
						animatorStart(self.host)
					elseif direc == DIRECTION.RIGHT then
						setAnimation(self.host, "attack_right")
						animatorStart(self.host)
					end
					self.lastdirection = direc
					self.detectAttackHits(self)
				elseif keys & (1<<INPUT.SELECT_BIT) == 0  and self.canattack == false then
					self.canattack = true
				end

				
			elseif self.attacking == true then
				local isanimating = getIsAnimating(self.host)
				--print(isanimating)
				if isanimating == false then
					self.attacking = false
					if self.lastdirection == DIRECTION.UP then
						setAnimation(self.host, "walk_up")
					elseif self.lastdirection == DIRECTION.DOWN then
						setAnimation(self.host, "walk_down")
					elseif self.lastdirection == DIRECTION.LEFT then
						setAnimation(self.host, "walk_left")
					elseif self.lastdirection == DIRECTION.RIGHT then
						setAnimation(self.host, "walk_right")
					end
					animatorStop(self.host)
				end
				vel = {x=0,y=0}
			end
		end
		setCamClamped(self.x,self.y)
		setVelocity(self.host,vel.x, vel.y)
		
	end,
	init = function( self )
 		local tileset = getTilesetByName(self.host,"24by24ModernRPGGuy_edit")
 		local tile_w = 24
 		local tile_h = 24

 		local scalex = tile_w / SCREEN_WIDTH
 		local scaley = tile_h / SCREEN_HEIGHT

 		setScale(self.host,scalex,scaley)

 		pushAnimation(self.host, "walk_up",    tileset, 10, true, { 0,  1,  2,  3})
 		pushAnimation(self.host, "walk_down",  tileset, 10, true, { 4,  5,  6,  7})
 		pushAnimation(self.host, "walk_right", tileset, 10, true, { 8,  9, 10, 11})
 		pushAnimation(self.host, "walk_left",  tileset, 10, true, {12, 13, 14, 15})

 		pushAnimation(self.host, "attack_up",    tileset, 12, false, {16, 17})
 		pushAnimation(self.host, "attack_down",  tileset, 12, false, {18, 19})
 		pushAnimation(self.host, "attack_right", tileset, 12, false, {20, 21})
 		pushAnimation(self.host, "attack_left",  tileset, 12, false, {22, 23})

 		setAnimation(self.host, "walk_down")
 		
 		setCollidableBG(self.host, true)
 		setCollidableGO(self.host, true)
 		setDrawable(self.host, true)
 		setFloorCollider(self.host, self.collider)
 		setGOType(self.host, GO_TYPE.PLAYER)
 		self.direction = DIRECTION.DOWN
 		self.lastdirection = DIRECTION.DOWN
 		self.cam_zoom = getCamZoom();
 		

	end,
	onInteract = function ( self,other )
		--print("collision!")
	end,
	detectAttackHits = function (self)
		self.enemies = getEnemiesPtrs(self.host)
		local mypos = {x=self.x,y=self.y}
		local no_enemies = 0 
		for k, enemy_ptr in pairs(self.enemies) do
			no_enemies = no_enemies + 1
		  	local enemypos = {x=0,y=0}
		  	enemypos.x, enemypos.y = getPos(enemy_ptr)
		  	local me_2_enemy = vec2_sub(enemypos,mypos)

		  	local direction = getNearestCardDir(me_2_enemy)
		  	if direction == self.lastdirection then
		  		if vec2_mag(me_2_enemy) <= self.hitrange then
		  			print("enemy hit!!")
		  			self.processAttackHit(self,enemy_ptr)
		  		end
		  	end
		end
	end,
	processAttackHit = function(self,enemy)
		local enemy_lua = getLuaObject(enemy)
		enemy_lua.onHit(enemy_lua,self)
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

			self.HP = self.HP - 1
			if self.HP <= 0 then
				print("PLAYER: I'm Dead")
				deleteGO(self.host)
			else
				print("PLAYER HP: ", self.HP)
			end
		end

	end

}