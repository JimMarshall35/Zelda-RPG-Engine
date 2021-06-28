
--Just an implementation to test
host            = nil
local player_hp = 5
local toDraw    = {}
local lastkeys  = 0
local paused    = false

local pause_text = {
	text = "paused", 
	x    = 450,
	y    = 500,
	scale = 3,
	shouldDraw = true
}

function onNotify(event)
	if event.type == "health_set" then
		player_hp = event.data
	end
	setHeartsVisibility()
end
function init()
	loadFont(host,"fonts/Final_Fantasy_VII.ttf")
	loadUISprite(host,"Spritesheet/heart pixel art 32x32.png", "heart")
	loadUISprite(host,"Spritesheet/msg_box_1.png", "msgbox")
	setupNormalUI()
end
function update(delta,keys)
	-- body
	local newkeys = keys & (~lastkeys)
	if newkeys == 0 and keys ~= lastkeys then
		newkeys = keys;                    
	end
	lastkeys = keys
	if newkeys & (1 << INPUT.PAUSE_BIT) > 0 then
		local t = {}
		if togglePause(host) then
			table.insert(t,pause_text)
			print(t)
			setTextToDraw(host,t)
		else
			clearTextToDraw(host)
		end

	end
end
function setupNormalUI( )
	
	local startx = 25;
	local incr   = 32;
	for i=1,player_hp do
		local sprite = {
			name = "heart", 
			x    = startx + ((i-1) * incr),
			y    = 960,
			scale = 1,
			shouldDraw = true
		}
		table.insert(toDraw,sprite)
	end	
	setSpritesToDraw(host,toDraw)

end

function setHeartsVisibility()
	local onindex = 0;
	for k,v in pairs(toDraw) do 
		if k > player_hp then
			v.shouldDraw = false
		else 
			v.shouldDraw = true
		end
	end
	setSpritesToDraw(host,toDraw)
end