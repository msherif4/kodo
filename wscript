#! /usr/bin/env python
# encoding: utf-8

import os

APPNAME = 'kodo'
VERSION = '11.0.0'

def recurse_helper(ctx, name):
    if not ctx.has_dependency_path(name):
        ctx.fatal('Load a tool to find %s as system dependency' % name)
    else:
        p = ctx.dependency_path(name)
        ctx.recurse(p)

def options(opt):

    import waflib.extras.wurf_dependency_bundle as bundle
    import waflib.extras.wurf_dependency_resolve as resolve
    import waflib.extras.wurf_configure_output

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'waf-tools',
            git_repository = 'github.com/steinwurf/external-waf-tools.git',
            major_version = 2))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'gtest',
            git_repository = 'github.com/steinwurf/external-gtest.git',
            major_version = 2))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'boost',
            git_repository = 'github.com/steinwurf/external-boost-light.git',
            major_version = 1))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'sak',
            git_repository = 'github.com/steinwurf/sak.git',
            major_version = 10))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'fifi',
            git_repository = 'github.com/steinwurf/fifi.git',
            major_version = 9))

    bundle.add_dependency(opt,
        resolve.ResolveGitMajorVersion(
            name = 'gauge',
            git_repository = 'github.com/steinwurf/cxx-gauge.git',
            major_version = 5))

    opt.load('wurf_dependency_resolve')
    opt.load('wurf_dependency_bundle')
    opt.load('wurf_tools')


def configure(conf):

    if conf.is_toplevel():

        conf.load('wurf_dependency_bundle')
        conf.load('wurf_tools')

        conf.load_external_tool('mkspec', 'wurf_cxx_mkspec_tool')
        conf.load_external_tool('runners', 'wurf_runner')
        conf.load_external_tool('install_path', 'wurf_install_path')
        conf.load_external_tool('project_gen', 'wurf_project_generator')

        recurse_helper(conf, 'boost')
        recurse_helper(conf, 'gtest')
        recurse_helper(conf, 'sak')
        recurse_helper(conf, 'fifi')
        recurse_helper(conf, 'gauge')

def build(bld):

    if bld.is_toplevel():

        bld.load('wurf_dependency_bundle')

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
        bld.recurse('examples/encode_decode_file')
        bld.recurse('examples/encode_decode_storage')
        bld.recurse('examples/encode_recode_decode_simple')
        bld.recurse('examples/decode_simple')
        bld.recurse('examples/encode_on_the_fly')
        bld.recurse('examples/rank_callback')
        bld.recurse('examples/use_cached_symbol_decoder')

        bld.recurse('benchmark/throughput')
        bld.recurse('benchmark/count_operations')
        bld.recurse('benchmark/overhead')
        bld.recurse('benchmark/decoding_probability')


    # Export own includes
    bld(includes = './src',
        export_includes = './src',
        name = 'kodo_includes')





