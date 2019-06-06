#/******************************************************************************
# * SConstruct
# *
# * Source of VieClus -- Vienna Graph Clustering.
# *
# ******************************************************************************
# * Copyright (C) 2018 Christian Schulz <christian.schulz@univie.ac.at>
# *
# * This program is free software: you can redistribute it and/or modify it
# * under the terms of the GNU General Public License as published by the Free
# * Software Foundation, either version 5 of the License, or (at your option)
# * any later version.
# *
# * This program is distributed in the hope that it will be useful, but WITHOUT
# * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# * more details.
# *
# * You should have received a copy of the GNU General Public License along with
# * this program.  If not, see <http://www.gnu.org/licenses/>.
# *****************************************************************************/



# scons build file for the KaHIP.
#
# You can build it in the following variants:
#
#   optimized            no debug symbols, no assertions, optimization.
#   optimized_output     no debug symbols, no assertions, optimization -- more output on console.
#
#   scons variant=${variant} program=${program}
import os
import platform
import sys

# Get the current platform.
SYSTEM = platform.uname()[0]
HOST = platform.uname()[1]

# Get shortcut to $HOME.
HOME = os.environ['HOME']

def GetEnvironment():
  """Get environment variables from command line and environment.

  Exits on errors.

  Returns
    Environment with the configuration from the command line.
  """
  opts = Variables()
  opts.Add('variant', 'the variant to build, optimized or optimized with output', 'debug')
  opts.Add('program', 'program or interface to compile', 'clustering')

  env = Environment(options=opts, ENV=os.environ)
  if not env['variant'] in ['optimized','optimized_output','debug']:
    print 'Illegal value for variant: %s' % env['variant']
    sys.exit(1)

  if not env['program'] in ['evaluator','evolutionary_clustering','graphchecker','library']:
    print 'Illegal value for program: %s' % env['program']
    sys.exit(1)

  # Special configuration for 64 bit machines.
  if platform.architecture()[0] == '64bit':
     env.Append(CPPFLAGS=['-DPOINTER64=1'])

  return env

# Get the common environment.
env = GetEnvironment()

# path to search for source files
env.Append(CPPPATH=['./app'])
env.Append(CPPPATH=['./lib'])
env.Append(CPPPATH=['./extern/KaHIP/extern/argtable-2.10/include'])
env.Append(CPPPATH=['./extern/KaHIP/lib'])
env.Append(CPPPATH=['./extern/KaHIP/app'])
env.Append(CPPPATH=['./extern/KaHIP/lib/tools'])
env.Append(CPPPATH=['./extern/KaHIP/lib/partition'])
env.Append(CPPPATH=['./extern/KaHIP/lib/io'])
env.Append(CPPPATH=['./extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/'])
env.Append(CPPPATH=['./extern/KaHIP/lib'])
env.Append(CPPPATH=['./lib'])
env.Append(CPPPATH=['./lib/clustering'])
env.Append(CPPPATH=['./lib/clustering'])
env.Append(CPPPATH=['./extern/KaHIP/lib/clustering'])
env.Append(CPPPATH=['./extern/KaHIP/lib/tools'])
env.Append(CPPPATH=['./extern/KaHIP/lib/partition'])
env.Append(CPPPATH=['./extern/KaHIP/lib/io'])
env.Append(CPPPATH=['./extern/KaHIP/lib/partition/uncoarsening/refinement/quotient_graph_refinement/flow_refinement/'])
env.Append(CPPPATH=['/usr/include/openmpi/'])

# we get compile errors with g++ 7.1,
# perhaps due to multiple compilers on the system
env.Replace(CXX='g++ -g')

conf = Configure(env)

if SYSTEM == 'Darwin':
        env.Append(CPPPATH=['/opt/local/include/','../KaHIP/include'])
        env.Append(LIBPATH=['/opt/local/lib/'])
        env.Append(LIBPATH=['/opt/local/lib/openmpi/'])
        # homebrew related paths
        env.Append(LIBPATH=['/usr/local/lib/'])
        env.Append(LIBPATH=['/usr/local/lib/openmpi/'])
        env.Append(LIBPATH=['../KaHIP/extern/argtable-2.10/maclib'])
        env.Append(LIBPATH=['./KaHIP/extern/argtable-2.10/maclib'])
else:
        env.Append(LIBPATH=['../KaHIP/extern/argtable-2.10/lib'])
        env.Append(LIBPATH=['./KaHIP/extern/argtable-2.10/lib'])
        env.Append(LIBPATH=['../../KaHIP/extern/argtable-2.10/lib'])

#by D. Luxen
#if not conf.CheckLibWithHeader('argtable2', 'argtable2.h', 'CXX'):
        #print "argtable library not found. Exiting"
        #Exit(-1)
#if not conf.CheckCXXHeader('mpi.h'):
        #print "openmpi header not found. Exiting"
        #Exit(-1)
#
#
env.Append(CXXFLAGS = '-fopenmp')

if "clang" in env['CC'] or "clang" in env['CXX']:
        if env['variant'] == 'optimized':
          env.Append(CXXFLAGS = '-DNDEBUG -Wall -funroll-loops -O3 -std=c++14')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -funroll-loops -std=c++14')
        elif env['variant'] == 'optimized_output':
          env.Append(CXXFLAGS = ' -DNDEBUG -funroll-loops -Wall -O3 -std=c++14')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -DKAFFPAOUTPUT  -std=c++14')
        else:
          env.Append(CXXFLAGS = ' -DNDEBUG -Wall -funroll-loops -O3 -std=c++14')
          env.Append(CCFLAGS  = '-O3  -DNDEBUG -funroll-loops -std=c++14 ')
          if SYSTEM != 'Darwin':
                env.Append(CXXFLAGS = '-march=native')
                env.Append(CCFLAGS  = '-march=native')

else:
        if env['variant'] == 'optimized':
          env.Append(CXXFLAGS = '-O3 -Wall -std=c++14 -DNDEBUG -DD_FILELOG_MAX_LEVEL=1 -funroll-loops -fno-stack-limit -fpermissive')
        elif env['variant'] == 'optimized_output':
          env.Append(CXXFLAGS = '-O3 -Wall -std=c++14 -DNDEBUG -DKAFFPAOUTPUT -DD_FILELOG_MAX_LEVEL=7 -funroll-loops -fno-stack-limit -fpermissive')
        else:
          env.Append(CXXFLAGS = '-O0 -Wall -std=c++14 -g -DKAFFPAOUTPUT -DD_FILELOG_MAX_LEVEL=7 -fpermissive')
          if SYSTEM != 'Darwin':
                env.Append(CXXFLAGS = '-march=native')
                env.Append(CCFLAGS  = '-march=native')

# Execute the SConscript.
SConscript('SConscript', exports=['env'],variant_dir=env['variant'], duplicate=False)

