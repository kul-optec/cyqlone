#!/usr/bin/env bash

set -euxo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

git subtree pull --prefix scripts/ci/conan-recipes https://github.com/tttapa/conan-recipes main --squash -m"Update conan-recipes"
git subtree pull --prefix scripts/ci/conan-profiles https://github.com/tttapa/conan-profiles main --squash -m"Update conan-profiles"

echo ""
echo "Subtrees updated successfully. If there were any changes to conan-profiles, "
echo "you will need to update the Conan lockfile in \"benchmarks/cyqpalm\"."
echo ""
