cmake_minimum_required(VERSION 3.14)
project(LEZWalletUiTests LANGUAGES CXX)

include(LogosTest)

logos_test(
    NAME lez_wallet_ui_registration_status_tests
    MODULE_SOURCES
        ../src/LEZWalletAccountModel.cpp
    TEST_SOURCES
        main.cpp
        test_PublicAccountRegistrationStatus.cpp
    EXTRA_INCLUDES
        ../src
)
