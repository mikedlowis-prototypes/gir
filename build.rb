#!/usr/bin/env ruby
require './modules/build-system/setup'

#------------------------------------------------------------------------------
# Environment Definitions
#------------------------------------------------------------------------------
# Define the default compiler environment
main_env = BuildEnv.new do |env|
  env["CFLAGS"]  += ['-O3', '-Wall', '-Wextra', '--std=c99', '--pedantic']
  env["CPPPATH"] += ['source/'] + Dir['modules/atf/source/**/']
end

sources = Dir['source/**/*.c'] - ['source/main.c']
test_sources = Dir['tests/**/*.c'] + ['modules/atf/source/atf.c']

#------------------------------------------------------------------------------
# Release Build Targets
#------------------------------------------------------------------------------
# Build the language library
main_env.Library('libgir.a', sources)

# Build the interpreter
main_env.Program('gir',  ['source/main.c', 'libgir.a'])

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
if Opts[:profile].include? "test"
  # Do nothing for now
    main_env.Program('test_gir', test_sources + ['libgir.a'])
    main_env.Command('Unit Tests', 'test_gir', "CMD" => ['./test_gir'])
end
