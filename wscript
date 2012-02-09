#! /usr/bin/env python
# encoding: utf-8

import os

# Necessary since we override different Contexts 
import waflib.extras.wurftools

APPNAME = 'kodo'
VERSION = '0.9'

def options(opt):
    opt.load('wurftools')

    opt.add_dependency('sak', 'git://github.com/steinwurf/sak.git')
    opt.add_dependency('fifi', 'git://github.com/steinwurf/fifi.git')
    opt.add_dependency('boost', 'git://github.com/steinwurf/external-waf-boost.git')
    opt.add_dependency('gtest', 'git://github.com/steinwurf/external-waf-gtest.git', 'v1.6.0')

    if not opt.bundle_dependency('sak'):
        opt.add_option('--sak-path')
    else:
        opt.recurse_dependency('sak')

    if not opt.bundle_dependency('fifi'):
        opt.add_option('--fifi-path')
    else:
        opt.recurse_dependency('fifi')

    if not opt.bundle_dependency('gtest'):
        opt.add_option('--gtest-path')
    else:
        opt.recurse_dependency('gtest')

    if not opt.bundle_dependency('boost'):
        opt.add_option('--boost-path')
    else:
        opt.recurse_dependency('boost')

def configure(conf):

    conf.load('wurftools')

    # Sak
    if not conf.bundle_dependency('sak'):
        if not conf.options.sak_path:
            conf.fatal('Either bundle sak of specify its path')
        else:
            conf.recurse(conf.options.sak_path)
            conf.env.SAK_PATH = conf.options.sak_path
    else:
        conf.recurse_dependency('sak')

    # Fifi
    if not conf.bundle_dependency('fifi'):
        if not conf.options.fifi_path:
            conf.fatal('Either bundle fifi of specify its path')
        else:
            conf.recurse(conf.options.fifi_path)
            conf.env.FIFI_PATH = conf.options.fifi_path
    else:
        conf.recurse_dependency('fifi')

    # Gtest
    if not conf.bundle_dependency('gtest'):
        if not conf.options.gtest_path:
            conf.fatal('Either bundle gtest of specify its path')
        else:
            conf.recurse(conf.options.gtest_path)
            conf.env.GTEST_PATH = conf.options.gtest_path
    else:
        conf.recurse_dependency('gtest')

    # Boost
    if not conf.bundle_dependency('boost'):
        if not conf.options.boost_path:
            conf.fatal('Either bundle boost of specify its path')
        else:
            conf.recurse(conf.options.boost_path)
            conf.env.BOOST_PATH = conf.options.boost_path
    else:
        conf.recurse_dependency('boost')


def build(bld):

    bld.load('wurftools')

    # Sak
    if not bld.bundle_dependency('sak'):
        if not bld.env.SAK_PATH: bld.fatal('Either bundle sak of specify'
                                           'its path during "./waf configure"')
        else: bld.recurse(bld.env.SAK_PATH)
    else:
        bld.recurse_dependency('sak')


    # Fifi
    if not bld.bundle_dependency('fifi'):
        if not bld.env.FIFI_PATH: bld.fatal('Either bundle fifi of specify'
                                            'its path during "./waf configure"')
        else: bld.recurse(bld.env.FIFI_PATH)
    else:
        bld.recurse_dependency('fifi')


    # Gtest
    if not bld.bundle_dependency('gtest'):
        if not bld.env.GTEST_PATH: bld.fatal('Either bundle gtest of specify'
                                             'its path during "./waf configure"')
        else: bld.recurse(bld.env.GTEST_PATH)
    else:
        bld.recurse_dependency('gtest')

    # Boost
    if not bld.bundle_dependency('boost'):
        if not bld.env.BOOST_PATH: bld.fatal('Either bundle boost of specify'
                                             'its path during "./waf configure"')
        else: bld.recurse(bld.env.BOOST_PATH)

    else:
        bld.recurse_dependency('boost')


    # Export own includes
    bld(includes = '.',
        export_includes = '.',
        name = 'kodo_includes')
    
    bld.recurse('test')
    #bld.recurse('benchmark/throughput')


    
        
