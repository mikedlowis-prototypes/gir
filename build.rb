#!/usr/bin/env ruby
require './modules/build-system/setup'

#------------------------------------------------------------------------------
# Environment Definitions
#------------------------------------------------------------------------------
# Define the default compiler environment
main_env = BuildEnv.new do |env|
  env["CFLAGS"]  += ['-Wall', '-Wextra', '--std=c99', '--pedantic']
  env["CPPPATH"] += Dir['modules/libcds/source/**/'] + [ ]
end

#------------------------------------------------------------------------------
# Release Build Targets
#------------------------------------------------------------------------------
# Build third party libraries
main_env.Library('build/lib/libcds.a', FileList['modules/libcds/source/**/*.c'])
runtime_libs = ['build/lib/libcds.a']

# Build the parser
main_env.CFile('source/lex.yy.c', 'source/lexer.l')
main_env.Program('parser', FileList['source/*.c'] + ['source/lex.yy.c'] + runtime_libs)

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
if Opts[:profile].include? "test"
#  compiler_libs = ['build/lib/libparse-test.a'] + runtime_libs
#  test_env.Library('build/lib/libparse-test.a', FileList['source/libparse/*.c'])
#  test_env.Program('build/bin/sclpl-test', FileList['source/sclpl/*.c'] + compiler_libs)
#  test_env.Command('RSpec', [], 'CMD' => [
#      'rspec', '--pattern', 'spec/**{,/*/**}/*_spec.rb', '--format', 'documentation'])
end
