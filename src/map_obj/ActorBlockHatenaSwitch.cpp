#include <red/util/SpriteUtil.h>
#include <blox/Blox.h>
#include <blox/map_obj/ActorBlockHatenaSwitch.h>
#include <game/FlagCtrl.h>
#include <game_info/CourseInfo.h>

namespace blox {

    // TODO!: Fix the crash when trying to hit the block after a screen transition
    SEAD_RTTI_OVERRIDE_IMPL(ActorBlockHatenaSwitch, ActorBlockSwitch)

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ActorBlockHatenaSwitch::cCreateInfo = {
        .offset_x = 0, .offset_y = 0,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 1000000, .half_size_y = 1000000
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_None
    };

    Profile* ActorBlockHatenaSwitch::sProfile = blox::getRegistrar()->newProfile<ActorBlockHatenaSwitch>("actor_block_hatena_switch")
        .resources<"switch">(ProfileInfo::cResType_Course)
        .createInfo(cCreateInfo)
        .drawPriority(232)
        .build();

    ActorBlockHatenaSwitch::ActorBlockHatenaSwitch(const ActorCreateParam& param)
        : ActorBlockSwitch(param)
    { }

    ActorBase::Result ActorBlockHatenaSwitch::create() {
        if (!ActorBlockSwitch::create()) {
            return cResult_Failed;
        }

        
        if (FlagCtrl::instance()->getFlagData(CourseInfo::instance()->getFileNo(), mPos.x, mPos.y) || FlagCtrl::instance()->getFlagData(CourseInfo::instance()->getFileNo(), mPos.x, mPos.y + 16.0f)) {
            mUnitID = cUnitID_BlockUsed;
            changeState(StateID_HitWait);
            mType = cType_Hit;
        };

        if (mUnitID == cUnitID_BrickBlock) {
            mUnitID = cUnitID_QBlock;
        }
        
        if (mParamEx.course.link_id >> 0x00 & 0x1) {
            mSwitchType = cSwitchType_SwitchP;
        }

        return cResult_Success;
    }

    void ActorBlockHatenaSwitch::spawnItemUp() {
        FlagCtrl::instance()->setFlagData(CourseInfo::instance()->getFileNo(), mPos.x, mPos.y, 1);
        return ActorBlockSwitch::spawnItemUp();
    }

    void ActorBlockHatenaSwitch::spawnItemDown() {
        FlagCtrl::instance()->setFlagData(CourseInfo::instance()->getFileNo(), mPos.x, mPos.y, 1);
        return ActorBlockSwitch::spawnItemDown();
    }
}