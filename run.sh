#! /bin/sh
safeRunCommand() {
  typeset cmnd="$*"
  typeset ret_code

  eval $cmnd
  ret_code=$?
  if [ $ret_code != 0 ]; then
    printf "Error: [%d] when executing command: '$cmnd'" $ret_code
    exit $ret_code
  fi
}

safeRunCommand make run
./build/stack_emu_ "$@"
