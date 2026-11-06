#!/bin/bash
# Check install

die() {
    echo $@
    exit 1
}

install() {
    tmpdir="$(mktemp -d)"
    pushd "$tmpdir"

    git clone https://github.com/libcheck/check.git || die "Failed git clone"
    pushd check
    git checkout tags/0.15.2 || die "Failed git checkout"
    autoreconf --install || die "Failed autoreconf"
    ./configure --prefix="$HOME/.local" || die "Failed configure"
    make MAKEINFO=true || die "Failed compilation"
    make MAKEINFO=true install || die "Failed install"

    popd
    popd

    rm -rf "$tmpdir"
}

install
