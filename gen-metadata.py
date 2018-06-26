from itertools import chain
import os
from os import path
import subprocess
import sys
from termcolor import colored, cprint
from shutil import copy2, move

externals_path = "C:\\Home\\Workspace\\Externals";

if 'EXTERNALS' in os.environ:
    externals_path = os.environ['EXTERNALS']
gen_metadata_exec_path = path.join( externals_path, 'GenMetadata\\gen-metadata.exe')
sdl_header_path = path.join( externals_path, 'SDL2-2.0.5/include' )
glad_header_path = path.join( externals_path, 'GLAD/include' )
genmetadata_header_path = path.join( externals_path, 'GenMetadata/include' )
gen_metadata_command = '{} --no-report src mathlib {} -i generated -i {} -i {} --output generated'.format( gen_metadata_exec_path, genmetadata_header_path, sdl_header_path, glad_header_path )
expected_outputs = [ 'generated/type_db.cpp', 'generated/type_db.h' ]
cleanup = [ 'main.h' ]

sys.stdout.write( "Generating metadata ({})... ".format(gen_metadata_command) )
result = subprocess.run( gen_metadata_command, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
output_exists = True

for output in expected_outputs:
    if not path.exists( output ):
        sys.stdout.write( colored(' Failure\n', 'red' ) )
        sys.exit( -1 )

sys.stdout.write( colored(' Success\n', 'green' ) )

sys.stdout.write( "Cleaning up... " )
for cl in chain( cleanup ):
    try:
        os.remove( cl )
    except OSError as e:
        sys.stdout.write( colored(' Failure\n', 'red' ) )
        print( e )
        sys.exit( 1 )
sys.stdout.write( colored(' Success\n', 'green' ) )