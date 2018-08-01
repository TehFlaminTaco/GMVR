#include "GarrysMod/Lua/Interface.h"
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "openvr.h"
int version = 1; //every release this will be incremented, in lua, the user will be warned to update the dll if they have a lua version ahead of the module.

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

using namespace GarrysMod::Lua;
using namespace vr;

IVRSystem *vr_pointer;
TrackedDevicePose_t tracked_poses[k_unMaxTrackedDeviceCount];

typedef struct double3{
	double x;
	double y;
	double z;
} double3;

double3 GetEuler(HmdMatrix34_t matrix){
	double y = 0;
	double x = 0;
	double z = z;
	if(matrix.m[1][0] > 0.999){
		y = atan2(-matrix.m[0][2], -matrix.m[2][2]);
		z = M_PI / 2;
		x = 0;
	}else if (matrix.m[1][0] < -0.999){
		y = atan2(-matrix.m[0][2], -matrix.m[2][2]);
		z = -M_PI / 2;
		x = 0;
	}else{
		y = atan2(matrix.m[2][0], matrix.m[0][0]);
		x = atan2(matrix.m[1][2], matrix.m[1][1]);
		z = asin(-matrix.m[1][0]);
	}
	double3 ang = {180/M_PI * x, 180/M_PI * -y, 180/M_PI * z};
	return ang;
}

LUA_FUNCTION( GetVersion )
{
	LUA->PushNumber(version);
	return 1;
}

LUA_FUNCTION(IsHmdPresent)
{
	LUA->PushBool(VR_IsHmdPresent());
	return 1;
}

LUA_FUNCTION(InitVR)
{
	if(vr_pointer){
		LUA -> PushBool(true);
		return 1;
	}
	EVRInitError eError = VRInitError_None;
	vr_pointer = VR_Init(&eError, VRApplication_Background);
	if (eError != VRInitError_None)
	{
		
		LUA->PushBool(false);
		return 1;
	}
	LUA->PushBool(true);
	return 1;
}

LUA_FUNCTION(CountDevices)
{
	LUA->PushNumber(vr::k_unMaxTrackedDeviceCount);
	return 1;
}

int ResolveDeviceType( int deviceId ){

	if (!vr_pointer) {
		return -1;
	}

	ETrackedDeviceClass deviceClass = VRSystem()->GetTrackedDeviceClass(deviceId);

	return static_cast<int>(deviceClass);
}

int ResolveDeviceRole(int deviceId) {

	if (!vr_pointer) {
		return -1;
	}

	int deviceRole = VRSystem()->GetInt32TrackedDeviceProperty(deviceId, ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);
	
	return static_cast<int>(deviceRole);
}

LUA_FUNCTION(GetDevicePose)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
    if(deviceId < 0 || deviceId >= k_unMaxTrackedDeviceCount){
        LUA -> PushBool(false);
        return 1;
    }
	if(!vr_pointer){
		LUA -> PushBool(false);
		return 1;
	}
    VRSystem() -> GetDeviceToAbsoluteTrackingPose(TrackingUniverseOrigin::TrackingUniverseStanding, 0, tracked_poses, k_unMaxTrackedDeviceCount);
    TrackedDevicePose_t pose = tracked_poses[deviceId];
    if(!pose.bPoseIsValid || !pose.bDeviceIsConnected){
        LUA -> PushBool(false);
        return 1;
    }
    LUA -> CreateTable();
        LUA -> CreateTable();
            LUA -> PushNumber(pose.vVelocity.v[0]); LUA -> SetField(-2, "x");
            LUA -> PushNumber(pose.vVelocity.v[2]); LUA -> SetField(-2, "y");
            LUA -> PushNumber(pose.vVelocity.v[1]); LUA -> SetField(-2, "z");
        LUA -> SetField(-2, "velocity");
        LUA -> CreateTable();
            LUA -> CreateTable();
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[0][0]); LUA -> SetField(-2, "x");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[1][0]); LUA -> SetField(-2, "y");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[2][0]); LUA -> SetField(-2, "z");
            LUA -> SetField(-2, "x");
            LUA -> CreateTable();
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[0][1]); LUA -> SetField(-2, "x");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[1][1]); LUA -> SetField(-2, "y");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[2][1]); LUA -> SetField(-2, "z");
            LUA -> SetField(-2, "y");
            LUA -> CreateTable();
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[0][2]); LUA -> SetField(-2, "x");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[1][2]); LUA -> SetField(-2, "y");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[2][2]); LUA -> SetField(-2, "z");
            LUA -> SetField(-2, "z");
            LUA -> CreateTable();
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[0][3]); LUA -> SetField(-2, "x");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[1][3]); LUA -> SetField(-2, "y");
                LUA -> PushNumber(pose.mDeviceToAbsoluteTracking.m[2][3]); LUA -> SetField(-2, "z");
            LUA -> SetField(-2, "w");
        LUA -> SetField(-2, "points");
        Vector vec;
        QAngle ang;
        vec.x = -pose.mDeviceToAbsoluteTracking.m[2][3];
        vec.y = -pose.mDeviceToAbsoluteTracking.m[0][3];
        vec.z = pose.mDeviceToAbsoluteTracking.m[1][3];
        LUA -> PushVector(vec); LUA -> SetField(-2, "pos");
		double3 a2 = GetEuler(pose.mDeviceToAbsoluteTracking);
        //toEulerAngle(GetRotation(pose.mDeviceToAbsoluteTracking), roll, pitch, yaw);
        ang.x = a2.x;
        ang.y = a2.y;
        ang.z = a2.z;
        LUA -> PushAngle(ang); LUA -> SetField(-2, "ang");
	return 1;
}

LUA_FUNCTION(GetDeviceClass)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
	int type = ResolveDeviceType(deviceId);
	LUA->PushNumber(type);
	return 1;
}

LUA_FUNCTION(GetDeviceRole)
{
	(LUA->CheckType(1, Type::NUMBER));
	int deviceId = static_cast<int>(LUA->GetNumber(1));
	int type = ResolveDeviceRole(deviceId);
	LUA->PushNumber(type);
	return 1;
}


GMOD_MODULE_OPEN()
{
	LUA->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		LUA->CreateTable();
			LUA->PushCFunction(GetVersion); LUA->SetField(-2, "GetVersion");
			LUA->PushCFunction(IsHmdPresent); LUA->SetField(-2, "IsHmdPresent");
			LUA->PushCFunction(InitVR); LUA->SetField(-2, "InitVR");
			LUA->PushCFunction(CountDevices); LUA->SetField(-2, "CountDevices");
			LUA->PushCFunction(GetDeviceClass); LUA->SetField(-2, "GetDeviceClass");
			LUA->PushCFunction(GetDeviceRole); LUA->SetField(-2, "GetDeviceRole");
            LUA->PushCFunction(GetDevicePose); LUA->SetField(-2, "GetDevicePose");
		LUA->SetField(-2, "gvr");
	LUA->Pop();
	return 0;
}

 
GMOD_MODULE_CLOSE()
{
    return 0;
}

void Shutdown()
{
	if (vr_pointer != NULL)
	{
		VR_Shutdown();
		vr_pointer = NULL;
	}
}