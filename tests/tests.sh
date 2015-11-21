#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
tar xvfz ./data.tar.gz
sed 's|REPLACEME|'${SHELL_DIR}'|g' data/sample.json > ./sample.json
./trial.sh 1.0 1.0 0.05
./trial.sh 100.0 1.0 0.05
./trial.sh 1.0 inf 0.05
./trial.sh 10.0 inf 0.05
