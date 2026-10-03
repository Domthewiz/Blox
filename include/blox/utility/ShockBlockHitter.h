#pragma once

#include "actor/Actor.h"
#include "map_obj/BlockCoinBase.h"
#include "system/TouchDrcMgr.h"
#include "utility/Direction.h"
#include <effect/EffectCreateUtil.h>
#include <map_obj/BlockMgr.h>
#include <actor/ActorMgr.h>
#include <map/Bg.h>

static constexpr u8 cRingTimeInterval = 5;
static constexpr u8 cInitialDelay = 10;
static constexpr u8 cMaximumRingRadius = 4;
static constexpr f32 cPowEffectScale = 0.5f;
static constexpr f32 cQuake1EffectScale = 2.0f;
static constexpr f32 cQuake2EffectScale = 3.0f;

// Not a port from NSMB2, made completely from scratch 
// TODO: somehow improve this
class Block : public BlockCoinBase {};

class ShockBlockHitter
{
public:
    ShockBlockHitter()
    : mOriginPosition(sead::Vector3f::zero)
    , mTimer(0)
    , mRingRadius(1)
    , mHitPlayerNo(-1)
    , mActive(false)
    {}

public:
    void initiateShockBlockHitter(const sead::Vector3f& origin, s8 player_no) {
        if (mActive) {
            return;
        }

        mActive = true;
        mRingRadius = 1;
        mTimer = cInitialDelay;
        mOriginPosition = origin;
        mHitPlayerNo = player_no;
        
        // TODO: Make better effects lmao
        sead::Vector3f effectPos(mOriginPosition.x, mOriginPosition.y + 8.0f, 4500.0f);
        sead::Vector3f effectScale(cPowEffectScale, cPowEffectScale, cPowEffectScale);
        EffectCreateUtil::createEffect(RP_Pow, &effectPos, nullptr, &effectScale);

        effectScale = sead::Vector3f(cQuake1EffectScale,cQuake1EffectScale,cQuake1EffectScale);
        EffectCreateUtil::createEffect(RP_Coinedit_StarCoin_on, &effectPos, nullptr, &effectScale);

        effectScale = sead::Vector3f(cQuake2EffectScale,cQuake2EffectScale,cQuake2EffectScale);
        EffectCreateUtil::createEffect(RP_Coinedit_StarCoin_on, &effectPos, nullptr, &effectScale);
    }
    
    bool isActive() const {
        return mActive;
    }

    void executeQuake() {
        if (!mActive) {
            return;
        }

        if (mRingRadius > cMaximumRingRadius) {
            mActive = false;
            return;
        }

        if (mTimer < cRingTimeInterval) {
            mTimer++;
            return;
        }
        
        tryTilesetBlocks(mRingRadius);
        tryActorBlocks(mRingRadius);
        mTimer = 0;
        mRingRadius++;
    }

private:
    static bool isInQuakeRing(const sead::Vector3f& origin_position, const sead::Vector3f& block_position, u8 ring_radius) {
        u32 dy = sead::Mathi::abs(static_cast<u32>(block_position.y - origin_position.y));
        u32 dx = sead::Mathi::abs(static_cast<u32>(block_position.x - origin_position.x));
        
        dy = ((dy / 4) + 1) / 4;
        dx = ((dx / 4) + 1) / 4;
    
        const u32 outer = ring_radius + 1;

        return (dx < outer && dy < outer) && (dx >= ring_radius || dy >= ring_radius);
    }
    
    void tryActorBlocks(u8 ring_radius) {
        ActorMgr* actorMgr = ActorMgr::instance();
        for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
            if (*it == nullptr) {
                continue;
            }
            
            ActorBlockBase* targetActor = sead::DynamicCast<ActorBlockBase>(*it);
            if (!targetActor) {
                continue;
            }

            if (targetActor->isRequestedDelete()) {
                continue;
            }
            
            if (!targetActor->isActive() && targetActor->getBlockType() == ActorBlockBase::cType_Hit && targetActor->isState(ActorBlockBase::StateID_Wait)) {
                continue;
            }
            
            if (!isInQuakeRing(mOriginPosition, targetActor->getPos(), ring_radius)) {
                continue;
            }

            if (targetActor->getBaseContent() == BlockCoinBase::Content::cContent_Empty && targetActor->getBlockType() == ActorBlockBase::cType_Renga) {
                targetActor->destroy();
            } else {
                targetActor->setBumpUpTimer(8);
            }
        }
    }
    
    void tryTilesetBlocks(u8 ring_radius) {
        
        BlockMgr::DestroyParam breakBlockParam;
        breakBlockParam.fragment_type = 1; // renga
        breakBlockParam._c = 1;
        breakBlockParam.player_no = mHitPlayerNo;
        breakBlockParam.sensor_id = 2;

        BlockMgr::HitParam blockHitParam;
        blockHitParam.player_no = mHitPlayerNo;
        blockHitParam.sensor_id = 2;
        blockHitParam.player_breaks_bricks = 1;

        sead::Vector2u getUnitCheckPos;

        sead::Vector2f breakPos;

        sead::Vector2u originUnsigned;
        originUnsigned.x = (s32)(mOriginPosition.x);  // I apologize for the c-style casts
        originUnsigned.y = (s32)(-mOriginPosition.y); // I apologize for the c-style casts

        // tx and ty are the target x and y relative to the origin (0, 0)
        for (s32 tx = -ring_radius; tx <= ring_radius; tx++) {
            for (s32 ty = -ring_radius; ty <= ring_radius; ty++) {
                // Not a big fan of all this nesting
                if ((tx == ring_radius) || (tx == -ring_radius) || ((ty == ring_radius) || (ty == -ring_radius)) && (tx != 0 || ty != 0)) {

                    getUnitCheckPos.x = (originUnsigned.x & ~0xF) + ((tx * 16));
                    getUnitCheckPos.y = (originUnsigned.y & ~0xF) - ((ty * 16) + 16);

                    breakPos.x = static_cast<f32>(getUnitCheckPos.x);
                    breakPos.y = -static_cast<f32>(getUnitCheckPos.y);

                    blockHitParam.position.x =  breakPos.x;
                    blockHitParam.position.y = breakPos.y;

                    breakBlockParam.position.x = (tx * 16.0f) + mOriginPosition.x - 8.0f;
                    breakBlockParam.position.y = (ty * 16.0f) + mOriginPosition.y + 16.0f;

                    u16* tile = Bg::getUnitCurrentCdFile(getUnitCheckPos.x,getUnitCheckPos.y, 0);
                    BgUnitCode::Type unitCode = Bg::instance()->getUnitType(breakPos.x,breakPos.y,0);
                    blockHitParam.content.index = *tile >> 10 & 0xF;
                    
                    // Do this check in order to avoid breaking bricks with boost mode, hence spawning a coin
                    if (unitCode == BgUnitCode::Type::cType_BreakBlock && blockHitParam.content.renga == BlockMgr::cRengaContent_Empty) {
                        BlockMgr::instance()->doDestroyAt(breakBlockParam);

                    } else if (unitCode == BgUnitCode::Type::cType_BreakBlock && blockHitParam.content.renga == BlockMgr::cRengaContent_None) {
                        BlockMgr::instance()->doDestroyAt(breakBlockParam);
                        *tile = cUnitID_Coin;
                        
                    } else if (unitCode == BgUnitCode::Type::cType_BreakBlock || unitCode == BgUnitCode::Type::cType_Q_Block) {
                        BlockMgr::instance()->feverModeHitBlockAt(blockHitParam.position);
                    }
                }
            }
        }
    }

    // void scanForBlockProfile(const sead::Vector2f& pos) {
    //     ActorMgr* actorMgr = ActorMgr::instance();
    //         for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
    //             if (*it == nullptr) {
    //                 continue;
    //             }
                
    //             Block* targetActor = sead::DynamicCast<Block>(*it);
    //             if (!targetActor) {
    //                 continue;
    //             }
    
    //             // if (!targetActor->getParent()) {
    //             //     continue;
    //             // }
    //             if (targetActor->getProfile() == red::ProfileEx::get(ProfileInfo::cProfileID_Block)) {
    //                 continue;
    //             }

    //             if (pos.x == targetActor->getPos().x && pos.y == targetActor->getPos().y) {
    //                 continue;
    //             }

    //             targetActor->getPos().x += 16.0f;
    
    //             return;
    //         }
    // }
    
protected:
    sead::Vector3f mOriginPosition;
    u8 mTimer;
    u8 mRingRadius;
    s8 mHitPlayerNo;
    bool mActive;
};