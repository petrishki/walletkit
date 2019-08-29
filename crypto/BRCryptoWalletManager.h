//
//  BRCryptoWalletManager.h
//  BRCore
//
//  Created by Ed Gamble on 3/19/19.
//  Copyright © 2019 breadwallet. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.

#ifndef BRCryptoWalletManager_h
#define BRCryptoWalletManager_h

#include "BRCryptoBase.h"
#include "BRCryptoKey.h"
#include "BRCryptoNetwork.h"
#include "BRCryptoAccount.h"
#include "BRCryptoTransfer.h"
#include "BRCryptoWallet.h"
#include "BRCryptoWalletManagerClient.h"

#include "ethereum/BREthereum.h"
#include "bitcoin/BRWalletManager.h"

#ifdef __cplusplus
extern "C" {
#endif

    /// MARK: Wallet Manager Event

    typedef enum {
        CRYPTO_WALLET_MANAGER_STATE_CREATED,
        CRYPTO_WALLET_MANAGER_STATE_DISCONNECTED,
        CRYPTO_WALLET_MANAGER_STATE_CONNECTED,
        CRYPTO_WALLET_MANAGER_STATE_SYNCING,
        CRYPTO_WALLET_MANAGER_STATE_DELETED
    } BRCryptoWalletManagerState;

    typedef enum {
        CRYPTO_WALLET_MANAGER_EVENT_CREATED,
        CRYPTO_WALLET_MANAGER_EVENT_CHANGED,
        CRYPTO_WALLET_MANAGER_EVENT_DELETED,

        CRYPTO_WALLET_MANAGER_EVENT_WALLET_ADDED,
        CRYPTO_WALLET_MANAGER_EVENT_WALLET_CHANGED,
        CRYPTO_WALLET_MANAGER_EVENT_WALLET_DELETED,

        // wallet: added, ...
        CRYPTO_WALLET_MANAGER_EVENT_SYNC_STARTED,
        CRYPTO_WALLET_MANAGER_EVENT_SYNC_CONTINUES,
        CRYPTO_WALLET_MANAGER_EVENT_SYNC_STOPPED,

        CRYPTO_WALLET_MANAGER_EVENT_BLOCK_HEIGHT_UPDATED,
    } BRCryptoWalletManagerEventType;

    extern const char *
    BRCryptoWalletManagerEventTypeString (BRCryptoWalletManagerEventType t);

    typedef struct {
        BRCryptoWalletManagerEventType type;
        union {
            struct {
                BRCryptoWalletManagerState oldValue;
                BRCryptoWalletManagerState newValue;
            } state;

            struct {
                /// Handler must 'give'
                BRCryptoWallet value;
            } wallet;

            struct {
                BRSyncTimestamp timestamp;
                BRSyncPercentComplete percentComplete;
            } sync;

            struct {
                uint64_t value;
            } blockHeight;
        } u;
    } BRCryptoWalletManagerEvent;

#define CRYPTO_NO_SYNC_TIMESTAMP        (NO_SYNC_TIMESTAMP)

    /// MARK: Listener

    typedef void *BRCryptoCWMListenerContext;

    /// Handler must 'give': manager, event.wallet.value
    typedef void (*BRCryptoCWMListenerWalletManagerEvent) (BRCryptoCWMListenerContext context,
                                                           BRCryptoWalletManager manager,
                                                           BRCryptoWalletManagerEvent event);

    /// Handler must 'give': manager, wallet, event.*
    typedef void (*BRCryptoCWMListenerWalletEvent) (BRCryptoCWMListenerContext context,
                                                    BRCryptoWalletManager manager,
                                                    BRCryptoWallet wallet,
                                                    BRCryptoWalletEvent event);

    /// Handler must 'give': manager, wallet, transfer
    typedef void (*BRCryptoCWMListenerTransferEvent) (BRCryptoCWMListenerContext context,
                                                      BRCryptoWalletManager manager,
                                                      BRCryptoWallet wallet,
                                                      BRCryptoTransfer transfer,
                                                      BRCryptoTransferEvent event);

    typedef struct {
        BRCryptoCWMListenerContext context;
        BRCryptoCWMListenerWalletManagerEvent walletManagerEventCallback;
        BRCryptoCWMListenerWalletEvent walletEventCallback;
        BRCryptoCWMListenerTransferEvent transferEventCallback;
    } BRCryptoCWMListener;

    extern BRCryptoWalletManager
    cryptoWalletManagerCreate (BRCryptoCWMListener listener,
                               BRCryptoCWMClient client,
                               BRCryptoAccount account,
                               BRCryptoNetwork network,
                               BRSyncMode mode,
                               BRCryptoAddressScheme scheme,
                               const char *path);

    extern BRCryptoNetwork
    cryptoWalletManagerGetNetwork (BRCryptoWalletManager cwm);

    extern BRCryptoAccount
    cryptoWalletManagerGetAccount (BRCryptoWalletManager cwm);

    extern BRSyncMode
    cryptoWalletManagerGetMode (BRCryptoWalletManager cwm);

    extern void
    cryptoWalletManagerSetMode (BRCryptoWalletManager cwm, BRSyncMode mode);

    extern BRCryptoWalletManagerState
    cryptoWalletManagerGetState (BRCryptoWalletManager cwm);

    extern BRCryptoAddressScheme
    cryptoWalletManagerGetAddressScheme (BRCryptoWalletManager cwm);

    extern void
    cryptoWalletManagerSetAddressScheme (BRCryptoWalletManager cwm,
                                         BRCryptoAddressScheme scheme);

    extern const char *
    cryptoWalletManagerGetPath (BRCryptoWalletManager cwm);

    extern BRCryptoBoolean
    cryptoWalletManagerHasWallet (BRCryptoWalletManager cwm,
                                  BRCryptoWallet wallet);

    extern BRCryptoWallet
    cryptoWalletManagerGetWallet (BRCryptoWalletManager cwm);

    /**
     * Returns a newly allocated array of the managers's wallets.
     *
     * The caller is responsible for deallocating the returned array using
     * free().
     *
     * @param cwm the wallet manager
     * @param count the number of wallets returned
     *
     * @return An array of wallets w/ an incremented reference count (aka 'taken')
     *         or NULL if there are no wallters in the manager.
     */
    extern BRCryptoWallet *
    cryptoWalletManagerGetWallets (BRCryptoWalletManager cwm,
                                   size_t *count);

    extern BRCryptoWallet
    cryptoWalletManagerGetWalletForCurrency (BRCryptoWalletManager cwm,
                                             BRCryptoCurrency currency);

    extern void
    cryptoWalletManagerConnect (BRCryptoWalletManager cwm);

    extern void
    cryptoWalletManagerDisconnect (BRCryptoWalletManager cwm);

    extern void
    cryptoWalletManagerSync (BRCryptoWalletManager cwm);

    extern BRCryptoBoolean
    cryptoWalletManagerSign (BRCryptoWalletManager cwm,
                             BRCryptoWallet wallet,
                             BRCryptoTransfer transfer,
                             const char *paperKey);

    extern void
    cryptoWalletManagerSubmit (BRCryptoWalletManager cwm,
                               BRCryptoWallet wid,
                               BRCryptoTransfer tid,
                               const char *paperKey);

    extern void
    cryptoWalletManagerSubmitForKey (BRCryptoWalletManager cwm,
                                     BRCryptoWallet wallet,
                                     BRCryptoTransfer transfer,
                                     BRCryptoKey key);

    extern void
    cryptoWalletManagerSubmitSigned (BRCryptoWalletManager cwm,
                                     BRCryptoWallet wallet,
                                     BRCryptoTransfer transfer);

    DECLARE_CRYPTO_GIVE_TAKE (BRCryptoWalletManager, cryptoWalletManager);

    /// MARK: Wallet Migrator
    
    typedef struct BRCryptoWalletMigratorRecord *BRCryptoWalletMigrator;

    typedef enum {
        CRYPTO_WALLET_MIGRATOR_SUCCESS,
        CRYPTO_WALLET_MIGRATOR_ERROR_TRANSACTION,
        CRYPTO_WALLET_MIGRATOR_ERROR_BLOCK,
        CRYPTO_WALLET_MIGRATOR_ERROR_PEER
    } BRCryptoWalletMigratorStatusType;

    typedef struct {
        BRCryptoWalletMigratorStatusType type;
        // union {} u;
    } BRCryptoWalletMigratorStatus;

    extern BRCryptoWalletMigrator // NULL on error
    cryptoWalletMigratorCreate (BRCryptoNetwork network,
                                const char *storagePath);

    extern void
    cryptoWalletMigratorRelease (BRCryptoWalletMigrator migrator);

    extern BRCryptoWalletMigratorStatus
    cryptoWalletMigratorHandleTransactionAsBTC (BRCryptoWalletMigrator migrator,
                                                const uint8_t *bytes,
                                                size_t bytesCount,
                                                uint32_t blockHeight,
                                                uint32_t timestamp);

    extern BRCryptoWalletMigratorStatus
    cryptoWalletMigratorHandleBlockAsBTC (BRCryptoWalletMigrator migrator,
                                          UInt256 hash,
                                          uint32_t height,
                                          uint32_t nonce,
                                          uint32_t target,
                                          uint32_t txCount,
                                          uint32_t version,
                                          uint32_t timestamp,
                                          uint8_t *flags,  size_t flagsLen,
                                          UInt256 *hashes, size_t hashesCount,
                                          UInt256 merkleRoot,
                                          UInt256 prevBlock);

    extern BRCryptoWalletMigratorStatus
    cryptoWalletMigratorHandlePeerAsBTC (BRCryptoWalletMigrator migrator,
                                         uint32_t address,
                                         uint16_t port,
                                         uint64_t services,
                                         uint32_t timestamp);

#ifdef __cplusplus
}
#endif

#endif /* BRCryptoWalletManager_h */
