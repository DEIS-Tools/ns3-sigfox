# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

# def options(opt):
#     pass

# def configure(conf):
#     conf.check_nonfatal(header_name='stdint.h', define_name='HAVE_STDINT_H')

def build(bld):
    module = bld.create_ns3_module('sigfox', ['core', 'network',
                                              'propagation', 'mobility',
                                              'point-to-point', 'energy'])
    module.source = [
        'model/sigfox-net-device.cc',
        'model/sigfox-mac.cc',
        'model/sigfox-phy.cc',
        'model/sigfox-channel.cc',
        'model/sigfox-interference-helper.cc',
        'model/gateway-sigfox-mac.cc',
        'model/end-point-sigfox-mac.cc',
        'model/gateway-sigfox-phy.cc',
        'model/end-point-sigfox-phy.cc',
        'model/simple-end-point-sigfox-phy.cc',
        'model/simple-gateway-sigfox-phy.cc',
        'model/sub-band.cc',
        'model/logical-sigfox-channel-helper.cc',
        'model/periodic-sender.cc',
        'model/forwarder.cc',
        'model/sigfox-mac-header.cc',
        'model/sigfox-tag.cc',
        'model/sigfox-radio-energy-model.cc',
        'model/sigfox-tx-current-model.cc',
        'model/sigfox-utils.cc',
        'helper/sigfox-radio-energy-model-helper.cc',
        'helper/sigfox-helper.cc',
        'helper/sigfox-phy-helper.cc',
        'helper/sigfox-mac-helper.cc',
        'helper/periodic-sender-helper.cc',
        'helper/forwarder-helper.cc',
        ]

    module_test = bld.create_ns3_module_test_library('sigfox')
    module_test.source = [
        ]

    headers = bld(features='ns3header')
    headers.module = 'sigfox'
    headers.source = [
        'model/sigfox-net-device.h',
        'model/sigfox-mac.h',
        'model/sigfox-phy.h',
        'model/sigfox-channel.h',
        'model/sigfox-interference-helper.h',
        'model/gateway-sigfox-mac.h',
        'model/end-point-sigfox-mac.h',
        'model/gateway-sigfox-phy.h',
        'model/end-point-sigfox-phy.h',
        'model/simple-end-point-sigfox-phy.h',
        'model/simple-gateway-sigfox-phy.h',
        'model/sub-band.h',
        'model/logical-sigfox-channel-helper.h',
        'model/periodic-sender.h',
        'model/forwarder.h',
        'model/sigfox-mac-header.h',
        'model/sigfox-tag.h',
        'model/sigfox-radio-energy-model.h',
        'model/sigfox-tx-current-model.h',
        'model/sigfox-utils.h',
        'helper/sigfox-radio-energy-model-helper.h',
        'helper/sigfox-helper.h',
        'helper/sigfox-phy-helper.h',
        'helper/sigfox-mac-helper.h',
        'helper/periodic-sender-helper.h',
        'helper/forwarder-helper.h',
        ]

    if bld.env.ENABLE_EXAMPLES:
        bld.recurse('examples')

    # Comment to disable python bindings
    # bld.ns3_python_bindings()
