nomar_sources = files(
  'aa.c',
  'aa.h',
  'aa_k_mer.c',
  'aa_k_mer.h',
  'count_k_mer.c',
  'count_k_mer.h',
  'gz.c',
  'gz.h',
  'k_mer.c',
  'k_mer.h',
  'log.c',
  'log.h',
  'utils.c',
  'utils.h',
  'wrapper.c',
  'wrapper.h'
)

# Static library necessary for
# executable and testing
nomar_lib = static_library(
  package,
  nomar_sources,
  include_directories : inc,
         dependencies : deps,
              install : false
)

# Our product!
executable(
  package,
  files('main.c'),
  include_directories : inc,
         dependencies : config_dep,
            link_with : nomar_lib,
              install : true
)
