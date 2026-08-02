#include <logos_test.h>

#include "LEZWalletAccountModel.h"
#include "PublicAccountRegistrationStatus.h"

using LezWallet::PublicAccountRegistrationState;

LOGOS_TEST(registration_state_projects_authoritative_status) {
    const auto unknown = LezWallet::registrationStatusFor(
        PublicAccountRegistrationState::Unknown);
    LOGOS_ASSERT_FALSE(unknown.known);
    LOGOS_ASSERT_FALSE(unknown.needsInitialization);
    LOGOS_ASSERT_FALSE(unknown.initialized);

    const auto uninitialized = LezWallet::registrationStatusFor(
        PublicAccountRegistrationState::Uninitialized);
    LOGOS_ASSERT_TRUE(uninitialized.known);
    LOGOS_ASSERT_TRUE(uninitialized.needsInitialization);
    LOGOS_ASSERT_FALSE(uninitialized.initialized);

    const auto initialized = LezWallet::registrationStatusFor(
        PublicAccountRegistrationState::Initialized);
    LOGOS_ASSERT_TRUE(initialized.known);
    LOGOS_ASSERT_FALSE(initialized.needsInitialization);
    LOGOS_ASSERT_TRUE(initialized.initialized);
}

LOGOS_TEST(registration_status_retry_is_bounded_and_backed_off) {
    LOGOS_ASSERT_TRUE(LezWallet::shouldRetryRegistrationStatus(true, false, 0));
    LOGOS_ASSERT_TRUE(LezWallet::shouldRetryRegistrationStatus(false, true, 0));
    LOGOS_ASSERT_FALSE(LezWallet::shouldRetryRegistrationStatus(false, false, 0));
    LOGOS_ASSERT_FALSE(LezWallet::shouldRetryRegistrationStatus(
        true,
        false,
        LezWallet::RegistrationStatusRefreshMaxAttempts));
    LOGOS_ASSERT_TRUE(LezWallet::shouldRetryRegistrationStatus(
        false,
        true,
        LezWallet::RegistrationStatusRefreshMaxAttempts - 1));
    LOGOS_ASSERT_FALSE(LezWallet::shouldRetryRegistrationStatus(
        false,
        true,
        LezWallet::RegistrationStatusRefreshMaxAttempts));
    LOGOS_ASSERT_FALSE(LezWallet::shouldScheduleRegistrationStatusRefresh(
        false,
        true,
        0,
        true));
    LOGOS_ASSERT_TRUE(LezWallet::shouldScheduleRegistrationStatusRefresh(
        false,
        true,
        0,
        true,
        true));
    LOGOS_ASSERT_FALSE(LezWallet::shouldScheduleRegistrationStatusRefresh(
        false,
        true,
        LezWallet::RegistrationStatusRefreshMaxAttempts,
        true,
        true));

    LOGOS_ASSERT_EQ(LezWallet::registrationStatusRefreshDelayMs(0), 3000);
    LOGOS_ASSERT_EQ(LezWallet::registrationStatusRefreshDelayMs(1), 6000);
    LOGOS_ASSERT_EQ(LezWallet::registrationStatusRefreshDelayMs(3), 24000);
    LOGOS_ASSERT_EQ(LezWallet::registrationStatusRefreshDelayMs(4), 30000);
    LOGOS_ASSERT_EQ(LezWallet::registrationStatusRefreshDelayMs(100), 30000);
}

LOGOS_TEST(registration_retry_role_survives_model_refresh) {
    LEZWalletAccountModel model;
    const QString accountId(64, QLatin1Char('a'));
    const QVariantMap account{
        {QStringLiteral("account_id"), accountId},
        {QStringLiteral("is_public"), true},
    };

    model.replaceFromVariantList(QVariantList{account});
    const QModelIndex accountIndex = model.index(0, 0);
    LOGOS_ASSERT_FALSE(model.data(
        accountIndex,
        LEZWalletAccountModel::RegistrationRetryAllowedRole).toBool());

    model.setRegistrationRetryAllowed(accountId, true);
    LOGOS_ASSERT_TRUE(model.data(
        accountIndex,
        LEZWalletAccountModel::RegistrationRetryAllowedRole).toBool());

    model.replaceFromVariantList(QVariantList{account});
    LOGOS_ASSERT_TRUE(model.data(
        model.index(0, 0),
        LEZWalletAccountModel::RegistrationRetryAllowedRole).toBool());
}

LOGOS_TEST(enriched_public_account_status_overrides_cached_status) {
    LEZWalletAccountModel model;
    const QString accountId(64, QLatin1Char('b'));
    const QVariantMap uninitializedAccount{
        {QStringLiteral("account_id"), accountId},
        {QStringLiteral("is_public"), true},
        {QStringLiteral("registration_status_known"), true},
        {QStringLiteral("needs_registration"), true},
        {QStringLiteral("is_initialized"), false},
    };

    model.replaceFromVariantList(QVariantList{uninitializedAccount});
    const QModelIndex accountIndex = model.index(0, 0);
    LOGOS_ASSERT_TRUE(model.data(
        accountIndex,
        LEZWalletAccountModel::RegistrationStatusKnownRole).toBool());
    LOGOS_ASSERT_TRUE(model.data(
        accountIndex,
        LEZWalletAccountModel::NeedsRegistrationRole).toBool());
    LOGOS_ASSERT_FALSE(model.data(
        accountIndex,
        LEZWalletAccountModel::IsInitializedRole).toBool());

    const QVariantMap initializedAccount{
        {QStringLiteral("account_id"), accountId},
        {QStringLiteral("is_public"), true},
        {QStringLiteral("registration_status_known"), true},
        {QStringLiteral("needs_registration"), false},
        {QStringLiteral("is_initialized"), true},
    };
    model.replaceFromVariantList(QVariantList{initializedAccount});
    LOGOS_ASSERT_TRUE(model.data(
        model.index(0, 0),
        LEZWalletAccountModel::RegistrationStatusKnownRole).toBool());
    LOGOS_ASSERT_FALSE(model.data(
        model.index(0, 0),
        LEZWalletAccountModel::NeedsRegistrationRole).toBool());
    LOGOS_ASSERT_TRUE(model.data(
        model.index(0, 0),
        LEZWalletAccountModel::IsInitializedRole).toBool());
}
