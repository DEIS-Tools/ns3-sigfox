set(base_examples
    large-scale-network-example
    sigfox-energy-model-example2
    sigfox-energy-model-example
)

foreach(
  example
  ${base_examples}
)
  build_lib_example(
    NAME ${example}
    SOURCE_FILES ${example}.cc
    LIBRARIES_TO_LINK
      ${libsigfox}
  )
endforeach()


