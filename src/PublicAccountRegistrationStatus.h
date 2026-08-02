#ifndef LEZ_PUBLIC_ACCOUNT_REGISTRATION_STATUS_H
#define LEZ_PUBLIC_ACCOUNT_REGISTRATION_STATUS_H

namespace LezWallet {

enum class PublicAccountRegistrationState {
    Unknown,
    Uninitialized,
    Initialized,
};

struct PublicAccountRegistrationStatus {
    bool known = false;
    bool needsInitialization = false;
    bool initialized = false;
};

constexpr int RegistrationStatusRefreshInitialDelayMs = 3000;
constexpr int RegistrationStatusRefreshMaxDelayMs = 30000;
constexpr int RegistrationStatusRefreshMaxAttempts = 6;

constexpr PublicAccountRegistrationStatus registrationStatusFor(
    const PublicAccountRegistrationState state)
{
    switch (state) {
    case PublicAccountRegistrationState::Unknown:
        return {};
    case PublicAccountRegistrationState::Uninitialized:
        return {true, true, false};
    case PublicAccountRegistrationState::Initialized:
        return {true, false, true};
    }
    return {};
}

constexpr bool shouldRetryRegistrationStatus(
    const bool statusUnavailable,
    const bool submittedRegistrationPending,
    const int completedAttempts)
{
    return (statusUnavailable || submittedRegistrationPending)
        && completedAttempts < RegistrationStatusRefreshMaxAttempts;
}

constexpr int registrationStatusRefreshDelayMs(const int completedAttempts)
{
    int delay = RegistrationStatusRefreshInitialDelayMs;
    for (int attempt = 0;
         attempt < completedAttempts && delay < RegistrationStatusRefreshMaxDelayMs;
         ++attempt) {
        delay *= 2;
        if (delay > RegistrationStatusRefreshMaxDelayMs)
            delay = RegistrationStatusRefreshMaxDelayMs;
    }
    return delay;
}

} // namespace LezWallet

#endif // LEZ_PUBLIC_ACCOUNT_REGISTRATION_STATUS_H
