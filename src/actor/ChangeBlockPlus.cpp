#include "actor/Actor.h"
#include "actor/ActorBase.h"
#include "actor/ActorState.h"
#include "audio/GameAudio.h"
#include "effect/EffectCreateUtil.h"
#include "map/Bg.h"
#include "telkin/Print.h"
#include <blox/Blox.h>
#include <collision/ActorBgCollisionMgr.h>
#include <red/util/SpriteUtil.h>
#include <map/SwitchFlagMgr.h>
#include <blox/actor/ChangeBlockPlus.h>
#include <game_info/CourseInfo.h>
#include <map/CourseData.h>

// Remade tile god
namespace blox {
    SEAD_RTTI_OVERRIDE_IMPL(ChangeBlockPlus, ActorMultiState);

    using ACI = ActorCreateInfo;
    const ActorCreateInfo ChangeBlockPlus::cCreateInfo = {
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

    Profile* ChangeBlockPlus::sProfile = blox::getRegistrar()->newProfile<ChangeBlockPlus>("change_block_plus")
        .createInfo(cCreateInfo)
        .build();

    ChangeBlockPlus::ChangeBlockPlus(const ActorCreateParam& param)
        : ActorMultiState(param)
        , mHeight(0)
        , mWidth(0)
        , mOldEvents(0)
        , mFillPattern(0)
        , mIsPermanent(0)
        , mObjectType(0)
        , mSpawnOrDestroy(0)
        , mTileData(nullptr)
        , mTileSize(0, 0)
        , mUsingLocation(0)
    { }

    ActorBase::Result ChangeBlockPlus::create() {
        u32 isPermanent;
        
        mWasNybbleTenInitiallyNotZero = false;

        mHeight = mParam1 & 0xf;
        if (mHeight == 0) {
            mHeight = 1;
        }

        mWidth = mParam1 >> 8 & 0xf;
        if (mWidth == 0) {
            mWidth = 1;
        }

        if (mParam0 >> 0x1f & 0x1) {
            mUsingLocation = true;
            scanLocationData(mParamEx.course.link_id);
        }

        mObjectType = mParam1 >> 0x10 & 0xFFFF;

        mNoDestroy = mParam0 >> 0x1b & 0x1;
        mIsPermanent = mParam0 >> 0x1c & 0x1;
        mSilent = mParam0 >> 0x1d & 0x1;
        mNoResidue = mParam0 >> 0x1e & 0x1;

        mFillPattern = mParam0 >> 0x10 & 0xf;
        mSpawnOrDestroy = mParam0 >> 0xC & 0xf;

        u64 events = readEvents();
        mOldEvents = events;

        if (events != 0) {
            if (mSpawnOrDestroy == 0) {
                apply(0, 1, 1);
                isPermanent = mIsPermanent;
            } else {
                apply(1, 1, 1);
                isPermanent = mIsPermanent;
                mWasNybbleTenInitiallyNotZero = true;
            }

            if (isPermanent != 0) {
                return cResult_Failed;
            }
        }
        return cResult_Success;
    }

    bool ChangeBlockPlus::execute() {
        update();

        if (!mWasNybbleTenInitiallyNotZero) {
            screenOutCheck(cScreenOutFlag_SkipNone);
        }

        return true;
    }

    u64 ChangeBlockPlus::readEvents() {
        u32 uVar1;
        u32 uVar2;
        u32 uVar3;
        u32 uVar4;
        u64 out;
        
        uVar2 = mSwitchFlag0;
        uVar3 = 0;
        uVar4 = 0;
        if (uVar2 != 0) {
            uVar3 = uVar2 - 1;
            uVar4 = SwitchFlagMgr::instance()->getSwitchFlag() & (1 << (uVar3 & 0x3f));
            uVar3 = SwitchFlagMgr::instance()->getSwitchFlag() & (1 << (uVar2 + 0x1f & 0x3f) | 0 << (uVar3 & 0x3f) | 1U >> (0x20 - uVar3 & 0x3f));
        }
        uVar2 = mSwitchFlag1;
        if (uVar2 != 0) {
            uVar1 = uVar2 - 1;
            uVar4 = uVar4 | SwitchFlagMgr::instance()->getSwitchFlag() & (1 << (uVar1 & 0x3f));
            uVar3 = uVar3 | SwitchFlagMgr::instance()->getSwitchFlag() & (1 << (uVar2 + 0x1f & 0x3f) | 0 << (uVar1 & 0x3f) | 1U >> (0x20 - uVar1 & 0x3f));
        }
        out = uVar4;
        out = out << 0x20; // maybe
        out += uVar3;

        return out;
    }

    void ChangeBlockPlus::update() {
        u32 dVar1;

        u64 events = readEvents();
        if (events != mOldEvents) {
            if (mSpawnOrDestroy == 0) {
                if (events == 0) {
                    apply(1, mNoResidue, mSilent);
                    dVar1 = mIsPermanent;
                    mOldEvents = 0;
                    mWasNybbleTenInitiallyNotZero = false;
                } else {
                    apply(0, mNoResidue, mSilent);
                    dVar1 = mIsPermanent;
                    mOldEvents = events;
                    mWasNybbleTenInitiallyNotZero = true;
                }
            } else if (events == 0) {
                apply(0, mNoResidue, mSilent);
                dVar1 = mIsPermanent;
                mOldEvents = 0;
                mWasNybbleTenInitiallyNotZero = false;
            } else {
                apply(1, mNoResidue, mSilent);
                dVar1 = mIsPermanent;
                mOldEvents = events;
                mWasNybbleTenInitiallyNotZero = true;
            }
            if (dVar1 != 0) {
                mDeleteRequestFlag = true;
            }
        }
        return;
    }

    void ChangeBlockPlus::apply(u32 destroy_or_spawn, u32 no_effect, u32 no_sound) {

        // static sead::SafeArray<u32, 6> cUnitConversionArray = {
        //     0x51, 0x2, 0xc, 0x50, 0xf, 0xf
        // };
        uint unit;
        u32 profileID;
        u32 fillPattern;
        u32 width;
        uint height;
        uint unitPosY;
        uint dy;
        uint dx;
        sead::Vector3f effectPos;
        sead::Vector2f audioPosIn;
        uint unitPosX;
        bool ifNotInvertedCheckers;
        bool notInvertedCheckers;
        float posX;
        
        fillPattern = mFillPattern;
        notInvertedCheckers = fillPattern != 2;
        posX = mPos.x;
        unit = 0;
        if (destroy_or_spawn) {
            unit = mObjectType;
        }
        if (mUsingLocation) {
            mHeight = mTileSize.y;
            mWidth = mTileSize.x;
        }
        height = mHeight;
        dy = 0;
        unitPosY = s32(-mPos.y) & 0xfff0; // not sure about this unit conversion
        if (height != 0) {
            width = mWidth;
            ifNotInvertedCheckers = notInvertedCheckers;
            do {
                dx = 0;
                if (width != 0) {
                unitPosX = s32(posX) & 0xfff0;
                do {
                    if (ifNotInvertedCheckers) {
                        u16* bgUnit = Bg::getUnitCurrentCdFile(unitPosX, unitPosY,mLayer);
                        
                        if (!mNoDestroy || destroy_or_spawn) {
                            if (mUsingLocation && destroy_or_spawn) {
                                *bgUnit = mTileData[dy * mTileSize.x + dx];
                            } else {
                                *bgUnit = unit;
                            }
                        }
                        
                        if ((no_effect == 0) && (mObjectType != 2)) {
                            effectPos.z = mPos.z;
                            effectPos.x = f32(unitPosX) + 8.0f;
                            effectPos.y = -f32(unitPosY) - 8.0f;
                            // profileID = ActorBase::getProfileId(void)(this);
                            // if (profileID == 0x2ed) {
                            //     effectPos.y = effectPos.y + 8.0;
                            // }
                            EffectCreateUtil::createEffect(RP_ChangeBlock_Change, &effectPos, 0x0, 0x0);
                        }

                        if (no_sound == 0) {
                            effectPos.x = mPos.x;
                            effectPos.y = mPos.y;
                            effectPos.z = mPos.z;
                            // if (RDashMgr::instance->isNSLU) {
                            //     effectPos.z = (_)._._.position.z;
                            //     effectPos.x = unitPosX + 8.0;
                            //     effectPos.y = -unitPosY - 8.0;
                            // }
                            audioPosIn.x = effectPos.x;
                            audioPosIn.y = effectPos.y;
                            if (destroy_or_spawn == 0) {
                                GameAudio::instance()->getAudioObjMap()->startSound("SE_OBJ_KAKIKAE_B_DISAPPEAR", audioPosIn);
                            } else {
                                GameAudio::instance()->getAudioObjMap()->startSound("SE_OBJ_KAKIKAE_B_APPEAR", audioPosIn);
                            }
                        }
                    width = mWidth;
                    fillPattern = mFillPattern;
                }
                if (fillPattern != 0) {
                    ifNotInvertedCheckers = ifNotInvertedCheckers ^ 1;
                }
                dx = dx + 1 & 0xffff;
                unitPosX = unitPosX + 0x10 & 0xffff;
                } while (dx < width);
                height = mHeight;
            }
            if (fillPattern != 0) {
                ifNotInvertedCheckers = notInvertedCheckers ^ 1;
                notInvertedCheckers = ifNotInvertedCheckers;
            }
            dy = dy + 1 & 0xffff;
            unitPosY = unitPosY + 0x10 & 0xffff;
            } while (dy < height);
        }
        return;
    }

    void ChangeBlockPlus::scanLocationData(u8 location_id) {
        // find location
        const CourseDataFile* area = CourseData::instance()->getFile(CourseInfo::instance()->getFileNo());
        const Location* location = area->getLocation(nullptr, location_id);
        
        if (location == nullptr) {
            tk::fatal("Magic Tile God failed to get location");
        }
        
        // init tile data
        u32 locX = location->offset.x & ~0xF;
        u32 locY = location->offset.y & ~0xF;
        mTileSize.x = (location->size.x + (location->offset.x & 0xF) + 0xF) / 16;
        mTileSize.y = (location->size.y + (location->offset.y & 0xF) + 0xF) / 16;
        
        if (mTileSize.x == 0 || mTileSize.y == 0) {
            tk::fatal("Magic Tile God failed to get tile size");
        }
        
        u8 layer = ((mParamEx.course.movement_id & 0x3) == 0x3) ? 0 : mParamEx.course.movement_id & 0x3;
        // scan and copy tile data
        mTileData = new u16[mTileSize.x * mTileSize.y];
        for (u32 y = 0; y < mTileSize.y; y++) {
            for (u32 x = 0; x < mTileSize.x; x++) {
                u16* tile = Bg::getUnitCurrentCdFile(locX + x * 16, locY + y * 16, layer);
                mTileData[x + y * mTileSize.x] = tile ? *tile : 0;
            }
        }
    }
}
