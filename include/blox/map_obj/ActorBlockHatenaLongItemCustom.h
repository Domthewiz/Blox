#pragma once

#include <actor/ActorUniqueID.h>
#include <graphics/AnimModel.h>
#include <map_obj/ActorBlockBase.h>
#include <actor/Profile.h>

namespace blox
{
    class ActorBlockHatenaLongItemCustom : public ActorBlockBase
    {
        SEAD_RTTI_OVERRIDE(ActorBlockHatenaLongItemCustom, ActorBlockBase);
        
    public:
        enum BlockType
        {
            cBlockType_Question = 0,
            cBlockType_Brick,
            cBlockType_Hidden,
        };
        
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        ActorBlockHatenaLongItemCustom(const ActorCreateParam& param);
        ~ActorBlockHatenaLongItemCustom() override = default;

        Result create() override;
        bool execute() override;
        bool draw() override;

        void calcMdl();
        void spawnSideCoins();

        void spawnItemUp() override;
        void spawnItemDown() override;

        void preSpawnItem() override;

        void spawnPowerUp(bool down);
        bool scanString();

        void setupMovement(const sead::Vector3f& position, u32 movement_mask, ParentMovementType movement_type, u32 movement_id);
        void setMovementParamaters(ParentMovementType movement_type);

    protected:
        BlockType mBlockType;
        u8 mStringScanAttempt;
        u16 mSpawnProfileID;
        bool mPrevFrameEvent;
        bool mSpawned;
        ActorUniqueID mNybbleBank;
        u8 mCounter;
        bool mWasHitFromBelow;
        bool mUseHitModel;
        f32 mZPosOffset;
        f32 mZInitialPosOffset;
        AnimModel* mModel;
    };
}