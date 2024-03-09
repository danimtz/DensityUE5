#pragma once


/**
 * when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list
 **/


// Trace used by weapons, will hit physics assets instead of capsules
#define Density_TraceChannel_Weapon					ECC_GameTraceChannel1


// Trace used by by weapons, will hit pawn capsules instead of physics assets
#define Density_TraceChannel_WeaponCapsule						ECC_GameTraceChannel2
