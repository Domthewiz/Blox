#pragma once

#include "actor/Profile.h"
#include "collision/ActorBoxBgCollision.h"
#include "graphics/AnimModel.h"
#include "state/FStateID.h"
#include <actor/ActorState.h>

namespace blox {

    class ElasticBlockEvent : public ActorMultiState {
        SEAD_RTTI_OVERRIDE(ElasticBlockEvent, ActorMultiState);
    public:
        enum ElasticType : u8 {
            cElasticType_Horizontal = 0,
            cElasticType_Vertical = 1,
        };

        enum VerticalElasticExpandDir : u8 {
            cElasticExpandDir_Up = 0,
            cElasticExpandDir_Down = 1,
        };

        enum HorizontalElasticExpandDir : u8 {
            cElasticExpandDir_Right = 0,
            cElasticExpandDir_Left = 1,
        };

    public:
        static Profile* sProfile;
        static const ActorCreateInfo cCreateInfo;

    public:
        ElasticBlockEvent(const ActorCreateParam& param);
        ~ElasticBlockEvent() override = default;
        // Coursetask-getexeframe no of frames on a level 
    public:
        Result create() override;
        bool execute() override;
        bool draw() override;

        void initModels();
        void initCollider();
        void changeStateOnSwitchStatus();

        void setOfs_();
        void calcMdl_();
        
        DECLARE_STATE_ID(ElasticBlockEvent, Retracted);
        DECLARE_STATE_ID(ElasticBlockEvent, Expanding);
        DECLARE_STATE_ID(ElasticBlockEvent, Expanded);
        DECLARE_STATE_ID(ElasticBlockEvent, Retracting);
        
        virtual void getSwitchState();
        
    protected:
        AnimModel*           mModel[16];
        ActorBoxBgCollision  mCollider;
        ElasticType          mElasticType;
        union {
            HorizontalElasticExpandDir horizontal;
            VerticalElasticExpandDir vertical;
        }                    mElasticExpandDir;
        u8                   mInitialSize;
        u8                   mFinalSize;
        u8                   mMaxSize;
        bool                 mEventActive;
        bool                 mPreviousEventActive;
        f32                  mChaseRate;
    };
}