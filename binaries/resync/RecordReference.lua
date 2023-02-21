-- LUA script to record a reference run to be used with the resync bruteforcer module
-- Currently only works on US ROMs

MARIO_STATE_ADDRESS = 0x8032d93c
GLOBAL_TIMER_ADDRESS = 0x8032D5D4
PLAYER1CONTROLLER_ADDRESS = 0x8032d5e4
POSITION_OFFSET = 0x3C
FACEANGLE_OFFSET = 0x2E
HSPEED_OFFSET = 0x54
ACTION_OFFSET = 0xC

local recordedFrames = {}
local recordingBaseFrame = nil
local lastGlobalTimer = nil

PATH_ROOT = debug.getinfo(1).source:sub(2):match("(.*\\)")
local targetFileName = PATH_ROOT.."reference_run.json"

local function main()
	local marioState = memory.readdword(MARIO_STATE_ADDRESS)
	local _globalTimer = memory.readdword(GLOBAL_TIMER_ADDRESS)
	if recordingBaseFrame == nil then
		recordingBaseFrame = _globalTimer
	end
	if lastGlobalTimer == nil or lastGlobalTimer < _globalTimer then
		lastGlobalTimer = _globalTimer
		local buttons = memory.readword(memory.readdword(PLAYER1CONTROLLER_ADDRESS) + 0x10)
		table.insert(recordedFrames,
		{
			positionX = memory.readfloat(marioState + POSITION_OFFSET),
			positionY = memory.readfloat(marioState + POSITION_OFFSET + 4),
			positionZ = memory.readfloat(marioState + POSITION_OFFSET + 8),
			faceAngle = memory.readword(marioState + FACEANGLE_OFFSET),
			hSpeed = memory.readfloat(marioState + HSPEED_OFFSET),
			action = memory.readdword(marioState + ACTION_OFFSET),
			buttonDown = buttons
		})
	end
end

local function save() 
	if recordingBaseFrame == nil then
		return
	end
	local file = io.open(targetFileName, "w")
	if (file == nil) then
		print("failed to write shit")
		return
	end
	file:write("[\n")
	local count = 0
	
	for key, value in pairs(recordedFrames) do
		if count > 0 then
			file:write(",\n")
		end
		count = count + 1
		file:write(string.format("{\"x\": %f, \"y\": %f, \"z\": %f, \"angle\": %d, \"hspeed\": %f, \"action\": %d, \"buttonDown\": %d}",
					value.positionX, value.positionY, value.positionZ, value.faceAngle, value.hSpeed, value.action, value.buttonDown))
	end

	file:write("\n]")
	file:close()
	print("Successfully wrote " .. tostring(count) .. " frames to \"" .. targetFileName .. "\".")
end

emu.atinput(main)
emu.atstop(save)