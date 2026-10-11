#include "system/MainGame.h"
#include <blox/actor/SwitchFlagW6Link.h>
#include <blox/Blox.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>

namespace blox {
    SEAD_RTTI_OVERRIDE_IMPL(SwitchFlagW6Link, Actor);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo SwitchFlagW6Link::cCreateInfo = {
        .offset_x = 0, .offset_y = 0,
        .spawn_range = {
            .offset_x = 0, .offset_y = 0,
            .half_size_x = 1000000, .half_size_y = 1000000
        },
        .cull_range = { 
            .up = 0, .down = 0, .left = 0, .right = 0
        },
        .flag = ACI::cFlag_IgnoreSpawnRange
    };

    Profile* SwitchFlagW6Link::sProfile = blox::getRegistrar()->newProfile<SwitchFlagW6Link>("switch_flag_w6_link")
        .createInfo(cCreateInfo)
        .build();

    SwitchFlagW6Link::SwitchFlagW6Link(const ActorCreateParam& param)
        : Actor(param)
    { }

    ActorBase::Result SwitchFlagW6Link::create() {
        SwitchFlagMgr::instance()->set(mSwitchFlag0 & 0x3f - 1, 0, static_cast<bool>(MainGame::instance()->getCSSwitchState()));
        mSet = SwitchFlagMgr::instance()->isActivated(mSwitchFlag0 & 0x3f - 1);
        return cResult_Success;
    }

    bool SwitchFlagW6Link::execute() {
        if (mSet != SwitchFlagMgr::instance()->isActivated(mSwitchFlag0 & 0x3f - 1)) {
            MainGame::instance()->setCSSwitchState(static_cast<CSSwitchState>(SwitchFlagMgr::instance()->isActivated(mSwitchFlag0 & 0x3f - 1)));
        }
        mSet = SwitchFlagMgr::instance()->isActivated(mSwitchFlag0 & 0x3f - 1);

        return true;
    }

}
