# Maintainer: Mark Carter <alt.mcarter@gmail.com>
pkgname=neoleo
pkgver=VERSION
pkgrel=1
pkgdesc="Lightweight curses/X11 spreadsheet based on GNU oleo"
arch=('i686' 'x86_64')
url="https://github.com/blippy/neoleo"
license=('GPL')
depends=('ncurses' 'readline')
source=(SOURCE)
md5sums=('MD5SUM')


build() {
	cd "$pkgname-$pkgver"
	./configure --prefix=/usr 
	make
}


package() {
	cd "$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install
}
