#!/bin/sh
set -eu

CC=${CC:-cc}
CFLAGS=${CFLAGS:-"-Wall -Wextra -Werror -Wstrict-prototypes"}
root=$(cd "$(dirname "$0")/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
status=0

check() {
	name=$1
	shift
	$CC $CFLAGS -I"$root" "$root/tests/$name.c" "$root/leak_hunter.c" -o "$tmp/$name"
	(cd "$tmp" && rm -f leak.log && "./$name")
	ok=1
	for expected in "$@"; do
		if ! grep -qF -- "$expected" "$tmp/leak.log"; then
			echo "FAIL $name: missing \"$expected\""
			ok=0
		fi
	done
	if [ $ok -eq 1 ]; then
		echo "ok   $name"
	else
		cat "$tmp/leak.log"
		status=1
	fi
}

check clean "Total leaks found:          0" "leaks free"
check leaks "Total leaks found:          2" "Total leaks size:          25"
check bad_free "Total leaks found:          1" "Bad frees found:            2"

$CC $CFLAGS -DLEAK_HUNTER_STDERR -I"$root" "$root/tests/leaks.c" "$root/leak_hunter.c" -o "$tmp/stderr"
(cd "$tmp" && rm -f leak.log && ./stderr 2> stderr.txt)
if [ ! -e "$tmp/leak.log" ] && grep -qF "Total leaks found:          2" "$tmp/stderr.txt"; then
	echo "ok   stderr"
else
	echo "FAIL stderr: report not on stderr or leak.log written"
	status=1
fi

exit $status
