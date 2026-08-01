#include "LEZWalletAccountModel.h"
#include <QHash>
#include <QPair>
#include <QVariantMap>
#include <algorithm>

LEZWalletAccountModel::LEZWalletAccountModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int LEZWalletAccountModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();
}

QVariant LEZWalletAccountModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size())
        return QVariant();

    const LEZWalletAccountEntry& e = m_entries.at(index.row());
    switch (role) {
    case NameRole:    return e.name;
    case AccountIdRole:  return e.accountId;
    case BalanceRole: return e.balance;
    case VaultBalanceRole: return e.vaultBalance;
    case IsPublicRole: return e.isPublic;
    case RegistrationStatusKnownRole: return e.registrationStatusKnown;
    case NeedsRegistrationRole: return e.needsRegistration;
    case SectionKeyRole: return e.sectionKey;
    case KeysJsonRole: return e.keysJson;
    case IsFirstInGroupRole: return e.isFirstInGroup;
    case IsInitializedRole: return e.isInitialized;
    default:          return QVariant();
    }
}

QHash<int, QByteArray> LEZWalletAccountModel::roleNames() const
{
    return {
        { NameRole,    "name"    },
        { AccountIdRole, "accountId" },
        { BalanceRole, "balance" },
        { VaultBalanceRole, "vaultBalance" },
        { IsPublicRole, "isPublic" },
        { RegistrationStatusKnownRole, "registrationStatusKnown" },
        { NeedsRegistrationRole, "needsRegistration" },
        { SectionKeyRole, "sectionKey" },
        { KeysJsonRole, "keysJson" },
        { IsFirstInGroupRole, "isFirstInGroup" },
        { IsInitializedRole, "isInitialized" }
    };
}

void LEZWalletAccountModel::replaceFromVariantList(const QVariantList& list)
{
    // Rebuilding from scratch loses any balance/vaultBalance already fetched for an
    // account that's still present — carry those over so periodic re-listing (e.g. to
    // pick up newly discovered private accounts) doesn't make the claimable list and
    // balances flicker empty until the next refresh repopulates them.
    QHash<QString, LEZWalletAccountEntry> previousEntries;
    previousEntries.reserve(m_entries.size());
    for (const LEZWalletAccountEntry& e : m_entries)
        previousEntries.insert(e.accountId, e);

    beginResetModel();
    int oldCount = m_entries.size();
    m_entries.clear();
    for (const QVariant& v : list) {
        LEZWalletAccountEntry e;
        e.balance = QString();
        e.name = QString();
        if (v.type() == QVariant::Map) {
            const QVariantMap map = v.toMap();
            e.accountId = map.value(QStringLiteral("account_id")).toString();
            e.isPublic = map.value(QStringLiteral("is_public"), true).toBool();
            e.isInitialized = map.value(QStringLiteral("is_initialized"), false).toBool();
            if (e.isPublic) {
                e.sectionKey = PublicSectionKey;
            } else {
                e.sectionKey = map.value(QStringLiteral("npk")).toString();
                e.keysJson = map.value(QStringLiteral("keys_json")).toString();
            }
        } else {
            e.accountId = v.toString();
            e.isPublic = true;
            e.sectionKey = PublicSectionKey;
        }
        const auto previous = previousEntries.find(e.accountId);
        if (previous != previousEntries.end()) {
            e.balance = previous->balance;
            e.vaultBalance = previous->vaultBalance;
            e.registrationStatusKnown = previous->registrationStatusKnown;
            e.needsRegistration = previous->needsRegistration;
        }
        m_entries.append(e);
    }
    // Keep entries grouped by section (public first) so consecutive rows of the same
    // group are contiguous, then mark each group's first row for the QML header.
    std::stable_sort(m_entries.begin(), m_entries.end(),
        [](const LEZWalletAccountEntry& a, const LEZWalletAccountEntry& b) {
            if (a.isPublic != b.isPublic) return a.isPublic;
            return a.sectionKey < b.sectionKey;
        });
    for (int i = 0; i < m_entries.size(); ++i)
        m_entries[i].isFirstInGroup = (i == 0) || (m_entries[i].sectionKey != m_entries[i - 1].sectionKey);
    endResetModel();
    if (oldCount != m_entries.size())
        emit countChanged();
}

void LEZWalletAccountModel::setBalanceByAccountId(const QString& accountId, const QString& balance)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries.at(i).accountId == accountId) {
            if (m_entries.at(i).balance != balance) {
                m_entries[i].balance = balance;
                QModelIndex idx = index(i, 0);
                emit dataChanged(idx, idx, { BalanceRole });
            }
            return;
        }
    }
}

void LEZWalletAccountModel::setVaultBalanceByAccountId(const QString& accountId, const QString& vaultBalance)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries.at(i).accountId == accountId) {
            if (m_entries.at(i).vaultBalance != vaultBalance) {
                m_entries[i].vaultBalance = vaultBalance;
                QModelIndex idx = index(i, 0);
                emit dataChanged(idx, idx, { VaultBalanceRole });
            }
            return;
        }
    }
}

void LEZWalletAccountModel::setPublicAccountRegistrationStatus(
    const QString& accountId,
    const bool known,
    const bool needsRegistration)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        LEZWalletAccountEntry& entry = m_entries[i];
        if (entry.accountId != accountId || !entry.isPublic)
            continue;
        if (entry.registrationStatusKnown == known
            && entry.needsRegistration == needsRegistration) {
            return;
        }
        entry.registrationStatusKnown = known;
        entry.needsRegistration = needsRegistration;
        const QModelIndex idx = index(i, 0);
        emit dataChanged(idx, idx, { RegistrationStatusKnownRole, NeedsRegistrationRole });
        return;
    }
}

void LEZWalletAccountModel::setInitializedByAccountId(const QString& accountId, bool isInitialized)
{
    for (int i = 0; i < m_entries.size(); ++i) {
        if (m_entries.at(i).accountId == accountId) {
            if (m_entries.at(i).isInitialized != isInitialized) {
                m_entries[i].isInitialized = isInitialized;
                QModelIndex idx = index(i, 0);
                emit dataChanged(idx, idx, { IsInitializedRole });
            }
            return;
        }
    }
}

bool LEZWalletAccountModel::isPublicAccount(const QString& accountId, bool defaultValue) const
{
    for (const LEZWalletAccountEntry& e : m_entries) {
        if (e.accountId == accountId)
            return e.isPublic;
    }
    return defaultValue;
}
