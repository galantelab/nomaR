nomar_sources = files(
  'aa.c',
  'aa.h'
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
