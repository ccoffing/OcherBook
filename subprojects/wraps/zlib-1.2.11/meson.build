project('zlib', 'c',
    version : '1.2.11',
    license : 'zlib'
)

src = files([
    'adler32.c',
    'crc32.c',
    'deflate.c',
    'infback.c',
    'inffast.c',
    'inflate.c',
    'inftrees.c',
    'trees.c',
    'zutil.c',
    'compress.c',
    'uncompr.c',
    'gzclose.c',
    'gzlib.c',
    'gzread.c',
    'gzwrite.c',
])

zlib = static_library('z', src,
    c_args : ['-Wno-implicit-function-declaration']
)

incdir = include_directories('.')

zlib_dep = declare_dependency(
    link_with : zlib,
    include_directories : incdir
)
