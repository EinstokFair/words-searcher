#include <QCoreApplication>
#include <QCryptographicHash>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

static QString hashFile(const QString &path, QCryptographicHash::Algorithm alg) {
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return QString();
    }
    QCryptographicHash h(alg);
    if (!h.addData(&f)) {
        return QString();
    }
    return QString(h.result().toHex());
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc < 4) {
        QTextStream(stderr) << "Usage: " << argv[0]
                            << " <tarball> <ebuild> <output-manifest>\n";
        return 1;
    }

    const QString tarball = argv[1];
    const QString ebuild = argv[2];
    const QString manifest = argv[3];

    QFileInfo tarballInfo(tarball);
    QFileInfo ebuildInfo(ebuild);

    if (!tarballInfo.exists()) {
        QTextStream(stderr) << "Error: tarball not found: " << tarball << "\n";
        return 1;
    }
    if (!ebuildInfo.exists()) {
        QTextStream(stderr) << "Error: ebuild not found: " << ebuild << "\n";
        return 1;
    }

    const QString distBlake2b  = hashFile(tarball, QCryptographicHash::Blake2b_512);
    const QString distSha512   = hashFile(tarball, QCryptographicHash::Sha512);
    const QString distSha256   = hashFile(tarball, QCryptographicHash::Sha256);

    const QString ebuildBlake2b = hashFile(ebuild, QCryptographicHash::Blake2b_512);
    const QString ebuildSha512  = hashFile(ebuild, QCryptographicHash::Sha512);
    const QString ebuildSha256  = hashFile(ebuild, QCryptographicHash::Sha256);

    QFile out(manifest);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream(stderr) << "Error: cannot write manifest: " << manifest << "\n";
        return 1;
    }

    QTextStream ts(&out);
    ts << "DIST " << tarballInfo.fileName() << " " << tarballInfo.size()
       << " BLAKE2B " << distBlake2b
       << " SHA512 " << distSha512
       << " SHA256 " << distSha256 << "\n";
    ts << "EBUILD " << ebuildInfo.fileName() << " " << ebuildInfo.size()
       << " BLAKE2B " << ebuildBlake2b
       << " SHA512 " << ebuildSha512
       << " SHA256 " << ebuildSha256 << "\n";
    out.close();

    QTextStream(stdout) << "Manifest written to " << manifest << "\n";
    return 0;
}
