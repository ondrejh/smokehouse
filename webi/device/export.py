#!/usr/bin/env python3

import re
from subprocess import call


FILENAME = 'webi.h'
DESTINATION = '../../sketch/web_server/'


def get_var_name(fname):

    return fname.replace('-', '_').replace('/', '_').split('.')[0]


def import_html(fname):

    varname = get_var_name(fname)
    sout = 'const char *{}_name PROGMEM = "/{}";\n'.format(varname, fname)
    sout += 'const char *{}_html PROGMEM = "\\\n'.format(varname)
    for line in open(fname, 'r'):
        line = line.replace('"', '\\"')
        line = line.rstrip()
        line += '\\n\\\n'
        sout += line
    sout += '";\n\n'
    return sout


def import_bin(fname, line_bytes=16, end=b'\x00'):

    varname = get_var_name(fname)
    fcont = open(fname, 'rb').read()
    if end is not None:
        fcont += end
    flen = len(fcont)
    sout = '#define {}_LEN {}\n'.format(varname.upper(), flen)
    sout += 'const char *{}_name PROGMEM = "/{}";\n'.format(varname, fname)
    sout += 'const char {}_bin[{}_LEN] PROGMEM = {}\n'.format(varname, varname.upper(), '{')
    first = True
    cnt = 0
    for b in fcont:
        if first:
            first = False
        else:
            sout += ',{}'.format('\n' if ((cnt % line_bytes) == 0) else ' ')
        sout += '0x{:02X}'.format(b)
        cnt += 1
    sout += '};\n\n'
    return sout


HEADER = FILENAME.upper().replace('.','_')


with open(FILENAME, 'w') as fout:

    print('Creating {} header file'.format(FILENAME))
    fout.write('#ifndef __{0}__\n#define __{0}__\n\n'.format(HEADER))
    print('  Embed html file index.h')
    fout.write(import_html('index.html'))
    print('  Embed html file config.html')
    fout.write(import_html('config.html'))
    print('  Embed binary css file style.css')
    fout.write(import_bin('style.css'))#, end=None))
    print('  Embed binary favicon.ico')
    fout.write(import_bin('favicon.ico', end=None))
    print('  Embed binary script.js')
    fout.write(import_bin('script.js', end=None))
    fout.write('#endif __{}__\n'.format(HEADER))
    print('Done')

print()
print('Copy "webi.h" to "{}" .. '.format(DESTINATION), end='')
print('OK' if call(['cp', 'webi.h', DESTINATION]) == 0 else 'ERROR')

