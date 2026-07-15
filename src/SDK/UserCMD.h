#pragma once
#define NOMINMAX
#undef max
#include <Windows.h>
#include <stdint.h>
#include <string>
// afaik u can use those without usercmd hooks directly in pInput
#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)

struct PBMessage
{
    void* vtable;
    void* metadata;
};
template <typename TT>
struct duplicate_offset
{
    struct Rep_t
    {
        int allocated_size;
        TT* elements[(std::numeric_limits<int>::max() - 2 * sizeof(int)) / sizeof(void*)];
    };

    void* arena;
    int current_size;
    int total_size;
    Rep_t* rep;
};

#pragma pack(push, 1)
struct in_button_state_t {
public:
    void* m_vtable; 
    uint64_t m_value;
    uint64_t m_value_changed;
    uint64_t m_value_scroll;

    enum e_button_state_t : int8_t {
        in_button_up = 0,
        in_button_down = 1,
        in_button_down_up = 2,
        in_button_up_down = 3,
    };

    void set_button_state(const uint64_t& u_value, e_button_state_t e_button_state) {
        switch (e_button_state) {
        case in_button_up:
            m_value &= ~u_value; m_value_changed &= ~u_value; m_value_scroll &= ~u_value; break;
        case in_button_down:
            m_value |= u_value; m_value_changed &= ~u_value; m_value_scroll &= ~u_value; break;
        case in_button_down_up:
            m_value &= ~u_value; m_value_changed |= u_value; m_value_scroll &= ~u_value; break;
        case in_button_up_down:
            m_value |= u_value; m_value_changed |= u_value; m_value_scroll &= ~u_value; break;
        }
    }
};
#pragma pack(pop)
// UserCMD Subtick struct
struct CSubtickMoveStep : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    uint64_t button;
    bool pressed;
    float when;
    float analog_forward_delta;
    float analog_left_delta;
    float pitch_delta;
    float yaw_delta;
};

// UserCMD Protobuf struct
struct CInButtonStatePB : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    uint64_t buttonstate1;
    uint64_t buttonstate2;
    uint64_t buttonstate3;
};

struct CBaseUserCmdExecutionNotes : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    std::string* ignored_reason;
};


struct CMsgQAngle : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    float x;
    float y;
    float z;
};
// IMPORTANT: After reversing subtick CBaseUserCmdPB is getting ignored/buggy when your yaw is desynced with your camera/keyboard input as subtick got reworked. So i recommend using CSubtickMoveStep as your primary movement function
struct CBaseUserCmdPB : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    duplicate_offset<CSubtickMoveStep> subtick_moves;
    std::string* move_crc;
    CInButtonStatePB* buttons_pb;
    CMsgQAngle* viewangles;
    CBaseUserCmdExecutionNotes* execution_notes;
    int32_t legacy_command_number;
    int32_t client_tick;
    float forwardmove;
    float leftmove;
    float upmove;
    int32_t impulse;
    int32_t weaponselect;
    int32_t random_seed;
    int32_t mousedx;
    int32_t mousedy;
};

struct CUserCmdBasePB : PBMessage
{
    uint32_t has_bits;
    uint32_t cached_size;
    CBaseUserCmdPB* usercmd_base;
};