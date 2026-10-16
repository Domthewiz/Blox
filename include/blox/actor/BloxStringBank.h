#pragma once

#include <actor/Actor.h>
#include <actor/Profile.h>

namespace blox {

class StringBank : public Actor {
    SEAD_RTTI_OVERRIDE(StringBank, Actor)

public:
    enum Type : u8 {
        Primary = 0,
        Secondary = 1,
        Final = 2
    };

public:
    static Profile* sProfile;

    StringBank(const ActorCreateParam& param);
    ~StringBank() override = default;

    Result create() override;
    bool execute() override;

    [[nodiscard]]
    Type getType() const {
        return Type(mType);
    }

    [[nodiscard]]
    const char* getString() const {
        return (const char*)(mString);
    }

    [[nodiscard]]
    u8 getBankID() const {
        return mBankID;
    }

    [[nodiscard]]
    bool isInited() const {
        return mInited;
    }

private:
    void scan();

private:
    static const ActorCreateInfo cCreateInfo;

    StringBank::Type mType;
    u8 mBankID;
    u8 mString[64];
    u8 mScanAttempt;
    StringBank::Type mHasUpTo;
    bool mInited;
    ActorUniqueID mSecondaryBank;
    ActorUniqueID mFinalBank;
};

}