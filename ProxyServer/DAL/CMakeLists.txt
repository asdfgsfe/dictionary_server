aux_source_directory(. DAL_SRC_LIST)
add_library(dal ${DAL_SRC_LIST})
target_link_libraries(dal
                      hashkit
                      hiredis
                      muduo_base
                      muduo_net
                      pthread)
