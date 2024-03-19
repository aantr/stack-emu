#! /bin/sh
safeRunCommand() {
  cmnd="$*"

  eval $cmnd
  ret_code=$?
  if [ $ret_code != 0 ]; then
    printf "Error: [%d] when executing command: '$cmnd'" $ret_code
    exit $ret_code
  fi
}

safeRunCommand make test
./build/test_ "$@"
