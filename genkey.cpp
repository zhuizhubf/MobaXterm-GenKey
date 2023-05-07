
#include "genkey.h"

#include <QtCore/QByteArray>
#include <QtCore/QDebug>

#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtGui/private/qzipwriter_p.h>
//#include <QtCore/QZipWriter>

const QByteArray VariantBase64Table
    = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
const QHash<char, int> VariantBase64ReverseDict = {
    {'A', 0},  {'B', 1},  {'C', 2},  {'D', 3},  {'E', 4},  {'F', 5},  {'G', 6},  {'H', 7},
    {'I', 8},  {'J', 9},  {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15},
    {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23},
    {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31},
    {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39},
    {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47},
    {'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55},
    {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63},
};

QByteArray VariantBase64Encode(const QByteArray &bs)
{
    QByteArray result;
    int blocks_count = bs.size() / 3;
    int left_bytes = bs.size() % 3;

    for (int i = 0; i < blocks_count; ++i) {
        int coding_int = (static_cast<unsigned char>(bs[3 * i + 2]) << 16)
                         | (static_cast<unsigned char>(bs[3 * i + 1]) << 8)
                         | (static_cast<unsigned char>(bs[3 * i + 0]));
        char block[5] = {VariantBase64Table[coding_int & 0x3F],
                         VariantBase64Table[(coding_int >> 6) & 0x3F],
                         VariantBase64Table[(coding_int >> 12) & 0x3F],
                         VariantBase64Table[(coding_int >> 18) & 0x3F],
                         '\0'};
        result.append(block);
    }

    if (left_bytes == 0) {
        return result;
    } else if (left_bytes == 1) {
        int coding_int = static_cast<unsigned char>(bs[3 * blocks_count]);
        char block[3] = {VariantBase64Table[coding_int & 0x3F],
                         VariantBase64Table[(coding_int >> 6) & 0x3F],
                         '\0'};
        result.append(block);
        return result;
    } else { // 2 bit
        int coding_int = (static_cast<unsigned char>(bs[3 * blocks_count + 1]) << 8)
                         | (static_cast<unsigned char>(bs[3 * blocks_count]) << 0);
        char block[4] = {VariantBase64Table[coding_int & 0x3F],
                         VariantBase64Table[(coding_int >> 6) & 0x3F],
                         VariantBase64Table[(coding_int >> 12) & 0x3F],
                         '\0'};
        result.append(block);
        return result;
    }
}

QByteArray EncryptBytes(int key, const QByteArray &bs)
{
    QByteArray result;
    for (int i = 0; i < bs.size(); ++i) {
        char byte = bs[i] ^ ((key >> 8) & 0xFF);
        key = (byte & key) | 0x482D;
        result.append(byte);
    }
    return result;
}

QByteArray DecryptBytes(int key, const QByteArray &bs)
{
    QByteArray result;
    for (int i = 0; i < bs.size(); ++i) {
        char byte = bs[i] ^ ((key >> 8) & 0xFF);
        key = (bs[i] & key) | 0x482D;
        result.append(byte);
    }
    return result;
}

QByteArray GenerateLicense(
    LicenseType type, int count, const QString &userName, int majorVersion, int minorVersion)
{
    //
    QString licenseString = QString("%1#%2|%3%4#%5# %6 3 %7 6%8 #%9#%10#%11#")
                                .arg(static_cast<int>(type)) //1
                                .arg(userName)               //2
                                .arg(majorVersion)           //3
                                .arg(minorVersion)           //4
                                .arg(count)                  //5
                                .arg(majorVersion)           //6
                                .arg(minorVersion)           //7
                                .arg(minorVersion)           //8
                                .arg(0)                      //9
                                .arg(0)                      //10
                                .arg(0);                     //11

    licenseString = licenseString.remove(' ');
    const QByteArray encryptedLicenseString = EncryptBytes(0x787, licenseString.toUtf8());
    const QByteArray encodedLicenseString = VariantBase64Encode(encryptedLicenseString);

    QFile file("Custom.mxtpro");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to create key file"
                   << "Custom.mxtpro";
        return QByteArray();
    }

    QZipWriter zipWriter(&file);
    zipWriter.setCompressionPolicy(QZipWriter::AlwaysCompress);
    zipWriter.addFile("Pro.key", encodedLicenseString);
    zipWriter.close();

    return encodedLicenseString;
}
