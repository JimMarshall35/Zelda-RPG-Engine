--[[ 

enums and constants used in the c++ code.
Also some useful lua helper functions.
This file is made avaliable to all lua scripts.

]]--

GO_TYPE = {
	NONE = 0,
	PLAYER = 1,
	DOOR = 2,
	PICKUP = 3,
	ENEMY = 4,
	SCENERY = 5,
	ZONE = 6
}
INPUT = {
	UP_BIT = 0,
	DOWN_BIT = 1,
	LEFT_BIT = 2,
	RIGHT_BIT = 3,
	ZOOM_IN_BIT = 4,
	ZOOM_OUT_BIT = 5,
	SELECT_BIT = 6,
	PAUSE_BIT = 7
}
DIRECTION = {
	NONE = 0,
	UP = 1,
	DOWN = 2,
	LEFT = 3,
	RIGHT = 4
}

SCREEN_WIDTH = 640.0
SCREEN_HEIGHT = 640.0

function vec2_sub( v1,v2 )
	return {
		x = v1.x - v2.x,
		y = v1.y - v2.y
	}
end
function vec2_add( v1,v2 )
	return {
		x = v1.x + v2.x,
		y = v1.y + v2.y
	}
end
function vec2_mag( v )
	return math.sqrt((v.x*v.x)+(v.y*v.y))
end
function vec2_dot( v1,v2 )
	return v1.x*v2.x + v1.y*v2.y
end
function vec2_scalar_div( v,s )
	return {
		x = v.x / s,
		y = v.y / s
	}
end
function vec2_scalar_mul( v,s )
	return {
		x = v.x * s,
		y = v.y * s
	}
end
function vec2_normalize( v )
	local mag = vec2_mag(v)
	return{
		x = v.x / mag,
		y = v.y / mag
	}
end

function getNearestCardDir( dir )
	--[[
	returns the direction enum of the cardinal
	direction which  'dir' is nearest to in angle
	]]--
	local compass = {
		[DIRECTION.UP]    = {x =  0, y =  1},
		[DIRECTION.DOWN]  = {x =  0, y = -1},
		[DIRECTION.LEFT]  = {x = -1, y =  0},
		[DIRECTION.RIGHT] = {x =  1, y =  0}
	}
	local max_dot = -math.huge
	local ret_dir = 0
	for index, value in ipairs(compass) do
	    local dot = vec2_dot(dir, value)
	    if dot > max_dot then 
	    	max_dot = dot
	    	ret_dir = index
	    end
	end
	return ret_dir
end

function testBit( value,bit )
	return value & (1<<bit) > 0
end

