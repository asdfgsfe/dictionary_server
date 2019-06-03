set(frame_SRCS
  ClientConfig.cc
  DicClient.cc
  )

add_library(sender_frame ${frame_SRCS})
target_link_libraries(sender_frame muduo_net)
