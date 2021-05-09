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
	hitrange = 0.1,
	HP = 5,

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
		self.x, self.y = getPos(self.host)
		
		
		local velx = 0
		local vely = 0

		local newkeys = keys & (~self.lastkeys)

		if newkeys == 0 and keys ~= self.lastkeys then
			newkeys = keys;                    
		end

		if newkeys & (1 << INPUT.UP_BIT) > 0 then
			self.direction = DIRECTION.UP
		elseif newkeys & (1 << INPUT.DOWN_BIT) > 0 then
			self.direction = DIRECTION.DOWN
		elseif newkeys & (1 << INPUT.LEFT_BIT) > 0 then
			self.direction = DIRECTION.LEFT
		elseif newkeys & (1 << INPUT.RIGHT_BIT) > 0 then
			self.direction = DIRECTION.RIGHT
		end

		if (keys & (1 << INPUT.UP_BIT) == 0) and
			(keys & (1 << INPUT.DOWN_BIT) == 0) and
		   	(keys & (1 << INPUT.LEFT_BIT) == 0) and
		   	(keys & (1 << INPUT.RIGHT_BIT) == 0) then
		   	if self.direction ~= self.NONE then
		   		self.lastdirection = self.direction
		   	end
		   	self.direction = self.NONE
		end
		if self.attacking == false then
			if self.direction == self.NONE then
				velx = 0
			   	vely = 0
			   	animatorStop(self.host)
			elseif self.direction == DIRECTION.UP then
				velx = 0
				vely = 1
				setAnimation(self.host, "walk_up")
				animatorStart(self.host)
			elseif self.direction == DIRECTION.DOWN then
				velx = 0
				vely = -1
				setAnimation(self.host, "walk_down")
				animatorStart(self.host)
			elseif self.direction == DIRECTION.LEFT then
				velx = -1
				vely = 0
				setAnimation(self.host, "walk_left")
				animatorStart(self.host)
			elseif self.direction == DIRECTION.RIGHT then
				velx = 1
				vely = 0
				setAnimation(self.host, "walk_right")
				animatorStart(self.host)
			end

			velx = velx * delta * self.TEST_SPEED
			vely = vely * delta * self.TEST_SPEED
			
			setVelocity(self.host, velx, vely)

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

			setCamClamped(self.x,self.y)
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
			setVelocity(self.host,0,0)
		end
		self.lastkeys = keys
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
		enemy_lua.onHit(enemy_lua)
	end,
	onHit = function (self)
		self.HP = self.HP - 1
		if self.HP <= 0 then
			print("PLAYER: I'm Dead")
			--deleteGO(self.host)
		else
			print("PLAYER HP: ", self.HP)
		end

	end

}