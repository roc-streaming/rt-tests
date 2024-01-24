# A custom function to create an option for the user which 
# can be set via environment or provided via cmdline.
# It can be boolean or string
# Order of precedence
# 1. Command line
# 2. Environment variable
# 3. Default specified in cmake

macro(define_option ARG_NAME ARG_DEFAULT ARG_TYPE ARG_HELP)
  # overwrite default from environment
  set(DEFAULT_VALUE $ENV{${ARG_NAME}})
  if(NOT DEFAULT_VALUE)
    set(DEFAULT_VALUE "${ARG_DEFAULT}")
  endif()

  # register option
  if(${ARG_TYPE} STREQUAL "BOOL")
    option(${ARG_NAME} "${ARG_HELP}" "${DEFAULT_VALUE}")
  elseif(${ARG_TYPE} STREQUAL "STRING")
    set(${ARG_NAME} "${DEFAULT_VALUE}" CACHE STRING "${ARG_HELP}")
  endif()
endmacro()
