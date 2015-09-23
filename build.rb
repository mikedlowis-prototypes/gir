#!/usr/bin/env ruby
require './modules/build-system/setup'

#------------------------------------------------------------------------------
# Environment Definitions
#------------------------------------------------------------------------------
# Define the default compiler environment
main_env = BuildEnv.new do |env|
  env["CFLAGS"]  += ['-O3', '-Wall', '-Wextra', '--std=c99', '--pedantic']
  env["CPPPATH"] += Dir['modules/libc/source/**/']
end

#------------------------------------------------------------------------------
# Release Build Targets
#------------------------------------------------------------------------------
# Build third party libraries
#main_env.Library('build/lib/libc.a',   FileList['modules/libc/source/**/*.c'])
runtime_libs = []#['build/lib/libc.a']

# Build the parser
main_env.Program('gir',  FileList['source/*.c'] + runtime_libs)

#------------------------------------------------------------------------------
# Test Build Targets
#------------------------------------------------------------------------------
if Opts[:profile].include? "test"
  # Do nothing for now
end
