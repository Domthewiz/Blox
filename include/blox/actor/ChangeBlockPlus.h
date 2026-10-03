#pragma once

#include <actor/ActorState.h>
#include <actor/Profile.h>
#include <graphics/AnimModel.h>

namespace blox {
    
    class ChangeBlockPlus : public ActorMultiState {
        SEAD_RTTI_OVERRIDE(ChangeBlockPlus, ActorMultiState);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        ChangeBlockPlus(const ActorCreateParam& param);
        ~ChangeBlockPlus() override = default;
        
    public:
        Result create() override;
        bool execute() override;
    
        u64 readEvents();

        void update();

        void apply(u32 destroy_or_spawn, u32 no_effect, u32 no_sound);
        
        // custom functions
        void scanLocationData(u8 location_id);

    protected:
        u64 mOldEvents;
        u32 mWidth; // nybble 18
        u32 mHeight; // nybble 20
        u32 mSpawnOrDestroy; // 0d, 1spawn. nybble 10; 0x4 size bool???
        u16 mObjectType; // nybble 9
        u32 mIsPermanent; // nybble 5; 0x4 size bool???
        u32 mFillPattern; // nybble 8
        bool mWasNybbleTenInitiallyNotZero;
        u8 _17e9;
        u8 _17ea;
        u8 _17eb;
        u8 _17ec;
        u8 _17ed;
        u8 _17ee;
        u8 _17ef; // End of vanilla settings, 0x17F0
        bool mSilent;
        bool mNoResidue;
        bool mUsingLocation;
        bool mNoDestroy;
        u16* mTileData;
        sead::Vector2u mTileSize;
    };
}