#include "collision/ActorBgCollisionMgr.h"
#include "map/SwitchFlagMgr.h"
#include "system/MainGame.h"
#include <blox/Blox.h>
#include <blox/actor/ElasticBlockEvent.h>

namespace blox {

    static constexpr sead::Vector2f cColliderInitialTopLeft(-8.0f, 8.0f);
    static constexpr sead::Vector2f cColliderInitialBottomRight(8.0f, -8.0f);
    static constexpr f32 cChaseBase = 0.03f;
    
    CREATE_STATE_ID(ElasticBlockEvent, Retracted);
    CREATE_STATE_ID(ElasticBlockEvent, Expanding);
    CREATE_STATE_ID(ElasticBlockEvent, Expanded);
    CREATE_STATE_ID(ElasticBlockEvent, Retracting);
    
    SEAD_RTTI_OVERRIDE_IMPL(ElasticBlockEvent, ActorMultiState);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ElasticBlockEvent::cCreateInfo = {
        .offset_x = 0, .offset_y = 0,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 128, .half_size_y = 128
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_None
    };

    Profile* ElasticBlockEvent::sProfile = blox::getRegistrar()->newProfile<ElasticBlockEvent>("elastic_block_CSW6")
        .resources<"block_CSW6s">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .createInfo(cCreateInfo)
        .build();

    ElasticBlockEvent::ElasticBlockEvent(const ActorCreateParam& param)
        : ActorMultiState(param)
        , mModel(nullptr)
        , mElasticType(cElasticType_Horizontal)
        , mElasticExpandDir(cElasticExpandDir_Right)
        , mInitialSize(0)
        , mFinalSize(0)
        , mMaxSize(0)
        , mChaseRate(1.0f)
    { }

    ActorBase::Result ElasticBlockEvent::create() {
        mElasticType = static_cast<ElasticType>(mParam0 >> 0x1D & 0x1);
        mElasticExpandDir.vertical = static_cast<VerticalElasticExpandDir>(mParam0 >> 0x1E & 0x1);
        
        mInitialSize = mParam0 >> 0x18 & 0xF;
        mInitialSize++;

        mFinalSize = mParam0 >> 0x14 & 0xF;
        mFinalSize++;

        mMaxSize = mFinalSize;
        if (mInitialSize > mFinalSize) {
            mMaxSize = mInitialSize;
        }

        if (mMaxSize == 16 || mMaxSize == 1) {
            tk::fatal("Maximum size for World 6 Stretch Block cannot be more than 15 blocks or less than 2");
        }

        mChaseRate = cChaseBase * sead::Mathf::abs(mInitialSize - mFinalSize);

        getSwitchState();

        if (mEventActive) {
            changeState(StateID_Expanded);
        } else {
            changeState(StateID_Retracted);
        }
        // // executeState();

        initModels();
        initCollider();
        setOfs_();

        // changeStateOnSwitchStatus();
        calcMdl_();
        
        return cResult_Success;
    }

    bool ElasticBlockEvent::execute() {
        mPreviousEventActive = mEventActive;
        getSwitchState();
        changeStateOnSwitchStatus();

        executeState();

        setOfs_();
        mCollider.execute();

        calcMdl_();
        
        return true;
    }

    bool ElasticBlockEvent::draw() {
        for (u8 i = mMaxSize; i > 0; i--) {
            if (mModel[i] != nullptr) {
                mModel[i]->draw();
            }
        }
        return true;
    }

    void ElasticBlockEvent::calcMdl_() {
        for (u8 i = mMaxSize; i > 0; i--) {
            if (mModel[i] != nullptr) {
                sead::Vector3f posOffset(sead::Vector3f::zero);
                posOffset.z = 16.0f * static_cast<f32>(i);

                switch (mElasticType) {
                    case cElasticType_Horizontal: {
                        switch (mElasticExpandDir.horizontal) {

                            case cElasticExpandDir_Right: {
                                posOffset.x = (16.0f * static_cast<f32>(i - 1) * (mScale.x - 1.0f))/static_cast<f32>(mMaxSize - 1.0f);
                                break;
                            }
                            
                            case cElasticExpandDir_Left: {
                                posOffset.x = -(16.0f * static_cast<f32>(i - 1) * (mScale.x - 1.0f))/static_cast<f32>(mMaxSize - 1.0f);
                                break;
                            }
                        }
                        break;
                    }
                    case cElasticType_Vertical: {
                        switch (mElasticExpandDir.vertical) {

                            case cElasticExpandDir_Up: {
                                posOffset.y = (16.0f * static_cast<f32>(i - 1) * (mScale.y - 1.0f))/static_cast<f32>(mMaxSize - 1.0f);
                                break;
                            }
                            
                            case cElasticExpandDir_Down: {
                                posOffset.y = -(16.0f * static_cast<f32>(i - 1) * (mScale.y - 1.0f))/static_cast<f32>(mMaxSize - 1.0f);
                                break;
                            }
                        }
                        break;
                    }
                }

                mModel[i]->update(mPos + posOffset, mAngle, sead::Vector3f::ones);
            }
        }
    }

    void ElasticBlockEvent::setOfs_() {
        sead::Vector2f topLeft = cColliderInitialTopLeft;
        sead::Vector2f bottomRight = cColliderInitialBottomRight;

        // do the actual scaling
        switch (mElasticType) {
            case cElasticType_Horizontal: {
                switch (mElasticExpandDir.horizontal) {

                    case cElasticExpandDir_Right: {
                        bottomRight.x += 16.0f * (mScale.x - 1.0f);
                        break;
                    }
                    
                    case cElasticExpandDir_Left: {
                        topLeft.x -= 16.0f * (mScale.x - 1.0f);
                        break;
                    }
                }
                break;
            }
            case cElasticType_Vertical: {
                switch (mElasticExpandDir.vertical) {

                    case cElasticExpandDir_Up: {
                        topLeft.y += 16.0f * (mScale.y - 1.0f);
                        break;
                    }
                    
                    case cElasticExpandDir_Down: {
                        bottomRight.y -= 16.0f * (mScale.y - 1.0f);
                        break;
                    }
                }
                break;
            }
        }
        
        mCollider.setOfs(topLeft, bottomRight);
        // LoopRideLineBgCollisionUtil::setOfs(&mCollider, topLeft, bottomRight);
    }

    void ElasticBlockEvent::initModels() {
        for (u8 i = mMaxSize; i > 0; i--) {
            mModel[i] = AnimModel::create("block_CSW6s", "block_slide", 2, 1);
            mModel[i]->playTexAnim("block_slide");
            mModel[i]->getTexAnim(0)->getFrameCtrl().setRate(0.0f);
            // make it blue
            mModel[i]->getTexAnim(0)->getFrameCtrl().setFrame(static_cast<f32>(mParam0 >> 0x1C & 0x1));
        }
    }

    void ElasticBlockEvent::initCollider() {
        mCollider.set(this, {
            .pos_offset         = { 0.0f, 0.0f },
            .rot_pivot_offset   = { 0.0f, 0.0f },
            .left_top_offset    = cColliderInitialTopLeft,
            .right_under_offset = cColliderInitialBottomRight,
            .angle              = mAngle.z()
        });
        mCollider.setSlipAttr(BgUnitCode::cSlipAttr_NoSuberu);
        ActorBgCollisionMgr::instance()->entry(mCollider);
    }

    //! TODO: see if this shit checks out
    void ElasticBlockEvent::getSwitchState() {
        // mEventActive = SwitchFlagMgr::instance()->isActivated(mSwitchFlag0 & 0x3f - 1);
        mEventActive = MainGame::instance()->getCSSwitchState();
    }

    void ElasticBlockEvent::changeStateOnSwitchStatus() {
        if (mEventActive == mPreviousEventActive) {
            return;
        }

        if (isState(StateID_Expanded) || isState(StateID_Expanding)) {
            changeState(StateID_Retracting);
        } else {
            changeState(StateID_Expanding);
        }
    }

    /** STATE: Retracted */
    
    void ElasticBlockEvent::initializeState_Retracted() {
        mScale.x = mInitialSize;
        mScale.y = mInitialSize;
    }

    void ElasticBlockEvent::executeState_Retracted() {
    }

    void ElasticBlockEvent::finalizeState_Retracted() {
    }

    /** STATE: Expanding */
    
    void ElasticBlockEvent::initializeState_Expanding() {
    }

    void ElasticBlockEvent::executeState_Expanding() {
        sead::Mathf::chase(&mScale.x, mFinalSize, mChaseRate);
        sead::Mathf::chase(&mScale.y, mFinalSize, mChaseRate);
    }

    void ElasticBlockEvent::finalizeState_Expanding() {
    }

    /** STATE: Expanded */
    
    void ElasticBlockEvent::initializeState_Expanded() {
        mScale.x = mFinalSize;
        mScale.y = mFinalSize;
    }

    void ElasticBlockEvent::executeState_Expanded() {
    }

    void ElasticBlockEvent::finalizeState_Expanded() {
    }

    /** STATE: Retracting */
    
    void ElasticBlockEvent::initializeState_Retracting() {
    }

    void ElasticBlockEvent::executeState_Retracting() {
        sead::Mathf::chase(&mScale.x, mInitialSize, mChaseRate);
        sead::Mathf::chase(&mScale.y, mInitialSize, mChaseRate);
    }

    void ElasticBlockEvent::finalizeState_Retracting() {
    }

}
