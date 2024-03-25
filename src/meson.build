nomar_sources = files(
  'aa.c',
  'aa.h',
  'aa_file.c',
  'aa_file.h',
  'aa_k_mer.c',
  'aa_k_mer.h',
  'cmp_k_mer.c',
  'cmp_k_mer.h',
  'count_k_mer.c',
  'count_k_mer.h',
  'count_table.c',
  'count_table.h',
  'gz.c',
  'gz.h',
  'h5.c',
  'h5.h',
  'k_mer.c',
  'k_mer.h',
  'log.c',
  'log.h',
  'strv.c',
  'strv.h',
  'test.c',
  'test.h',
  'train.c',
  'train.h',
  'utils.c',
  'utils.h',
  'weight.c',
  'weight.h',
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
