// WARNING: Please don't edit this file. It was generated by C++/WinRT v2.0.240405.15

#pragma once
#ifndef WINRT_Windows_Devices_SmartCards_2_H
#define WINRT_Windows_Devices_SmartCards_2_H
#include "winrt/impl/Windows.Foundation.1.h"
#include "winrt/impl/Windows.Storage.Streams.1.h"
#include "winrt/impl/Windows.Devices.SmartCards.1.h"
WINRT_EXPORT namespace winrt::Windows::Devices::SmartCards
{
    struct SmartCardPinResetHandler : winrt::Windows::Foundation::IUnknown
    {
        SmartCardPinResetHandler(std::nullptr_t = nullptr) noexcept {}
        SmartCardPinResetHandler(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Foundation::IUnknown(ptr, take_ownership_from_abi) {}
        template <typename L> SmartCardPinResetHandler(L lambda);
        template <typename F> SmartCardPinResetHandler(F* function);
        template <typename O, typename M> SmartCardPinResetHandler(O* object, M method);
        template <typename O, typename M> SmartCardPinResetHandler(com_ptr<O>&& object, M method);
        template <typename O, typename LM> SmartCardPinResetHandler(weak_ref<O>&& object, LM&& lambda_or_method);
        template <typename O, typename M> SmartCardPinResetHandler(std::shared_ptr<O>&& object, M method);
        template <typename O, typename LM> SmartCardPinResetHandler(std::weak_ptr<O>&& object, LM&& lambda_or_method);
        auto operator()(winrt::Windows::Devices::SmartCards::SmartCardProvisioning const& sender, winrt::Windows::Devices::SmartCards::SmartCardPinResetRequest const& request) const;
    };
    struct WINRT_IMPL_EMPTY_BASES CardAddedEventArgs : winrt::Windows::Devices::SmartCards::ICardAddedEventArgs
    {
        CardAddedEventArgs(std::nullptr_t) noexcept {}
        CardAddedEventArgs(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ICardAddedEventArgs(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES CardRemovedEventArgs : winrt::Windows::Devices::SmartCards::ICardRemovedEventArgs
    {
        CardRemovedEventArgs(std::nullptr_t) noexcept {}
        CardRemovedEventArgs(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ICardRemovedEventArgs(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCard : winrt::Windows::Devices::SmartCards::ISmartCard,
        impl::require<SmartCard, winrt::Windows::Devices::SmartCards::ISmartCardConnect>
    {
        SmartCard(std::nullptr_t) noexcept {}
        SmartCard(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCard(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardChallengeContext : winrt::Windows::Devices::SmartCards::ISmartCardChallengeContext
    {
        SmartCardChallengeContext(std::nullptr_t) noexcept {}
        SmartCardChallengeContext(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardChallengeContext(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardConnection : winrt::Windows::Devices::SmartCards::ISmartCardConnection
    {
        SmartCardConnection(std::nullptr_t) noexcept {}
        SmartCardConnection(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardConnection(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardPinPolicy : winrt::Windows::Devices::SmartCards::ISmartCardPinPolicy
    {
        SmartCardPinPolicy(std::nullptr_t) noexcept {}
        SmartCardPinPolicy(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardPinPolicy(ptr, take_ownership_from_abi) {}
        SmartCardPinPolicy();
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardPinResetDeferral : winrt::Windows::Devices::SmartCards::ISmartCardPinResetDeferral
    {
        SmartCardPinResetDeferral(std::nullptr_t) noexcept {}
        SmartCardPinResetDeferral(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardPinResetDeferral(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardPinResetRequest : winrt::Windows::Devices::SmartCards::ISmartCardPinResetRequest
    {
        SmartCardPinResetRequest(std::nullptr_t) noexcept {}
        SmartCardPinResetRequest(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardPinResetRequest(ptr, take_ownership_from_abi) {}
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardProvisioning : winrt::Windows::Devices::SmartCards::ISmartCardProvisioning,
        impl::require<SmartCardProvisioning, winrt::Windows::Devices::SmartCards::ISmartCardProvisioning2>
    {
        SmartCardProvisioning(std::nullptr_t) noexcept {}
        SmartCardProvisioning(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardProvisioning(ptr, take_ownership_from_abi) {}
        static auto FromSmartCardAsync(winrt::Windows::Devices::SmartCards::SmartCard const& card);
        static auto RequestVirtualSmartCardCreationAsync(param::hstring const& friendlyName, winrt::Windows::Storage::Streams::IBuffer const& administrativeKey, winrt::Windows::Devices::SmartCards::SmartCardPinPolicy const& pinPolicy);
        static auto RequestVirtualSmartCardCreationAsync(param::hstring const& friendlyName, winrt::Windows::Storage::Streams::IBuffer const& administrativeKey, winrt::Windows::Devices::SmartCards::SmartCardPinPolicy const& pinPolicy, winrt::guid const& cardId);
        static auto RequestVirtualSmartCardDeletionAsync(winrt::Windows::Devices::SmartCards::SmartCard const& card);
        static auto RequestAttestedVirtualSmartCardCreationAsync(param::hstring const& friendlyName, winrt::Windows::Storage::Streams::IBuffer const& administrativeKey, winrt::Windows::Devices::SmartCards::SmartCardPinPolicy const& pinPolicy);
        static auto RequestAttestedVirtualSmartCardCreationAsync(param::hstring const& friendlyName, winrt::Windows::Storage::Streams::IBuffer const& administrativeKey, winrt::Windows::Devices::SmartCards::SmartCardPinPolicy const& pinPolicy, winrt::guid const& cardId);
    };
    struct WINRT_IMPL_EMPTY_BASES SmartCardReader : winrt::Windows::Devices::SmartCards::ISmartCardReader
    {
        SmartCardReader(std::nullptr_t) noexcept {}
        SmartCardReader(void* ptr, take_ownership_from_abi_t) noexcept : winrt::Windows::Devices::SmartCards::ISmartCardReader(ptr, take_ownership_from_abi) {}
        static auto GetDeviceSelector();
        static auto GetDeviceSelector(winrt::Windows::Devices::SmartCards::SmartCardReaderKind const& kind);
        static auto FromIdAsync(param::hstring const& deviceId);
    };
}
#endif
