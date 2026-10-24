EAPI=8

inherit autotools vcs-snapshot

DESCRIPTION="Directory tree viewer with search planned"
HOMEPAGE="https://github.com/EinstokFair/words-searcher"
SRC_URI="words-searcher-0.0.7.tar.gz -> words-searcher-0.0.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="~amd64"
IUSE=""

DEPEND="
	dev-qt/qtbase:6[gui,widgets]
"
RDEPEND="${DEPEND}"

src_prepare() {
	default
	eautoreconf
}
