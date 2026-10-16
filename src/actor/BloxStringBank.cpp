#include <blox/actor/BloxStringBank.h>
#include <blox/Blox.h>
#include <actor/ActorMgr.h>
#include <red/utility/SpriteUtil.h>
#include <red/utility/Strybble.h>

namespace {

    template <s32 TOutCount>
    s32 Decode(const blox::StringBank* from, u8 (&to)[TOutCount]) {
        u8 nybbles[12];
        red::SpriteUtil::getBitRange<0, 96>(from, nybbles);
        
        u8 codes[17] = { 0 };
        red::StrybbleUtil::decodeFromBitstream(nybbles, codes, 16);
        
        return red::StrybbleUtil::decodeFromChars(codes, to);
    }

} // namespace

SEAD_RTTI_OVERRIDE_IMPL(blox::StringBank, Actor)

const ActorCreateInfo blox::StringBank::cCreateInfo = {
    .offset_x = 0, .offset_y = 0,
    .spawn_range = {
        .offset_x = 0, .offset_y = 0,
        .half_size_x = 0, .half_size_y = 0
    },
    .cull_range = { 
        .up = 0, .down = 0, .left = 0, .right = 0
    },
    .flag = ActorCreateInfo::cFlag_IgnoreSpawnRange
};

Profile* blox::StringBank::sProfile = blox::getRegistrar()->newProfile<blox::StringBank>("string_bank")
    .createInfo(cCreateInfo)
    .build();

blox::StringBank::StringBank(const ActorCreateParam& param)
    : Actor(param)
    , mType(StringBank::Type::Primary)
    , mBankID(0)
    , mString{}
    , mScanAttempt(0)
    , mHasUpTo(StringBank::Type::Primary)
    , mInited(false)
{ }

ActorBase::Result blox::StringBank::create() {
    // Setting: Bank ID
    mBankID = mParamEx.course.init_state_flag;
    if (mBankID == 0) {
        tk::fatal("StringBank: Bank ID was unset.");
        return cResult_Failed;
    }
    
    // Setting: Bank Type
    mType = static_cast<StringBank::Type>(mLayer & 0b11);
    
    if (mType == StringBank::Type::Primary) {
        // Setting: Has up to this type
        mHasUpTo = static_cast<StringBank::Type>((mLayer >> 2) & 0b11);
    }
    
    scan();
    
    return cResult_Success;
}

bool blox::StringBank::execute() {
    scan();
    
    return true;
}

void blox::StringBank::scan() {
    if (mType != StringBank::Type::Primary)
        return;
    
    if (mInited)
        return;

    mScanAttempt++;
    if (mScanAttempt > 5) {
        tk::fatal("StringBank: Failed to find other banks after 5 attempts...");
    }
    
    if (mHasUpTo == StringBank::Type::Primary) {
        ::Decode(this, mString);
        
        mInited = true;
        return;
    }
    
    ActorMgr* actorMgr = ActorMgr::instance();
    
    for (auto it = actorMgr->getActorBegin(); it != actorMgr->getActorEnd(); it++) {
        if (StringBank* bank = sead::DynamicCast<StringBank>(*it)) {
            if (bank == this)
                continue;
            
            if (bank->getBankID() == mBankID) {
                switch (bank->getType()) {
                    case StringBank::Type::Primary: {
                        tk::fatal("StringBank: Found duplicate primary banks with ID: %d", mBankID);
                        break;
                    }
                    
                    case StringBank::Type::Secondary: {
                        if (mSecondaryBank.isValid()) {
                            tk::fatal("StringBank: Found duplicate secondary banks with ID: %d", mBankID);
                        }
                        
                        mSecondaryBank = bank->getActorUniqueID();
                        
                        break;
                    }
                    
                    case StringBank::Type::Final: {
                        if (mFinalBank.isValid()) {
                            tk::fatal("StringBank: Found duplicate final banks with ID: %d", mBankID);
                        }
                        
                        mFinalBank = bank->getActorUniqueID();
                        
                        break;
                    }
                }
            }
        }
    }
    
    if (mHasUpTo == StringBank::Type::Secondary && mSecondaryBank.isValid()) {
        const StringBank* secondaryBank = ActorMgr::instance()->getActorPtr<StringBank>(mSecondaryBank);
        if (secondaryBank == nullptr)
            return;
        
        u8 str1[18] = { 0 };
        s32 len1 = ::Decode(this, str1);
        u8 str2[18] = { 0 };
        s32 len2 = ::Decode(secondaryBank, str2);
        
        std::memcpy(mString, str1, len1);
        std::memcpy(mString + len1, str2, len2);
        mString[len1 + len2] = '\0';
        
        mInited = true;
        return;
    }
    
    if (mHasUpTo == StringBank::Type::Final && mSecondaryBank.isValid() && mFinalBank.isValid()) {
        const StringBank* secondaryBank = ActorMgr::instance()->getActorPtr<StringBank>(mSecondaryBank);
        const StringBank* finalBank = ActorMgr::instance()->getActorPtr<StringBank>(mFinalBank);
        if (secondaryBank == nullptr || finalBank == nullptr)
            return;
        
        u8 str1[18] = { 0 };
        s32 len1 = ::Decode(this, str1);
        u8 str2[18] = { 0 };
        s32 len2 = ::Decode(secondaryBank, str2);
        u8 str3[18] = { 0 };
        s32 len3 = ::Decode(finalBank, str3);
        
        std::memcpy(mString, str1, len1);
        std::memcpy(mString + len1, str2, len2);
        std::memcpy(mString + len1 + len2, str3, len3);
        mString[len1 + len2 + len3] = '\0';
        
        mInited = true;
        return;
    }
}