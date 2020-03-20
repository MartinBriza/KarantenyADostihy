#!/bin/bash

PROBABLY_TAG=${GITHUB_REF/refs\/tags\//}

MAYBE_TAG=$(grep "^v[0-9][0-9]*[.][0-9][0-9]*[.][0-9][0-9]*$" <<< "$PROBABLY_TAG")
if [ "$MAYBE_TAG" == "" ]; then
    echo "v0.0.0"
else
    echo $MAYBE_TAG
fi

exit 0
