
find_package( Git REQUIRED )

if( EXISTS ${GIT_SOURCE_DIR}/.git )
  # extract working copy information for source into IBIS variables
  # Get the latest abbreviated commit hash of the working branch
  execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${GIT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${GIT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else( EXISTS ${GIT_SOURCE_DIR}/.git )
  set( GIT_COMMIT_HASH 0 )
  set( GIT_COMMIT_HASH_SHORT 0 )
endif( EXISTS ${GIT_SOURCE_DIR}/.git )

