## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_program('tcp_bug',
                                 ['lte','flow-monitor'])
    obj.source = 'tcp_bug.cc'

    obj = bld.create_ns3_program('lte',
                                 ['lte','flow-monitor'])
    obj.source = 'lte.cc'

    obj = bld.create_ns3_program('tcp-single-channel',
                                 ['lte','flow-monitor'])
    obj.source = 'tcp_single_channel.cc'
    obj = bld.create_ns3_program('udp-single-channel',
                                 ['lte','flow-monitor'])
    obj.source = 'udp_single_channel.cc'

    obj = bld.create_ns3_program('tcp-bbr',
                                 ['lte','flow-monitor'])
    obj.source = 'tcp_bbr.cc'

    obj = bld.create_ns3_program('udp-bottle',
                                 ['lte','flow-monitor'])
    obj.source = 'udp_bottle.cc'