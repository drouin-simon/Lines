# creates githash.h using cmake script write a file with the LINES_GIT_HASH define
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/githash.h.txt "#define LINES_GIT_HASH \"${GIT_COMMIT_HASH}\"\n#define LINES_GIT_HASH_SHORT \"${GIT_COMMIT_HASH_SHORT}\"\n")

# copy the file to the final header only if the version changes reduces needless rebuilds
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different githash.h.txt githash.h)

