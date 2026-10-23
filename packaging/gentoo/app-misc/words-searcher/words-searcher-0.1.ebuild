EAPI=8

inherit autotools

DESCRIPTION="Directory tree viewer with search planned"
HOMEPAGE="https://github.com/EinstokFair/words-searcher"
SRC_URI="https://github.com/EinstokFair/words-searcher/archive/v${PV}.tar.gz -> ${P}.tar.gz"

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
