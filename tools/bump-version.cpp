#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>

static bool replaceInFile(const QString &path, const QRegularExpression &re, const QString &replacement) {
    QFile f(path);
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
    if (argc < 2) {
        QTextStream(stderr) << "Usage: " << argv[0] << " <new-version>\n";
        return 1;
    }
    const QString newVersion = argv[1];

    QRegularExpression versionRe(QStringLiteral("\\d+\\.\\d+\\.\\d+"));
    if (!versionRe.match(newVersion).hasMatch()) {
        QTextStream(stderr) << "Error: version must be like X.Y.Z\n";
        return 1;
    }

    const QString root = QDir::currentPath();

    // 1. configure.ac: AC_INIT([words-searcher], [0.0.1], [])
    {
        QRegularExpression re(QStringLiteral("(AC_INIT\\(\\[words-searcher\\],\\s*\\[)[^\\]]+(\\])"));
        replaceInFile(root + "/configure.ac", re, "\\1" + newVersion + "\\2");
    }

    // 2. README.en.md: Version 0.0.1
    {
        QRegularExpression re(QStringLiteral("(Version\\s+)\\d+\\.\\d+\\.\\d+"));
        replaceInFile(root + "/README.en.md", re, "\\1" + newVersion);
    }

    // 3. packaging/debian/changelog: words-searcher (0.0.1-1)
    {
        QRegularExpression re(QStringLiteral("(words-searcher\\s*\\()[0-9.]+(\\-1\\))"));
        replaceInFile(root + "/packaging/debian/changelog", re, "\\1" + newVersion + "\\2");
    }

    // 4. packaging/gentoo/.../words-searcher-0.1.ebuild: SRC_URI
    {
        const QString ebuildPath = root + "/packaging/gentoo/app-misc/words-searcher/words-searcher-0.1.ebuild";
        QRegularExpression re(QStringLiteral("(SRC_URI=\"words-searcher-)[0-9.]+(\\.tar\\.gz\")"));
        replaceInFile(ebuildPath, re, "\\1" + newVersion + "\\2");
        // Переименовать ebuild? (words-searcher-0.1.ebuild -> words-searcher-<new>.ebuild) — оставляем 0.1 как слот.
    }

    // 5. doc/REQUIREMENTS.ru.html: строка версии (обычно 0.0.1)
    {
        QRegularExpression re(QStringLiteral("^0\\.0\\.1$"), QRegularExpression::MultilineOption);
        replaceInFile(root + "/doc/REQUIREMENTS.ru.html", re, newVersion);
    }

    // 6. Запустить make dist
    if (!run("make", {"dist"}, root)) return 1;

    // 7. Пересобрать Manifest (tools/manifest-signer)
    if (!run(root + "/tools/manifest-signer",
             {root + "/words-searcher-" + newVersion + ".tar.gz",
              root + "/packaging/gentoo/app-misc/words-searcher/words-searcher-0.1.ebuild",
              root + "/packaging/gentoo/app-misc/words-searcher/Manifest"},
             root)) return 1;

    // 8. git commit и git tag
    if (!run("git", {"add", "-A"}, root)) return 1;
    if (!run("git", {"commit", "-m", "release: " + newVersion}, root)) {
        QTextStream(stdout) << "Nothing to commit, continuing to tag...\n";
    }
    if (!run("git", {"tag", newVersion}, root)) return 1;

    QTextStream(stdout) << "Version bumped to " << newVersion << ", tarball built, committed, tagged.\n";
    return 0;
}
