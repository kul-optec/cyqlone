#!/usr/bin/env bash

set -euxo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")/../.."

git subtree pull --prefix scripts/ci/conan-recipes https://github.com/tttapa/conan-recipes main --squash
git subtree pull --prefix scripts/ci/conan-profiles https://github.com/tttapa/conan-profiles main --squash
