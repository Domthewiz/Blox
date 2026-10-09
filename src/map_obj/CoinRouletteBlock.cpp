#include "actor/ActorBase.h"
#include "actor/ActorMgr.h"
#include "container/seadSafeArray.h"
#include "map_obj/BlockCoinBase.h"
#include "map_obj/RouletteBlock.h"
#include "utility/Direction.h"
#include <blox/map_obj/CoinRouletteBlock.h>
#include <blox/Blox.h>
#include <red/util/SpriteUtil.h>
#include <player/PlayerObject.h>
#include <blox/map_obj/ActorRouletteCoinJump.h>

namespace blox {

    static sead::SafeArray<f32, 12> cCoinRouletteSpinArray1 { 
        0.0f, 1.0f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 1.0f, 2.0f, 0.0f, 3.0f, 1.0f
    };

    SEAD_RTTI_OVERRIDE_IMPL(CoinRouletteBlock, RouletteBlock);

    Profile* CoinRouletteBlock::sProfile = blox::getRegistrar()->newProfile<CoinRouletteBlock>("coin_roulette_block")
        .resources<"block_roulette", "coins_roulette", "ten_coin">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .build();

    CoinRouletteBlock::CoinRouletteBlock(const ActorCreateParam& param)
        : RouletteBlock(param)
        , mModelCustom(nullptr)
        , mCustomSpinCountdown(0)
        , mCustomRouletteIndex(0)
        , mCustomCountdownApplied(false)
        , mWasHitFromBelow(false)
    { }

    ActorBase::Result CoinRouletteBlock::create() {
        tk::println("a");
        mModelCustom = AnimModel::create("coins_roulette", "block_roulette", 1, 1, 1);
        mModelCustom->playTexAnim("block_roulette");
        // mTexAnim = mModelCustom->getTexAnim(0); // this might cause some problems
        // mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(0.0f);
        mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(0.0f);
        mModelCustom->getTexAnim(0)->getFrameCtrl().setRate(0.0f);

        tk::println("a");
        if (!RouletteBlock::create()) {
            return cResult_Failed;
        }
        tk::println("a");
        // Custom spin interval
        const u8 nybble14 = mParam1 >> 0x18 & 0xF;
        mCustomSpinCountdown = (nybble14) ? (nybble14 * 2) : 10;
        mCustomSpinCountdown--;
        tk::println("bruh %u", mCustomSpinCountdown);

        mItemCreateYOffsetUpMulti -= 8.0f;
        mItemCreateYOffsetUpSingle -= 8.0f;
        mItemCreateYOffsetDownMulti -= 8.0f;
        mItemCreateYOffsetDownSingle -= 8.0f;
        
        tk::println("a");
        updateModel();
        tk::println("a");
        calcMdl_();
        tk::println("a");
        return cResult_Success;
    }

    bool CoinRouletteBlock::execute() {
        u8 prevCountdownVal = mRouletteCountdown;

        if (!RouletteBlock::execute()) {
            return false;
        }

        // Do this in order to avoid glitches with boost mode
        if (prevCountdownVal == mRouletteCountdown) {
            calcMdl_();
            return true;
        }
        
        if (mAlreadyUsed) {
            return true;
        }

        if (mRouletteCountdown == 9) {
            if (!mCustomCountdownApplied) {
                mCustomCountdownApplied = true;
                mRouletteCountdown = mCustomSpinCountdown;
            }
        } else if (mRouletteCountdown == 0) {
            mCustomCountdownApplied = false;
            if (mCustomRouletteIndex >= 11) {
                mCustomRouletteIndex = 0;
            } else {
                mCustomRouletteIndex++;
            }
            // tk::println("content index %u",mCustomRouletteIndex);
            mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(cCoinRouletteSpinArray1[mCustomRouletteIndex]);
            if (cCoinRouletteSpinArray1[mCustomRouletteIndex] == 3.0f) {
                GameAudio::getAudioObjMap()->startSound("SE_OBJ_LIFT_LIMIT_0", mPos);
            }
        }
        
        updateModel();
        calcMdl_();
        
        return true;
    }
    
    bool CoinRouletteBlock::draw() {
        if (mAlreadyUsed) {
            if (!RouletteBlock::draw()) {
                return false;
            }
            return true;
        }

        if (mModelCustom != nullptr) {
            mModelCustom->draw();
        }
        
        return true;
    }

    void CoinRouletteBlock::calcMdl_() {
        if (mModelCustom != nullptr) {
            mModelCustom->update(mPos, mAngle, mScaleFactor);
        }
    }

    void CoinRouletteBlock::destroy() {
        changeState(StateID_Wait);
        // doShock_();
    }
    void CoinRouletteBlock::destroy2() {
        changeState(StateID_Wait);
        // doShock_();
    }
    
    void CoinRouletteBlock::vf2DC() {
        RouletteBlock::vf2DC();
        mContent = cContent_Empty;
    }

    void CoinRouletteBlock::preSpawnItem() {
        if (mWasHitFromBelow) {
            mSpawnDirection = cDirType_Up;
            // if (red::SpriteUtil::getNybble20(this) == 1) {
            //     mBumpMode = cBumpMode_Up;
            // }
        } else {
            mSpawnDirection = cDirType_Down;
            // if (red::SpriteUtil::getNybble20(this) == 1) {
            //     mBumpMode = cBumpMode_Down;
            // }
        }

        u8 customContentIndex = cCoinRouletteSpinArray1[mCustomRouletteIndex];
        switch (customContentIndex) {
            case cCoinRouletteContent_5: {
                ActorBlockBase::spawnCoinShower();
                break;
            }
            case cCoinRouletteContent_10: {
                spawnTenCoin(1, !mWasHitFromBelow);
                break;
            }
            case cCoinRouletteContent_30: {
                spawnTenCoin(3, !mWasHitFromBelow);
                break;
            }
            case cCoinRouletteContent_50: {
                spawnTenCoin(5, !mWasHitFromBelow);
                break;
            }
            default: {
                mContent = cContent_Empty;
                break;
            }
        }
        
        mWasHitFromBelow = false;
        return ActorBlockBase::preSpawnItem();
    }

    void CoinRouletteBlock::onUpMoveStart() {
        mWasHitFromBelow = true;
        return ActorBlockBase::onUpMoveStart();
    }

    void CoinRouletteBlock::spawnTenCoin(u8 no, bool down) {
        GameAudio::getAudioObjMap()->startSound("SE_OBJ_GET_COIN_SHOWER", mPos);
        
        ActorCreateParam item;
        item.profile = ActorRouletteCoinJump::getProfile();
        item.position = mPos;
        if (down) {
            item.position.y -= 8.0f;
        } else {
            item.position.y += 8.0f;
        }

        if (!no || no > 10) {
            tk::fatal("\"spawnTenCoin()\" number is out of range 1-10.");
        }

        for (u8 i = no; i > 0; i--) {
            item.param_0 = 0x00000000;
            item.param_0 += i;
            ActorBase* tenCoin = ActorMgr::instance()->createImmediately(item);

            if (down) {
                static_cast<ActorRouletteCoinJump*>(tenCoin)->getSpeedVec().y *= -1;
            }
        }
    }
}

// CoinRoulette coin no
// 5
// 10
// 30
// 5
// 10
// 30
// 5
// 10
// 30
// 5
// 50
// 10

