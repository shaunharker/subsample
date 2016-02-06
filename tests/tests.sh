#!/bin/bash
SHELL_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd $SHELL_DIR
tar xvfz ./data.tar.gz
sed 's|REPLACEME|'${SHELL_DIR}'|g' data/sample.json > ./sample.json
sed 's|REPLACEME|'${SHELL_DIR}'|g' data/filter_10.0_inf.txt > ./filter_10.0_inf.txt

# ./trial.sh 1.0 1.0 0
# ./trial.sh 10.0 1.0 0
# ./trial.sh 1.0 1.0 0.01
# ./trial.sh 10.0 1.0 0.01
# ./trial.sh 1.0 1.0 0.1
# ./trial.sh 10.0 1.0 0.1

# ./trial.sh 1.0 2.0 0
# ./trial.sh 10.0 2.0 0
# ./trial.sh 1.0 2.0 0.01
# ./trial.sh 10.0 2.0 0.01
# ./trial.sh 1.0 2.0 0.1
# ./trial.sh 10.0 2.0 0.1

# ./trial.sh 1.0 "inf" 0
# ./trial.sh 10.0 "inf" 0
# ./trial.sh 1.0 "inf" 0.01
# ./trial.sh 10.0 "inf" 0.01
# ./trial.sh 1.0 "inf" 0.1
# ./trial.sh 10.0 "inf" 0.1

./trial.sh 10.0 "inf" 0 filter_10.0_inf.txt

