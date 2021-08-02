#!/bin/sh -x

echo "ORACLE_LDFLAGS: ${ORACLE_LDFLAGS:?}"
echo "ORACLE_LIBS: ${ORACLE_LIBS:?}"

for f
do
    rpath_from="`otool -L $f | grep '@rpath' | cut -d '(' -f 1`"
    if [ -n "$rpath_from" ]
    then
        rpath_to_dir="`echo ${ORACLE_LDFLAGS:?} | cut -c 3-`"
        rpath_to_lib="`echo ${ORACLE_LIBS:?} | cut -c 3-`"
        install_name_tool -change $rpath_from ${rpath_to_dir}/lib${rpath_to_lib}.dylib $f
        otool -L $f
    fi
    rpath_from="`otool -L $f | grep '@rpath'`"
    if [ -n "${rpath_from// /}" ]
    then
        echo "@rpath still found in $f: $rpath_from" 1>&2
        exit 1
    fi
done
