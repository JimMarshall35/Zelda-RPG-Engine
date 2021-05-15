GameObject = {

	collider = {
		left_offset = 0.0,
		right_offset = 0.0,
		top_offset = 0.0,
		bottom_offset = 0.0,
		pixelswidth = 16.0,
		pixelsheight = 16.0
	},
	x = 0,
	y = 0,

	lastx = 0,
	lasy = 0,

	direction = DIRECTION.NONE,
	speed = 0.5,
	range = 1.0,
	distance_traveled = 0,

	update = function( self,delta,keys )
		self.x, self.y = getPos(self.host)
		if self.lastx == self.x and self.lasty == self.y then
			deleteGO(self.host)
			return
		end
		self.lastx = self.x
		self.lasty = self.y
		local vel = {x=0, y=0}
		if 	   self.direction == DIRECTION.UP    then
			vel.y = 1
		elseif self.direction == DIRECTION.DOWN  then
			vel.y = -1
		elseif self.direction == DIRECTION.LEFT  then
			vel.x = -1
		elseif self.direction == DIRECTION.RIGHT then
			vel.x = 1
		end
		vel = vec2_scalar_mul(vel,delta*self.speed)
		self.distance_traveled = self.distance_traveled + vec2_mag(vel)
		setVelocity(self.host, vel.x, vel.y)
		if self.distance_traveled >= self.range then
			deleteGO(self.host)
		end
	end,
	init = function( self )
		local tileset = getTilesetByName(self.host,"alien_projectile")
 		local tile_w = 16
 		local tile_h = 16

 		local scalex = tile_w / SCREEN_WIDTH
 		local scaley = tile_h / SCREEN_HEIGHT

 		setScale(self.host,scalex,scaley)

 		pushAnimation(self.host, "main", tileset, 20, true, { 0, 1 })

 		setAnimation(self.host, "main")
 		
 		setCollidableBG(self.host, true)
 		--setCollidableGO(self.host, true)
 		setDrawable(self.host, true)
 		setFloorCollider(self.host, self.collider)
 		setGOType(self.host, GO_TYPE.ENEMY)

 		animatorStart(self.host)
	end,
	onInteract = function ( self,other )
		local other_type = getGOType(other)
		if  other_type == GO_TYPE.PLAYER then
			local player_lua = getLuaObject(other)
			player_lua.onHit(player_lua,self)
			
		end
		if other_type ~= GO_TYPE.ENEMY then
			deleteGO(self.host)
		end
	end,
	setDirection = function( self, direction )
		self.direction = direction
	end
}