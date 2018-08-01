require('sh_gvr.lua')

function VR_PACKET_SEND()
    net.Start("vr_packet")
        net.WriteBool(true)
        local deviceCount = 0
        local infoTable = {}
        for i=0, gvr.CountDevices() do
            local class = gvr.GetDeviceClass(i)
            if(class == -1)then
                break;
            end
            if(class == 0)then
                break
            end
            local pose = gvr.GetDevicePose(i)
            if(pose)then
                deviceCount = deviceCount + 1
                local role = gvr.GetDeviceRole(i)
                infoTable[i] = {class = class, role = role, pos = pose.pos, ang = pose.ang}
            end
        end
        net.WriteInt(deviceCount, 8)
        for i=0, deviceCount - 1 do
            net.WriteInt(infoTable[i].class, 8)
            net.WriteInt(infoTable[i].role, 8)
            net.WriteVector(infoTable[i].pos)
            net.WriteAngle(infoTable[i].ang)
        end
    net.SendToServer()
end

function NO_VR_PACKET_SEND()
    net.Start("vr_packet")
        net.WriteBool(false)
    net.SendToServer()
end

local HASVR = pcall(require, "gmvr");


if HASVR && gvr.IsHmdPresent() then

    gvr.InitVR()
    for i=0, gvr.CountDevices() do
        local id = i
        local class = gvr.GetDeviceClass(i)
        local role = gvr.GetDeviceRole(i)

        if(class == 0)then
            break
        end
        if(class == -1)then
            error("SteamVR not initialized")
        end
        print(id, classes[class], roles[role])
        local pose = gvr.GetDevicePose(id)
        if(pose)then
            PrintTable(pose)
        end
    end

    function PointToPos(point)
        return Vector(point.x, -point.z, point.y)
    end

    local negatives = {{1, 1, 1}}
    //{1, 1, -1}, {1, -1, 1}, {1, -1, -1}, {-1, 1, 1}, {-1, 1, -1}, {-1, -1, 1}, {-1, -1, -1}}
    local angles = {{'x', 'y', 'z'}}
    //{'z', 'x', 'y'}, {'y', 'z', 'x'}, {'z', 'y', 'x'}, {'x', 'z', 'y'}, {'y', 'x', 'z'}}

    // matrix[0] = LEFT
    // matrix[1] = UP?
    // matrix[2] = FORWARD?

    local PDIST = 4
    local eyes = {[0] = -PDIST, PDIST}
    local HUDSCALE = 0.5

    hook.Add("ShouldDrawLocalPlayer", "Yeah", function()
        return true
    end)

    local DODRAW = false
    hook.Add("PostDrawHUD", "ReWriteHUDS", function()
        if(DODRAW)then
            return
        end
        cam.Start2D()
            DODRAW = true
            local id = 0
            local class = gvr.GetDeviceClass(id)
            if(class == 0)then
                return
            end
            if(class == -1)then
                error("SteamVR Failed.")
            end
            local HMD = gvr.GetDevicePose(id)
            if(HMD)then
                for k,v in pairs(eyes) do
                    render.RenderView({
                        aspectratio = 1,
                        origin = LocalPlayer():GetPos() + HMD.pos * 40 + HMD.ang:Right() * v,
                        angles = HMD.ang,
                        x = (ScrW()/2) * k,
                        y = 0,
                        w = ScrW()/2,
                        h = ScrH(),
                        drawhud = false,
                        drawviewmodel = false,
                        fov = 90
                    })
                    surface.SetDrawColor(255, 255, 255, 255)
                    local HUDWIDTH = ScrW()/2 * HUDSCALE
                    local HUDHEIGHT = ScrH() * HUDSCALE
                    local HUDLEFT = ScrW()/4 - HUDWIDTH/2
                    local HUDTOP = ScrH()/2 - HUDHEIGHT/2
                    render.RenderHUD((ScrW()/2) * k + HUDLEFT - v, HUDTOP, HUDWIDTH, HUDHEIGHT)
                end
        end
            DODRAW = false
        cam.End2D()
    end)

    hook.Add("CalcView", "VRView", function(ply, origin, angles, fov, znear, zfar)
        local id = 0
        local class = gvr.GetDeviceClass(id)
        if(class == 0)then
            return
        end
        if(class == -1)then
            error("SteamVR Failed.")
        end
        local HMD = gvr.GetDevicePose(id)
        if(!HMD)then
            return
        end
        local pos = LocalPlayer():GetPos() + HMD.pos * 100
        local m = HMD.points
        /*local matrix = Matrix({
            {m.x.z, m.y.z, m.z.z, m.w.z},
            {m.x.x, m.y.x, m.z.x, m.w.x},
            {m.x.y, m.y.y, m.z.y, m.w.y},
            {0    , 0    , 0    , 1    }
        })*/
        local matrix = Matrix()
        matrix:SetRight(PointToPos(m.x))
        matrix:SetForward(-PointToPos(m.z))
        matrix:SetUp(PointToPos(m.y))
        
        local ang = matrix:GetAngles()
        local v = {}
        v.origin = pos
        if(LocalPlayer():KeyDown(IN_SPEED))then
            v.angles = HMD.ang
        else
            v.angles = ang
        end
        v.fov = 110
        return v
    end)

    hook.Add("Tick", "VR_PACKET", VR_PACKET_SEND)
else
    hook.Remove("Tick", "VR_PACKET", NO_VR_PACKET_SEND)
end