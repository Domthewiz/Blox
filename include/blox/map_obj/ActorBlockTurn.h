#pragma once

#include <graphics/AnimModel.h>
#include <actor/Profile.h>
#include <map_obj/ActorBlockBase.h>

namespace blox {

    class ActorBlockTurn : public ActorBlockBase {
        SEAD_RTTI_OVERRIDE(ActorBlockTurn, ActorBlockBase);
    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;
    
    public:
        ActorBlockTurn(const ActorCreateParam& param);
        ~ActorBlockTurn() override = default;
        
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;
        void calcMdl_();

        bool restoreState() override;
        void destroy() override;
        void destroy2() override;

        void preSpawnItem() override;
        void spawnItemUp() override;
        void spawnItemDown() override;

        bool playerOverlaps();
        
        static void collisionCallback(ActorCollisionCheck* cc_self, ActorCollisionCheck* cc_other);
    
        DECLARE_STATE_ID(ActorBlockTurn, Flipping);

    protected:
        AnimModel* mCModel;
        AnimModel* mLModel;
        AnimModel* mRModel;
        AnimModel* mEModel;
        void initializeFootSensor_();
        void setBoxBgCollisionOfs_();
        s32 mFlipsRemaining;
        bool mFlipsInstantly;
        f32 mColliderExtra;
    };
}
