# Maintainer: Mark Carter <alt.mcarter@gmail.com>
pkgname=neoleo
pkgver=VERSION
pkgrel=1
pkgdesc="Lightweight curses/Motif spreadsheet based on GNU oleo"
arch=('i686' 'x86_64')
url="https://github.com/blippy/neoleo"
license=('GPL')
DEPENDS
SOURCE
md5sums=('MD5SUM')


build() {
	cd "$pkgname-$pkgver"
	./configure --prefix=/usr CONFX
	make
}


package() {
	cd "$pkgname-$pkgver"
	make DESTDIR="$pkgdir/" install
}
