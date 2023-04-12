add_executable(test_together test_together.c)
target_link_libraries(test_together lace)
add_test(test_together test_together)

add_executable(test_suspend test_suspend.c)
target_link_libraries(test_suspend lace)
add_test(test_suspend test_suspend)
