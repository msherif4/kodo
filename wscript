#! /usr/bin/env python
# encoding: utf-8

import os

APPNAME = 'kodo'
VERSION = '2.0.1'

def recurse_helper(ctx, name):
    if not ctx.has_dependency_path(name):
        ctx.fatal('Load a tool to find %s as system dependency' % name)
    else:
        p = ctx.dependency_path(name)
        ctx.recurse(p)


def options(opt):

    opt.load('toolchain_cxx')
    opt.load('dependency_bundle')

    import waflib.extras.dependency_bundle as bundle
    import waflib.extras.dependency_resolve as resolve

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'gtest',
            git_repository = 'git://github.com/steinwurf/external-gtest.git',
            major_version = 1))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'boost',
            git_repository = 'git://github.com/steinwurf/external-boost.git',
            major_version = 1))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'sak',
            git_repository = 'git://github.com/steinwurf/sak.git',
            major_version = 2))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'fifi',
            git_repository = 'git://github.com/steinwurf/fifi.git',
            major_version = 2))

    bundle.add_dependency(opt,
        resolve.ResolveGitFollowMaster(
            name = 'gauge',
            git_repository = 'git://github.com/steinwurf/cxx-gauge.git'))



def configure(conf):

    if conf.is_toplevel():

        conf.load('toolchain_cxx')
        conf.load('dependency_bundle')

        recurse_helper(conf, 'boost')
        recurse_helper(conf, 'gtest')
        recurse_helper(conf, 'sak')
        recurse_helper(conf, 'fifi')
        recurse_helper(conf, 'gauge')

def build(bld):

    if bld.is_toplevel():

        bld.load('dependency_bundle')

        recurse_helper(bld, 'boost')
        recurse_helper(bld, 'gtest')
        recurse_helper(bld, 'sak')
        recurse_helper(bld, 'fifi')
        recurse_helper(bld, 'gauge')

        # Only build test when executed from the
        # top-level wscript i.e. not when included as a dependency
        # in a recurse call

        bld.recurse('test')
        bld.recurse('examples/encode_decode_simple')

        bld.recurse('benchmark/throughput')
        bld.recurse('benchmark/count_operations')

    # Export own includes
    bld(includes = '.',
        export_includes = '.',
        name = 'kodo_includes')







