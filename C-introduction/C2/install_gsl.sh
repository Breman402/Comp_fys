#!/bin/bash
# GSL install

die() {
    echo $@
    exit 1
}

install() {
    tmpdir="$(mktemp -d)"
    pushd "$tmpdir"

    wget ftp://ftp.gnu.org/gnu/gsl/gsl-2.7.tar.gz || die "Failed downloading archive"
    tar -xvf gsl-2.7.tar.gz || die "Failed extracting archive"
    pushd gsl-2.7/
    ./configure --prefix="$HOME/.local" || die "Failed configure"
    make || die "Failed compilation"
    make install || die "Failed install"

    popd
    popd

    rm -rf "$tmpdir"
}

install
