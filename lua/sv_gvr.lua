require("sh_gvr")

util.AddNetworkString("vr_packet")
net.Receive("vr_packet", function(len, ply)
	local hasVR = net.ReadBool()
	ply.hasVR = hasVR
	if hasVR then
		local deviceCount = net.ReadInt(8)
		ply.vr_devices = {}
		for i=1, deviceCount do
			local class = net.ReadInt(8)
			local role = net.ReadInt(8)
			local pos = net.ReadVector()
			local ang = net.ReadAngle()
			ply.vr_devices[i-1] = {class = class, role = role, pos = pos, ang = ang}
		end
	end
end)