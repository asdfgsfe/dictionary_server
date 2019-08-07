aux_source_directory(. DAL_SRC_LIST)
add_library(query ${DAL_SRC_LIST})
target_link_libraries(query
                      muduo_base
                      muduo_net
                      pthread)
