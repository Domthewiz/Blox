#include "actor/ActorBase.h"
#include "map_obj/BlockCoinBase.h"
#include "map_obj/RouletteBlock.h"
#include "map_obj/ChibiYoshiMgr.h"
#include "player/PlayerMgr.h"
#include <blox/map_obj/CustomRouletteBlock.h>
#include <blox/Blox.h>
#include <red/util/SpriteUtil.h>
#include <player/PlayerObject.h>

// Was originally going to be the NSMB2 roulette block, but it ended up being so much more.
// TODO: Fix 3-up moon and baby yoshi y spawn offset
namespace blox {
    SEAD_RTTI_OVERRIDE_IMPL(CustomRouletteBlock, RouletteBlock);

    Profile* CustomRouletteBlock::sProfile = blox::getRegistrar()->newProfile<CustomRouletteBlock>("custom_roulette_block")
        .resources<"block_roulette", "custm_roulette", "I_yoshichibi_egg", "YoshiChibi_TexBalloon", "YoshiChibi_TexBubble", "YoshiChibi_TexLight", "balloon">(ProfileInfo::cResType_Course)
        .drawPriority(232)
        .build();

    CustomRouletteBlock::CustomRouletteBlock(const ActorCreateParam& param)
        : RouletteBlock(param)
        , mModelCustom(nullptr)
        , mCustomSpinCountdown(0)
        , mCustomRouletteIndex(0)
        , mCustomCountdownApplied(false)
    { }

    ActorBase::Result CustomRouletteBlock::create() {
        tk::println("a");
        mModelCustom = AnimModel::create("custm_roulette", "block_roulette", 1, 1, 1);
        mModelCustom->playTexAnim("block_roulette");
        // mTexAnim = mModelCustom->getTexAnim(0); // this might cause some problems
        // mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(0.0f);
        mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(red::SpriteUtil::getNybble6(this));
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

    bool CustomRouletteBlock::execute() {
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
            if (mCustomRouletteIndex >= (mParam0 >> 0x1C & 0x7)) {
                mCustomRouletteIndex = 0;
            } else {
                mCustomRouletteIndex++;
            }
            // tk::println("content index %u",mCustomRouletteIndex);
            mModelCustom->getTexAnim(0)->getFrameCtrl().setFrame(red::SpriteUtil::getNybbleRange(this, mCustomRouletteIndex + 6, mCustomRouletteIndex + 6));
        }
        
        updateModel();
        calcMdl_();
        
        return true;
    }
    
    bool CustomRouletteBlock::draw() {
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

    void CustomRouletteBlock::calcMdl_() {
        if (mModelCustom != nullptr) {
            mModelCustom->update(mPos, mAngle, mScaleFactor);
        }
    }

    void CustomRouletteBlock::destroy() {
        changeState(StateID_Wait);
        // doShock_();
    }
    void CustomRouletteBlock::destroy2() {
        changeState(StateID_Wait);
        // doShock_();
    }
    
    void CustomRouletteBlock::vf2DC() {
        RouletteBlock::vf2DC();
        u8 customContentIndex = red::SpriteUtil::getNybbleRange(this, mCustomRouletteIndex + 6, mCustomRouletteIndex + 6);
        switch (customContentIndex) {
            case cRouletteContent_Mushroom: {
                mContent = cContent_Mushroom;
                break;
            }
            case cRouletteContent_Star: {
                mContent = cContent_Star;
                break;
            }
            case cRouletteContent_1UP: {
                mContent = cContent_LifeMushroom;
                break;
            }
            case cRouletteContent_FireFlower: {
                mContent = cContent_FireFlower;
                break;
            }
            case cRouletteContent_PropellerMushroom: {
                mContent = cContent_PropellerMushroom;
                break;
            }
            case cRouletteContent_IceFlower: {
                mContent = cContent_PropellerMushroom;
                break;
            }
            case cRouletteContent_PenguinSuit: {
                mContent = cContent_PropellerMushroom;
                break;
            }
            case cRouletteContent_SquirrelMushroom: {
                mContent = cContent_SquirrelMushroom;
                break;
            }
            case cRouletteContent_MiniMushroom: {
                mContent = cContent_MiniMushroom;
                break;
            }
            case cRouletteContent_Coin: {
                mContent = cContent_Coin;
                break;
            }
            case cRouletteContent_Yoshi: {
                mContent = cContent_Yoshi;
                break;
            }
            case cRouletteContent_Spring: {
                mContent = cContent_Spring;
                break;
            }
            default: {
                mContent = cContent_Empty;
                break;
            }
        }
    }

    void CustomRouletteBlock::spawnItemUp() {
        RouletteBlock::spawnItemUp();
        u8 customContentIndex = red::SpriteUtil::getNybbleRange(this, mCustomRouletteIndex + 6, mCustomRouletteIndex + 6);
        switch (customContentIndex) {
            case cRouletteContent_BubbleChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 0, 1);
                break;
            }
            case cRouletteContent_BalloonChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 1, 1);
                break;
            }
            case cRouletteContent_GlowChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 2, 1);
                break;
            }
            case cRouletteContent_3UP: {
                u32 playerCount = PlayerMgr::instance()->getNumInGame();
                mContent = cContent_LifeMoon;
                if (playerCount > 1) {
                    spawnMultiPowerup(mPos, 0, 1, true);
                    break;
                }
                spawnPowerup(mPos, 0, 1, true);
                break;
            }
        }
    }

    void CustomRouletteBlock::spawnItemDown() {
        RouletteBlock::spawnItemDown();
        u8 customContentIndex = red::SpriteUtil::getNybbleRange(this, mCustomRouletteIndex + 6, mCustomRouletteIndex + 6);
        switch (customContentIndex) {
            case cRouletteContent_BubbleChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 0, 2);
                break;
            }
            case cRouletteContent_BalloonChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 1, 2);
                break;
            }
            case cRouletteContent_GlowChibiYoshi: {
                mContent = cContent_Empty;
                ChibiYoshiMgr::spawnEgg(mPos, 2, 2);
                break;
            }
            case cRouletteContent_3UP: {
                u32 playerCount = PlayerMgr::instance()->getNumInGame();
                mContent = cContent_LifeMoon;
                if (playerCount > 1) {
                    spawnMultiPowerup(mPos, 0, 1, true);
                    break;
                }
                spawnPowerup(mPos, 0, 1, true);
                break;
            }
        }
    }
}

// CoinRoulette coin no
// 5
// 50
// 10
// 5
// 10
// 30
// 5
// 10
// 30
// 5
// 10
// 30