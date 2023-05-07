
#ifndef GENKEY_H
#define GENKEY_H

#include <QString>

enum class LicenseType { Professional = 1, Educational = 3, Personal = 4 };

QByteArray GenerateLicense(
    LicenseType type, int count, const QString &userName, int majorVersion, int minorVersion);
#endif // GENKEY_H
