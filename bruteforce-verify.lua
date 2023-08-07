-- LUA script for use in mupen64
-- This script will take the output of a bruteforcer module run with the --outputmode=sequence or --outputmode=m64_and_sequence, as operated by STROOP

local fastEmulation = false -- if set to true, speedmode will be set to maximum during the verification

local modulePath = nil
local fileName = nil
local inputs = {}
local savestateFile = nil
local advanceFrame = false
local globalTimerAtStart = nil
local globalTimerAtSavestate = nil
local originalInputs = {}

gGlobalTimer = {
	J = function() return memory.readdword(0x8032c694) end,
	U = function() return memory.readdword(0x8032d5d4) end,
	Read = function() return gGlobalTimer.U() end
}

local MAX_ACCEPTED_INPUTS = 1000;

local function getPath(str,sep)
    sep=sep or'/'
    return str:match("(.*"..sep..")"):gsub("\\","/")
end

local function verifyFrame() 
	if (advanceFrame) then
		emu.pause(false)
		advanceFrame = false
	end
	local glob = gGlobalTimer.Read()
	if (globalTimerAtStart ~= nil) then
		local currentInput = glob - globalTimerAtStart + 1
		if currentInput >= 1 then 
			-- This is a bruteforce result
			if currentInput == #inputs - 1 and fastEmulation then
				emu.speedmode("normal") -- on the second to last frame, set the speedmode to normal to make sure the game is displayed properly
			end
			if (currentInput <= #inputs) then
				joypad.set(1, inputs[currentInput])
				if (currentInput == #inputs) then
					emu.pause(false)
					globalTimerAtStart = nil
					print("Waiting for brutforcer results...")
				end
			end
		else
			-- This is before bruteforce results start
			currentInput = glob - globalTimerAtSavestate + 1
			joypad.set(1, originalInputs[currentInput])
		end
	end
end

local function checkUpdates()
	-- do not read files while verifying
	if globalTimerAtStart ~= nil then return end
	
	-- TODO: make this revision based
	local file = io.open(fileName, "r")
	if (file ~= nil) then

		-- read contents of the file
		local _globalTimerAtStart = file:read("n")

		local input = file:read("n")
		local i = 1

		while (input ~= nil) do
			local buttons = (input >> 16) & 0xFFFFF
			local stickX = (input >> 8) & 0xFF
			local stickY = (input >> 0) & 0xFF
			if (stickX > 127) then stickX = stickX - 256 end
			if (stickY > 127) then stickY = stickY - 256 end
			local inputFrame = {
				["X"] = stickX, 
				["Y"] = stickY,
				A = (buttons & 0x8000) > 0,
				B = (buttons & 0x4000) > 0,
				Z = (buttons & 0x2000) > 0,
				start = (buttons & 0x1000) > 0,
				up = (buttons & 0x0800) > 0,
				down = (buttons & 0x0400) > 0,
				left = (buttons & 0x0200) > 0,
				right = (buttons & 0x0100) > 0,
				L = (buttons & 0x20) > 0,
				R = (buttons & 0x10) > 0,
				Cup = (buttons & 0x08) > 0,
				Cdown = (buttons & 0x04) > 0,
				Cleft = (buttons & 0x02) > 0,
				Cright = (buttons & 0x01) > 0
			}
			inputs[i] = inputFrame
			
			--print("stick: " .. stickX .. "; " .. stickY .. "; Buttons: " .. string.format("%x8", buttons) .. "; A: " .. tostring(inputFrame.A))

			input = file:read("n")

			i = i + 1
			if (i > MAX_ACCEPTED_INPUTS) then break end
		end
		file:close()
		os.remove(fileName)
		
		print("verifying...")
		savestate.loadfile(savestateFile)
		emu.pause(true)
		globalTimerAtStart = _globalTimerAtStart
		if fastEmulation then emu.speedmode("maximum") end
	end
end

local function getOriginalInputs()
	print("Getting original inputs...")

	local forwardDone = nil;

	local function grabFrame()
		local inputFrame = gGlobalTimer.Read() - globalTimerAtSavestate + 1
		local currentInput = joypad.get(1)
		originalInputs[inputFrame] = currentInput
		if (inputFrame > MAX_ACCEPTED_INPUTS) then
			forwardDone();
		end
	end

	local function done()
		emu.atloadstate(done, true) -- unregister done
		emu.atinput(grabFrame, true) -- unregister grabFrame
		emu.speedmode("normal")

		savestate.loadfile(savestateFile)
		emu.atinterval(checkUpdates)
		emu.atinput(verifyFrame)
		emu.pause(true)

		print("Waiting for bruteforce results...")
	end

	forwardDone = done;

	emu.atinput(grabFrame)
	emu.atloadstate(done) -- assumes that "loop movie playback" option is enabled and reacts to the automatic loadstate from its occurrence
	emu.speedmode("maximum")
end

local bruteforcerExecutable = iohelper.filediag("bruteforcer path (*.exe)\0*.exe\0\0", 0)

local function init()
	modulePath = getPath(bruteforcerExecutable, '\\')
	fileName = modulePath .. "tmp.m64.part"
	savestateFile = modulePath .. "bf_start.st"
	print("Saving to: " .. savestateFile)
	globalTimerAtSavestate = gGlobalTimer.Read()
	print("Global timer is " .. globalTimerAtSavestate)
	savestate.savefile(savestateFile)
	getOriginalInputs()
	advanceFrame = true
	emu.pause(true)
	emu.atstop(function() emu.speedmode("normal") end)
end

if (bruteforcerExecutable ~= nil) then
	init()
else
	print("No moudle selected")
end