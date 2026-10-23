#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QStringList>
#include <QTextStream>

static bool replaceInFile(const QString &path, const QRegularExpression &re, const QString &replacement) {
    QFile f(path);
    if (!f.exists()) {
        QTextStream(stderr) << "Warning: file not found " << path << "\n";
        return false;
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream(stderr) << "Error: cannot read " << path << "\n";
        return false;
    }
    QString content = QString::fromUtf8(f.readAll());
    f.close();

    if (!content.contains(re)) {
        QTextStream(stderr) << "Warning: no match for pattern in " << path << "\n";
        return false;
    }
    content.replace(re, replacement);

    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream(stderr) << "Error: cannot write " << path << "\n";
        return false;
    }
    QTextStream out(&f);
    out << content;
    f.close();
    return true;
}

static bool run(const QString &program, const QStringList &args, const QString &cwd = QString()) {
    QProcess p;
    if (!cwd.isEmpty()) p.setWorkingDirectory(cwd);
    p.start(program, args);
    p.waitForFinished(-1);
    if (p.exitCode() != 0) {
        QTextStream(stderr) << "Command failed: " << program << " " << args.join(" ") << "\n";
        QTextStream(stderr) << p.readAllStandardError() << "\n";
        return false;
    }
    QTextStream(stdout) << p.readAllStandardOutput();
    return true;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    args.removeFirst(); // program name

    bool doAutotools = false;
    QString version;
    QStringList documents;

    for (const QString &a : args) {
        if (a == "--autotools") {
            doAutotools = true;
        } else if (version.isEmpty()) {
            version = a;
        } else {
            documents << a;
        }
    }

    if (version.isEmpty()) {
        QTextStream(stderr) << "Usage: " << argv[0] << " <new-version> [--autotools] [<document1> ...]\n";
        return 1;
    }

    QRegularExpression versionRe(QStringLiteral("\\d+\\.\\d+\\.\\d+"));
    if (!versionRe.match(version).hasMatch()) {
        QTextStream(stderr) << "Error: version must be like X.Y.Z\n";
        return 1;
    }

    const QString root = QDir::currentPath();

    // Обновление Autotools-файлов (только с --autotools)
    if (doAutotools) {
        replaceInFile(root + "/configure.ac",
                      QRegularExpression(QStringLiteral("(AC_INIT\\(\\[words-searcher\\],\\s*\\[)[^\\]]+(\\])")),
                      "\\1" + version + "\\2");
        replaceInFile(root + "/README.en.md",
                      QRegularExpression(QStringLiteral("(Version\\s+)\\d+\\.\\d+\\.\\d+")),
                      "\\1" + version);
        replaceInFile(root + "/packaging/debian/changelog",
                      QRegularExpression(QStringLiteral("(words-searcher\\s*\\()[0-9.]+(\\-1\\))")),
                      "\\1" + version + "\\2");
        replaceInFile(root + "/packaging/gentoo/app-misc/words-searcher/words-searcher-0.1.ebuild",
                      QRegularExpression(QStringLiteral("(SRC_URI=\"words-searcher-)[0-9.]+(\\.tar\\.gz\")")),
                      "\\1" + version + "\\2");
    }

    // Обновление документов (всегда)
    QRegularExpression docRe(QStringLiteral("<!--VERSION\\(N\\.N\\.N\\)-->[0-9.]+"));
    for (const QString &doc : documents) {
        replaceInFile(root + "/" + doc, docRe, "<!--VERSION(N.N.N)-->" + version);
    }

    QTextStream(stdout) << "Version " << version << " set"
                        << (doAutotools ? " (autotools files)" : " (documents only)")
                        << ".\n";
    return 0;
}
