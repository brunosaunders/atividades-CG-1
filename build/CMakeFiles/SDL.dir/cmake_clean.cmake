file(REMOVE_RECURSE
  "libSDL.a"
  "libSDL.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/SDL.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
