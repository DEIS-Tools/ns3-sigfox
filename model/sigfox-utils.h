/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Author: Davide Magrin <davide@magr.in>
 *
 * Modified by: Michele Albano <mialb@cs.aau.dk>
 */

#ifndef SIGFOX_UTILS_H
#define SIGFOX_UTILS_H

#include "ns3/nstime.h"
#include "ns3/uinteger.h"

namespace ns3 {
namespace sigfox {

/**
 * Convert from dBm to Watts.
 *
 * \param dbm the power in dBm
 *
 * \return the equivalent Watts for the given dBm
 */
double DbmToW (double dbm);
/**
 * Convert from dB to ratio.
 *
 * \param db
 *
 * \return ratio
 */
double DbToRatio (double db);
/**
 * Convert from Watts to dBm.
 *
 * \param w the power in Watts
 *
 * \return the equivalent dBm for the given Watts
 */
double WToDbm (double w);
/**
 * Convert from ratio to dB.
 *
 * \param ratio
 *
 * \return dB
 */
double RatioToDb (double ratio);

}   // namespace ns3

}
#endif /* SIGFOX_UTILS_H */
