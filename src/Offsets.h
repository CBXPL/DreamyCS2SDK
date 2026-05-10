#pragma once

#include <cstddef>
#include <cstdint>

enum BonesIndexes : int {
    ROOT = 0,
    PELVIS = 1,
    SPINE0 = 2,
    SPINE1 = 3,
    SPINE2 = 4,
    NECK = 6,
    HEAD = 7,
    CLAVICLE_L = 8,
    SHOULDER_L = 9,
    ELBOW_L = 10,
    HAND_L = 11,
    CLAVICLE_R = 12,
    SHOULDER_R = 13,
    ELBOW_R = 14,
    HAND_R = 15,
    HIP_L = 17,
    KNEE_L = 18,
    FOOT_HEEL_L = 19,
    HIP_R = 20,
    KNEE_R = 21,
    FOOT_HEEL_R = 22,
    CHEST = 23,
    EYE_L = 25,
    EYE_R = 26,
};

namespace dreamy_offsets {
    namespace offsets {
        namespace client_dll {
            inline std::ptrdiff_t dwEntityList = 0; // Resolved via sigscan
            inline std::ptrdiff_t dwViewMatrix = 0; // Resolved via sigscan
            inline std::ptrdiff_t dwLocalPlayerController = 0; // Resolved via sigscan

            constexpr std::ptrdiff_t dwGameEntitySystem = 0x24D0DC0;
            constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x2090;
            constexpr std::ptrdiff_t dwGlowManager = 0x2327D40;
            constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x2056700;
            constexpr std::ptrdiff_t dwPlantedC4 = 0x2338A68;
            constexpr std::ptrdiff_t dwViewAngles = 0x2340288;
            constexpr std::ptrdiff_t dwViewRender = 0x232FCD0;
            constexpr std::ptrdiff_t dwWeaponC4 = 0x22A8E38;
        }
        namespace matchmaking_dll {
            constexpr std::ptrdiff_t dwGameTypes = 0x1B0F80;
        }
        namespace soundsystem_dll {
            constexpr std::ptrdiff_t dwSoundSystem = 0x512360;
            constexpr std::ptrdiff_t dwSoundSystem_engineViewData = 0x7C;

            namespace client_offsets {
                constexpr std::ptrdiff_t m_hPlayerPawn = 0x90C;
                constexpr std::ptrdiff_t m_vOldOrigin = 0x1390;
                constexpr std::ptrdiff_t m_iHealth = 0x34C;
                constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;

                // Skeleton
                constexpr std::ptrdiff_t m_pGameSceneNode = 0x330; // CGameSceneNode*
                constexpr std::ptrdiff_t m_modelState = 0x150; // CModelState
                constexpr std::ptrdiff_t m_pBoneArray = 0x80; // CModelState (You got bone indexes on the top)
            }
        }
    }

    void Initialize();

}
