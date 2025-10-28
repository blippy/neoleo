cmake_minimum_required(VERSION 3.23)

set(SANITIZE_CONDITION "$<CONFIG:Sanitize>")
set(SANITIZE_CONDITION 1)

if (MSVC)
  set(
    SANITIZE_FLAGS 
    "$<${SANITIZE_CONDITION}:/fsanitize=address>"
  )
  add_compile_options("$<${SANITIZE_CONDITION}:/Zi>")
else()
  set(SANITIZE_FLAGS
    "$<${SANITIZE_CONDITION}:-fsanitize=address>"
    "$<${SANITIZE_CONDITION}:-fno-omit-frame-pointer>"
    "$<${SANITIZE_CONDITION}:-g>"
  )
endif()

add_compile_options(${SANITIZE_FLAGS})
add_link_options(${SANITIZE_FLAGS})

#add_compile_options(-fsanitize=address)
#target_link_options(neoleo -fsanitize=address)

