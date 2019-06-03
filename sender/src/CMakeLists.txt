set(sender_SRCS
  main.cc
  )

add_executable(sender ${sender_SRCS})
target_link_libraries(sender sender_frame z m)

add_subdirectory(frame)
