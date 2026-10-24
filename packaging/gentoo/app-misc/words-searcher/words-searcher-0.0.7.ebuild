EAPI=8

inherit autotools qt-utils

DESCRIPTION="Directory tree viewer with search planned"
HOMEPAGE="https://github.com/EinstokFair/words-searcher"
SRC_URI="words-searcher-0.0.7.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="~amd64"
IUSE=""

DEPEND="
	dev-qt/qtbase:6[gui,widgets]
"
RDEPEND="${DEPEND}"


src_configure() {
	econf \
		MOC="${EPREFIX}$(qt_get_libexecdir 6)/moc" \
		RCC="${EPREFIX}$(qt_get_libexecdir 6)/rcc" \
		LRELEASE="${EPREFIX}$(qt_get_bindir 6)/lrelease" \
		LUPDATE="${EPREFIX}$(qt_get_bindir 6)/lupdate"
}

src_prepare() {
	default
	eautoreconf
}
