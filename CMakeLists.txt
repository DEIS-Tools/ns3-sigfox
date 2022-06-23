build_lib(
  LIBNAME sigfox
  SOURCE_FILES
    helper/sigfox-radio-energy-model-helper.cc
    helper/periodic-sender-helper.cc
    helper/sigfox-mac-helper.cc
    helper/sigfox-helper.cc
    helper/sigfox-phy-helper.cc
    helper/forwarder-helper.cc
#    helper/sdc-energy-source-helper.cc
    model/sigfox-utils.cc
    model/gateway-sigfox-phy.cc
    model/forwarder.cc
    model/simple-gateway-sigfox-phy.cc
    model/simple-end-point-sigfox-phy.cc
    model/end-point-sigfox-mac.cc
    model/sigfox-radio-energy-model.cc
    model/gateway-sigfox-mac.cc
#    model/sdc-energy-source.cc
    model/sigfox-tag.cc
    model/sigfox-net-device.cc
    model/logical-sigfox-channel-helper.cc
    model/periodic-sender.cc
    model/sub-band.cc
    model/sigfox-channel.cc
    model/sigfox-mac.cc
    model/end-point-sigfox-phy.cc
    model/sigfox-mac-header.cc
    model/sigfox-interference-helper.cc
    model/sigfox-tx-current-model.cc
  HEADER_FILES
    model/sigfox-tx-current-model.h
    model/sigfox-interference-helper.h
    model/sigfox-mac-header.h
    model/gateway-sigfox-mac.h
    model/simple-gateway-sigfox-phy.h
    model/periodic-sender.h
    model/sigfox-mac.h
    model/forwarder.h
#    model/sdc-energy-source.h
    model/sub-band.h
    model/sigfox-channel.h
    model/sigfox-phy.cc
    model/sigfox-tag.h
    model/simple-end-point-sigfox-phy.h
    model/gateway-sigfox-phy.h
    model/sigfox-phy.h
    model/end-point-sigfox-mac.h
    model/sigfox-radio-energy-model.h
    model/end-point-sigfox-phy.h
    model/sigfox-net-device.h
    model/logical-sigfox-channel-helper.h
    model/sigfox-utils.h
#    helper/sdc-energy-source-helper.h
    helper/sigfox-helper.h
    helper/forwarder-helper.h
    helper/periodic-sender-helper.h
    helper/sigfox-phy-helper.h
    helper/sigfox-radio-energy-model-helper.h
    helper/sigfox-mac-helper.h
  LIBRARIES_TO_LINK
    ${libcore}
    ${libenergy}
    ${libmobility}
    ${libpropagation}
    ${libpoint-to-point}
)

