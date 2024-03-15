test_sources = files(
  'check_nomar.h',
  'check_nomar_aa.c',
  'check_nomar_example.c',
  'check_nomar_gz.c',
  'check_nomar_k_mer.c',
  'check_nomar_main.c',
  'check_nomar_utils.c',
  'check_nomar_wrapper.c'
)

# Package executable tester
check = '_'.join(['check', package])

# Test only if libcheck is found
if check_dep.found()
  # The executable tester for the units
  e = executable(
    check,
    test_sources,
    include_directories : inc,
           dependencies : [deps, check_dep],
           link_with    : nomar_lib,
                install : false
  )

  # Enable testing
  test('all', e)
else
  # Make users aware that without libcheck the test
  # does not run
  warning('Testing is disabled without \'libcheck\'')
endif
