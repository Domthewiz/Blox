#pragma once

#include <graphics/AnimModel.h>
#include <actor/Profile.h>
#include <prim/seadRuntimeTypeInfo.h>
#include <map_obj/ActorBlockBase.h>

namespace blox {

    class ActorBlockHatenaDuplicate : public ActorBlockBase {
        SEAD_RTTI_OVERRIDE(ActorBlockHatenaDuplicate, ActorBlockBase)
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
        
    public:
        ActorBlockHatenaDuplicate(const ActorCreateParam& param);
        ~ActorBlockHatenaDuplicate() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;
        
        void calcMdl();
        
        void spawnItemUp() override;
        void spawnItemDown() override;
        void spawnItem();

        u32 vf32C() override;
        
        void onBumpDiff() override;
        void onUpMoveStart() override;
        
        void spawnCoinShower() override;
        void preSpawnItem() override;
        
        void setupMovement(const sead::Vector3f& position, u32 movement_mask, ParentMovementType movement_type, u32 movement_id);
        void setMovementParamaters(ParentMovementType movement_type);
        
    protected:
        u8 mCounter;
        bool mWasHitFromBelow;
        bool mUseHitModel;
        f32 mZPosOffset;
        f32 mZInitialPosOffset;
        AnimModel* mModel;
        ParentMovementMgr mMovementMgr;
    };
}