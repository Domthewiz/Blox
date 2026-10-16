#pragma once

#include "actor/ActorUniqueID.h"
#include <map_obj/ActorBlockBase.h>
#include <actor/Profile.h>

namespace blox
{
    class ActorBlockItemCustom : public ActorBlockBase
    {
        SEAD_RTTI_OVERRIDE(ActorBlockItemCustom, ActorBlockBase);
        
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
        ActorBlockItemCustom(const ActorCreateParam& param);
        ~ActorBlockItemCustom() override = default;

        Result create() override;
        bool execute() override;

        void spawnItemUp() override;
        void spawnItemDown() override;

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
    };
}