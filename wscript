#! /usr/bin/env python
# encoding: utf-8

import os

# Necessary since we override different Contexts
import waflib.extras.wurftools as wt

APPNAME = 'kodo'
VERSION = '1.0.0'

wt.add_dependency('sak', 'git://github.com/steinwurf/sak.git', '1.0.0')

wt.add_dependency('fifi', 'git://github.com/steinwurf/fifi.git', '1.0.1')

wt.add_dependency('boost', 'git://github.com/steinwurf/external-boost.git',
                  '1.1.0-boost_1_48_0')

wt.add_dependency('gtest', 'git://github.com/steinwurf/external-gtest.git',
                  '1.0.1-gtest_1_6_0')


def load_helper(ctx, name):
    if ctx.is_system_dependency(name):
        ctx.fatal('Load a tool to find %s as system dependency' % name)
    else:
        ctx.load_dependency(name)

def options(opt):

    opt.load('wurftools')

    load_helper(opt, 'sak')
    load_helper(opt, 'fifi')
    load_helper(opt, 'boost')
    load_helper(opt, 'gtest')


def configure(conf):

    conf.load('wurftools')

    load_helper(conf, 'sak')
    load_helper(conf, 'fifi')
    load_helper(conf, 'boost')
    load_helper(conf, 'gtest')

    if conf.env.TOOLCHAIN == 'linux':
        conf.env.CXXFLAGS_KODO_FLAGS = ['-O2', '-g', '-ftree-vectorize',
                                        '-Wextra', '-Wall']

    if conf.env.TOOLCHAIN == 'win32':
        conf.env.CXXFLAGS_KODO_FLAGS = ['/O2', '/Ob2', '/W3', '/EHsc']



def build(bld):

    bld.load('wurftools')

    load_helper(bld, 'sak')
    load_helper(bld, 'fifi')
    load_helper(bld, 'boost')
    load_helper(bld, 'gtest')

    # Export own includes
    bld(includes = '.',
        export_includes = '.',
        name = 'kodo_includes')

    bld.recurse('test')
    #bld.recurse('benchmark/throughput')




