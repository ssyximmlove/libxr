#pragma once

#include <linux/can.h>
#include <linux/can/error.h>

#include <cstdint>

#include "can.hpp"

namespace LibXR::LinuxCANDetail
{
/// 内部协议错误解码，供驱动和无硬件回归使用 / Internal decoder shared with unit tests.
inline CAN::ErrorID ProtocolErrorID(const canfd_frame& frame, uint8_t length)
{
  const uint8_t prot_bits = length >= 3 ? frame.data[2] : 0;
  if ((prot_bits & CAN_ERR_PROT_STUFF) != 0)
  {
    return CAN::ErrorID::CAN_ERROR_ID_STUFF;
  }
  if ((prot_bits & CAN_ERR_PROT_FORM) != 0)
  {
    return CAN::ErrorID::CAN_ERROR_ID_FORM;
  }
  if ((prot_bits & CAN_ERR_PROT_BIT1) != 0)
  {
    return CAN::ErrorID::CAN_ERROR_ID_BIT1;
  }
  if ((prot_bits & CAN_ERR_PROT_BIT0) != 0)
  {
    return CAN::ErrorID::CAN_ERROR_ID_BIT0;
  }
  if ((prot_bits & (CAN_ERR_PROT_BIT | CAN_ERR_PROT_OVERLOAD)) != 0)
  {
    return CAN::ErrorID::CAN_ERROR_ID_OTHER;
  }
  return CAN::ErrorID::CAN_ERROR_ID_PROTOCOL;
}
}  // namespace LibXR::LinuxCANDetail
